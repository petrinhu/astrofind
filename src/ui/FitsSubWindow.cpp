#include "FitsSubWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QMenu>
#include <QContextMenuEvent>
#include <QKeyEvent>
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
    setFocusProxy(view_);

    // ── HDU navigation bar ────────────────────────────────────────────────────
    // Scan HDUs upfront; show bar only when there are multiple image extensions.
    if (!img_.filePath.isEmpty())
        hduList_ = core::scanImageHdus(img_.filePath);
    buildHduBar();   // creates hduBar_ (hidden if single-HDU)

    // ── Info bar ──────────────────────────────────────────────────────────────
    infoBar_ = new QLabel(this);
    infoBar_->setObjectName("imageInfoBar");
    infoBar_->setStyleSheet("QLabel#imageInfoBar { background:#1a1a2e; color:#88aadd; "
                            "padding:1px 6px; font-size:9px; }");
    infoBar_->setFixedHeight(16);
    infoBar_->setWordWrap(false);
    infoBar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    refreshInfoBar();

    lay->addWidget(view_);
    if (hduBar_) lay->addWidget(hduBar_);
    lay->addWidget(infoBar_);

    connect(view_, &FitsImageView::cursorMoved,    this, &FitsSubWindow::cursorMoved);
    connect(view_, &FitsImageView::regionSelected, this, &FitsSubWindow::regionSelected);

    setWindowTitle(img_.fileName);
    setAttribute(Qt::WA_DeleteOnClose);
}

// ─── buildHduBar ─────────────────────────────────────────────────────────────

void FitsSubWindow::buildHduBar()
{
    if (hduList_.size() <= 1) return;   // nothing to navigate

    hduBar_ = new QWidget(this);
    hduBar_->setObjectName("hduNavBar");
    hduBar_->setStyleSheet(
        "QWidget#hduNavBar { background:#0f1520; }"
        "QPushButton { background:#1e2a40; color:#88aadd; border:none; "
        "              padding:1px 6px; font-size:10px; min-width:20px; }"
        "QPushButton:hover { background:#2a3a55; }"
        "QComboBox { background:#1e2a40; color:#88aadd; border:none; "
        "            font-size:10px; padding:0 4px; }"
        "QComboBox::drop-down { border:none; width:16px; }");
    hduBar_->setFixedHeight(20);

    auto* hlay = new QHBoxLayout(hduBar_);
    hlay->setContentsMargins(4, 0, 4, 0);
    hlay->setSpacing(2);

    hduPrev_ = new QPushButton(QStringLiteral("‹"), hduBar_);
    hduNext_ = new QPushButton(QStringLiteral("›"), hduBar_);
    hduCombo_ = new QComboBox(hduBar_);
    hduCombo_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    for (const auto& h : hduList_) {
        hduCombo_->addItem(
            QStringLiteral("%1: %2  (%3×%4)")
                .arg(h.hduNumber)
                .arg(h.name)
                .arg(h.width)
                .arg(h.height));
    }

    hlay->addWidget(hduPrev_);
    hlay->addWidget(hduCombo_);
    hlay->addWidget(hduNext_);

    // Select the combo entry matching the currently-loaded HDU
    // (for multi-ext RGB, img_ has isColor=true but we default to entry 0)
    hduCombo_->setCurrentIndex(0);

    connect(hduCombo_, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &FitsSubWindow::onHduSelected);
    connect(hduPrev_, &QPushButton::clicked, this, [this]() {
        const int idx = hduCombo_->currentIndex();
        if (idx > 0) hduCombo_->setCurrentIndex(idx - 1);
    });
    connect(hduNext_, &QPushButton::clicked, this, [this]() {
        const int idx = hduCombo_->currentIndex();
        if (idx < hduCombo_->count() - 1) hduCombo_->setCurrentIndex(idx + 1);
    });
}

// ─── onHduSelected ───────────────────────────────────────────────────────────

void FitsSubWindow::onHduSelected(int comboIndex)
{
    if (comboIndex < 0 || comboIndex >= hduList_.size()) return;
    const int hduNum = hduList_[comboIndex].hduNumber;

    auto result = core::loadFitsHdu(img_.filePath, hduNum);
    if (!result) return;   // silently ignore errors (file may have changed on disk)

    // Preserve overlay data and WCS if already reduced
    result->detectedStars = img_.detectedStars;
    result->catalogStars  = img_.catalogStars;
    result->kooObjects    = img_.kooObjects;
    result->wcs           = img_.wcs;

    img_ = std::move(*result);
    view_->setFitsImage(img_);
    refreshInfoBar();
    hduPrev_->setEnabled(comboIndex > 0);
    hduNext_->setEnabled(comboIndex < hduList_.size() - 1);
}

// ─── refreshInfoBar ──────────────────────────────────────────────────────────

void FitsSubWindow::refreshInfoBar()
{
    const QString name = img_.objectName.isEmpty() ? img_.fileName : img_.objectName;
    QString text = QStringLiteral("%1  |  %2×%3  |  JD %4  |  %5 s")
        .arg(name)
        .arg(img_.width).arg(img_.height)
        .arg(img_.jd, 0, 'f', 4)
        .arg(img_.expTime, 0, 'f', 1);
    if (hduList_.size() > 1 && hduCombo_) {
        const int idx = hduCombo_->currentIndex();
        if (idx >= 0 && idx < hduList_.size())
            text.prepend(QStringLiteral("HDU %1/%2  |  ")
                .arg(hduList_[idx].hduNumber).arg(hduList_.size()));
    }
    infoBar_->setText(text);
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

void FitsSubWindow::keyPressEvent(QKeyEvent* e)
{
    // Call processKey directly — avoids routing through QApplication::notify,
    // which would traverse the parent chain and cause infinite recursion.
    view_->processKey(e);
    if (!e->isAccepted()) QWidget::keyPressEvent(e);
}

void FitsSubWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    menu.addAction(tr("Export as JPEG/PNG…"),    this, &FitsSubWindow::exportImageRequested);
    menu.addSeparator();
    menu.addAction(tr("Apply Dark Frame…"),      this, &FitsSubWindow::applyDarkRequested);
    menu.addAction(tr("Apply Flat Field…"),      this, &FitsSubWindow::applyFlatRequested);
    menu.addSeparator();
    menu.addAction(tr("Show Histogram"),         this, &FitsSubWindow::showHistogramRequested);
    menu.addAction(tr("Show Power Spectrum"),    this, &FitsSubWindow::showPowerSpectrumRequested);
    if (img_.cubeDepth > 3) {
        menu.addAction(tr("Animate Cube (%1 frames)…").arg(img_.cubeDepth),
                       this, &FitsSubWindow::showCubeAnimationRequested);
    }
    menu.addAction(tr("Show Image Catalog"),     this, &FitsSubWindow::showImageCatalogRequested);
    menu.addSeparator();
    menu.addAction(tr("FITS Header…"),           this, &FitsSubWindow::showFitsHeader);

    menu.exec(event->globalPos());
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
