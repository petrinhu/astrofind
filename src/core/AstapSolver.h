#pragma once

#include "FitsImage.h"
#include <QObject>
#include <QString>

class QProcess;

namespace core {

/// Local plate solver using the ASTAP command-line tool.
///
/// Emits the same signals as AstrometryClient so it can be used interchangeably.
/// Workflow: setAstapPath() → solve() → listen to progress/solved/failed.
class AstapSolver : public QObject {
    Q_OBJECT
public:
    explicit AstapSolver(QObject* parent = nullptr);
    ~AstapSolver() override;

    void setAstapPath(const QString& path) { astapPath_ = path; }
    const QString& astapPath() const              { return astapPath_; }

    /// Start an async plate solve.
    /// raHint/decHint are initial position guesses in degrees (0 = no hint → blind solve).
    /// radiusDeg is the search radius around the hint.
    void solve(const QString& fitsPath,
               double raHint    = 0.0,
               double decHint   = 0.0,
               double radiusDeg = 180.0);

    bool isBusy() const noexcept;
    void cancel();

signals:
    void progress(const QString& message, int percent);
    void solved(const core::PlateSolution& wcs, int jobId);
    void failed(const QString& reason);

private slots:
    void onProcessFinished(int exitCode, int exitStatus);

private:
    void fail(const QString& reason);

    QString   astapPath_;
    QProcess* process_         = nullptr;
    QString   pendingFitsPath_;
};

} // namespace core
