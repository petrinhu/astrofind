#include "AboutDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFont>
#include <QApplication>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About AstroFind"));
    setFixedSize(420, 320);
    setModal(true);

    auto* lay = new QVBoxLayout(this);
    lay->setSpacing(12);
    lay->setContentsMargins(24, 24, 24, 16);

    // Title
    auto* titleLabel = new QLabel("AstroFind", this);
    QFont tf = titleLabel->font();
    tf.setPointSize(22);
    tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color:#4488ff;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto* versionLabel = new QLabel(
        tr("Version %1").arg(QApplication::applicationVersion()), this);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color:#8899aa;");

    auto* descLabel = new QLabel(
        tr("Modern asteroid detection software for citizen science.\n"
           "Compatible with the IASC / MPC workflow.\n"
           "Outputs ADES 2022 format reports."), this);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet("color:#aabbcc; margin-top:8px;");

    auto* sep = new QLabel(this);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background:#334455;");

    auto* credLabel = new QLabel(
        tr("Inspired by Astrometrica by Herbert Raab.\n"
           "Built with Qt6, cfitsio, Eigen3, SEP.\n"
           "Released under the MIT License."), this);
    credLabel->setWordWrap(true);
    credLabel->setAlignment(Qt::AlignCenter);
    credLabel->setStyleSheet("color:#667788; font-size:10px;");

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);

    lay->addWidget(titleLabel);
    lay->addWidget(versionLabel);
    lay->addWidget(descLabel);
    lay->addWidget(sep);
    lay->addWidget(credLabel);
    lay->addStretch();
    lay->addWidget(bb);
}
