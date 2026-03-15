#include "FitsSubWindow.h"

#include <QVBoxLayout>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDateTime>
#include <fitsio.h>

FitsSubWindow::FitsSubWindow(const core::FitsImage& img, QWidget* parent)
    : QWidget(parent)
    , img_(img)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    view_ = new FitsImageView(this);
    view_->setFitsImage(img_);

    infoBar_ = new QLabel(this);
    infoBar_->setObjectName("imageInfoBar");
    infoBar_->setStyleSheet("QLabel#imageInfoBar { background:#1a1a2e; color:#88aadd; "
                            "padding:2px 6px; font-size:10px; }");
    infoBar_->setText(QString("%1   %2×%3   JD %4   %5 s")
        .arg(img_.objectName.isEmpty() ? img_.fileName : img_.objectName)
        .arg(img_.width).arg(img_.height)
        .arg(img_.jd, 0, 'f', 5)
        .arg(img_.expTime, 0, 'f', 1));

    lay->addWidget(view_);
    lay->addWidget(infoBar_);

    connect(view_, &FitsImageView::cursorMoved, this, &FitsSubWindow::cursorMoved);

    setWindowTitle(img_.fileName);
    setAttribute(Qt::WA_DeleteOnClose);
}

void FitsSubWindow::updateImage(const core::FitsImage& img)
{
    // Copy overlay and WCS from updated img (pixel buffer stays unchanged)
    img_.detectedStars = img.detectedStars;
    img_.catalogStars  = img.catalogStars;
    img_.kooObjects    = img.kooObjects;
    img_.wcs           = img.wcs;
    img_.pixScaleX     = img.pixScaleX;
    img_.pixScaleY     = img.pixScaleY;
    view_->setFitsImage(img_);
}

void FitsSubWindow::showFitsHeader()
{
    fitsfile* fptr = nullptr;
    int status = 0;
    if (fits_open_file(&fptr, img_.filePath.toLocal8Bit().constData(), READONLY, &status))
        return;

    int nkeys = 0;
    fits_get_hdrspace(fptr, &nkeys, nullptr, &status);

    QString text;
    char card[FLEN_CARD];
    for (int i = 1; i <= nkeys; ++i) {
        status = 0;
        fits_read_record(fptr, i, card, &status);
        if (status == 0)
            text += QString::fromLatin1(card) + "\n";
    }
    fits_close_file(fptr, &status);

    auto* dlg = new QDialog(this);
    dlg->setWindowTitle(tr("FITS Header — %1").arg(img_.fileName));
    dlg->resize(700, 500);
    auto* lay  = new QVBoxLayout(dlg);
    auto* edit = new QTextEdit(dlg);
    edit->setReadOnly(true);
    edit->setFont(QFont("Monospace", 9));
    edit->setPlainText(text);
    auto* bb = new QDialogButtonBox(QDialogButtonBox::Close, dlg);
    lay->addWidget(edit);
    lay->addWidget(bb);
    connect(bb, &QDialogButtonBox::rejected, dlg, &QDialog::accept);
    dlg->exec();
    dlg->deleteLater();
}
