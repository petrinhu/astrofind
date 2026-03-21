#include "AstapSolver.h"

#include <fitsio.h>
#include <spdlog/spdlog.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

namespace core {

AstapSolver::AstapSolver(QObject* parent)
    : QObject(parent)
{}

AstapSolver::~AstapSolver()
{
    cancel();
}

bool AstapSolver::isBusy() const noexcept
{
    return process_ && process_->state() != QProcess::NotRunning;
}

void AstapSolver::solve(const QString& fitsPath,
                         double raHint, double decHint, double radiusDeg)
{
    if (isBusy()) {
        spdlog::warn("AstapSolver: already busy, ignoring request");
        return;
    }
    if (astapPath_.isEmpty()) {
        fail(tr("Caminho do executável ASTAP não configurado (Configurações → Conexões → ASTAP)."));
        return;
    }

    pendingFitsPath_ = fitsPath;

    QStringList args;
    args << QStringLiteral("-f") << fitsPath;
    // ASTAP takes RA in hours
    if (std::abs(raHint) > 0.001 || std::abs(decHint) > 0.001) {
        args << QStringLiteral("-ra")  << QString::number(raHint / 15.0, 'f', 6);
        args << QStringLiteral("-d")   << QString::number(decHint, 'f', 6);
        args << QStringLiteral("-r")   << QString::number(radiusDeg, 'f', 2);
    }
    args << QStringLiteral("-speed") << QStringLiteral("0");  // best quality

    spdlog::info("AstapSolver: {} {}", astapPath_.toStdString(),
                 args.join(' ').toStdString());

    process_ = new QProcess(this);
    process_->setProcessChannelMode(QProcess::MergedChannels);

    connect(process_, &QProcess::readyReadStandardOutput, this, [this]() {
        const QString line = QString::fromUtf8(process_->readAllStandardOutput()).trimmed();
        if (!line.isEmpty()) {
            spdlog::debug("ASTAP: {}", line.toStdString());
            emit progress(QStringLiteral("ASTAP: ") + line, 50);
        }
    });
    connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus status) {
        onProcessFinished(code, static_cast<int>(status));
    });

    emit progress(tr("Iniciando ASTAP…"), 5);
    process_->start(astapPath_, args);

    if (!process_->waitForStarted(5000)) {
        process_->deleteLater();
        process_ = nullptr;
        fail(tr("Não foi possível iniciar o ASTAP. Verifique o caminho: %1").arg(astapPath_));
    }
}

void AstapSolver::cancel()
{
    if (process_ && process_->state() != QProcess::NotRunning) {
        process_->kill();
        process_->waitForFinished(3000);
    }
    if (process_) {
        process_->deleteLater();
        process_ = nullptr;
    }
}

// ─── Private ─────────────────────────────────────────────────────────────────

void AstapSolver::onProcessFinished(int exitCode, int exitStatus)
{
    process_->deleteLater();
    process_ = nullptr;

    // ASTAP exit codes: 0 = solved, 1 = low star count, 2 = no solution, 3 = FITS error
    if (exitStatus != static_cast<int>(QProcess::NormalExit)) {
        fail(tr("ASTAP encerrado de forma inesperada"));
        return;
    }
    if (exitCode != 0) {
        const QString reason = (exitCode == 1)
            ? tr("ASTAP: estrelas insuficientes para resolver a placa")
            : (exitCode == 2)
            ? tr("ASTAP: nenhuma solução de placa encontrada")
            : tr("ASTAP falhou (código %1)").arg(exitCode);
        fail(reason);
        return;
    }

    // Read the .wcs output file (FITS format, produced alongside the input file)
    const QFileInfo fi(pendingFitsPath_);
    const QString wcsPath = fi.dir().filePath(fi.completeBaseName() + QStringLiteral(".wcs"));

    if (!QFile::exists(wcsPath)) {
        fail(tr("ASTAP: arquivo de solução não encontrado: %1").arg(wcsPath));
        return;
    }

    emit progress(tr("ASTAP: lendo solução WCS…"), 90);

    PlateSolution wcs;
    fitsfile* fptr  = nullptr;
    int       fstat = 0;

    if (fits_open_file(&fptr, wcsPath.toLocal8Bit().constData(), READONLY, &fstat) == 0) {
        char val[FLEN_VALUE]   = {};
        char cmt[FLEN_COMMENT] = {};

        auto getDbl = [&](const char* key) -> double {
            fstat = 0;
            if (fits_read_keyword(fptr, key, val, cmt, &fstat) == 0) {
                bool ok = false;
                const double v = QString::fromLatin1(val).trimmed().toDouble(&ok);
                return ok ? v : 0.0;
            }
            return 0.0;
        };

        wcs.crval1 = getDbl("CRVAL1");
        wcs.crval2 = getDbl("CRVAL2");
        wcs.crpix1 = getDbl("CRPIX1");
        wcs.crpix2 = getDbl("CRPIX2");
        wcs.cd1_1  = getDbl("CD1_1");
        wcs.cd1_2  = getDbl("CD1_2");
        wcs.cd2_1  = getDbl("CD2_1");
        wcs.cd2_2  = getDbl("CD2_2");
        wcs.solved = (wcs.cd1_1 != 0.0 || wcs.cd2_2 != 0.0);

        fits_close_file(fptr, &fstat);
    }

    // Clean up the temporary .wcs file
    QFile::remove(wcsPath);

    if (!wcs.solved) {
        fail(tr("ASTAP: não foi possível ler os coeficientes WCS de %1").arg(wcsPath));
        return;
    }

    spdlog::info("AstapSolver: solved! CRVAL=({:.4f}, {:.4f})",
                 wcs.crval1, wcs.crval2);
    emit progress(tr("ASTAP: placa resolvida!"), 100);
    emit solved(wcs, 0);
}

void AstapSolver::fail(const QString& reason)
{
    spdlog::error("AstapSolver: {}", reason.toStdString());
    emit failed(reason);
}

} // namespace core
