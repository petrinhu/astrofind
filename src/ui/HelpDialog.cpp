#include "HelpDialog.h"

#include <QTextBrowser>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFile>
#include <QLocale>
#include <QApplication>

HelpDialog::HelpDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("AstroFind Help"));
    setWindowFlag(Qt::Window);           // independent window, not sheet
    setAttribute(Qt::WA_DeleteOnClose, false);
    resize(900, 680);

    browser_ = new QTextBrowser(this);
    browser_->setOpenExternalLinks(true);
    browser_->setSearchPaths({ QStringLiteral(":/help") });

    // Language selector
    langBox_ = new QComboBox(this);
    langBox_->addItem(tr("English"),        QStringLiteral(":/help/help_en.html"));
    langBox_->addItem(tr("Português (BR)"), QStringLiteral(":/help/help_pt_br.html"));

    // Default to system locale
    const QString sysLang = QLocale::system().name();   // e.g. "pt_BR"
    if (sysLang.startsWith(QStringLiteral("pt")))
        langBox_->setCurrentIndex(1);

    connect(langBox_, &QComboBox::currentIndexChanged,
            this, &HelpDialog::onLanguageChanged);

    auto* topBar = new QHBoxLayout;
    topBar->addWidget(new QLabel(tr("Language:"), this));
    topBar->addWidget(langBox_);
    topBar->addStretch();

    auto* btnClose = new QPushButton(tr("Close"), this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::hide);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(4);
    lay->addLayout(topBar);
    lay->addWidget(browser_);
    lay->addWidget(btnClose);

    // Load initial page
    onLanguageChanged(langBox_->currentIndex());
}

void HelpDialog::onLanguageChanged(int index)
{
    loadPage(langBox_->itemData(index).toString());
}

void HelpDialog::loadPage(const QString& resourcePath)
{
    QFile f(resourcePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        browser_->setHtml(QStringLiteral(
            "<h2 style='color:#f55'>Help file not found</h2>"
            "<p>Resource: <code>%1</code></p>").arg(resourcePath));
        return;
    }
    browser_->setSearchPaths({ QStringLiteral("qrc:/help") });
    browser_->setHtml(QString::fromUtf8(f.readAll()));
}

void HelpDialog::jumpTo(const QString& anchor)
{
    show();
    raise();
    activateWindow();
    browser_->scrollToAnchor(anchor);
}
