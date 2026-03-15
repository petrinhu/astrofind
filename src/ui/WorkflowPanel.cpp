#include "WorkflowPanel.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QFont>
#include <QSizePolicy>

WorkflowPanel::WorkflowPanel(QWidget* parent)
    : QWidget(parent)
{
    setMinimumWidth(180);
    setMaximumWidth(220);

    layout_ = new QVBoxLayout(this);
    layout_->setSpacing(4);
    layout_->setContentsMargins(8, 12, 8, 8);

    auto* title = new QLabel(tr("WORKFLOW"), this);
    QFont f = title->font();
    f.setPointSize(8);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    f.setBold(true);
    title->setFont(f);
    title->setStyleSheet("color: #6688aa;");
    layout_->addWidget(title);

    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #334455;");
    layout_->addWidget(line);
    layout_->addSpacing(4);

    buildStepRow(0, "1", tr("Load Images"),         tr("Open FITS files"));
    buildStepRow(1, "2", tr("Data Reduction"),       tr("Solve plate"));
    buildStepRow(2, "3", tr("Known Object Overlay"), tr("Mark known objects"));
    buildStepRow(3, "4", tr("Blink Images"),         tr("Find moving objects"));
    buildStepRow(4, "5", tr("Measure Objects"),      tr("Click to measure"));
    buildStepRow(5, "6", tr("ADES Report"),          tr("Generate & submit"));

    layout_->addStretch();

    setStep(core::SessionStep::Idle);
}

void WorkflowPanel::buildStepRow(int index, const QString& number,
                                  const QString& name, const QString& desc)
{
    auto* row = new QWidget(this);
    row->setObjectName(QString("stepRow%1").arg(index));
    auto* rl = new QVBoxLayout(row);
    rl->setContentsMargins(6, 6, 6, 6);
    rl->setSpacing(2);

    auto* top = new QWidget(row);
    auto* tl  = new QHBoxLayout(top);
    tl->setContentsMargins(0,0,0,0);
    tl->setSpacing(6);

    auto& sw = steps_[index];

    sw.icon = new QLabel("○", top);
    sw.icon->setFixedWidth(16);
    sw.icon->setAlignment(Qt::AlignCenter);

    auto* numLabel = new QLabel(number + ".", top);
    numLabel->setStyleSheet("font-weight:bold; color:#557799;");

    sw.name = new QLabel(name, top);
    sw.name->setStyleSheet("font-weight:bold;");

    tl->addWidget(sw.icon);
    tl->addWidget(numLabel);
    tl->addWidget(sw.name);
    tl->addStretch();

    sw.status = new QLabel(desc, row);
    sw.status->setStyleSheet("color:#667788; font-size:9px; padding-left:22px;");

    sw.btn = new QPushButton(tr("▶ Do it"), row);
    sw.btn->setObjectName(QString("stepBtn%1").arg(index));
    sw.btn->setFixedHeight(24);
    sw.btn->hide();
    connect(sw.btn, &QPushButton::clicked, this, [this, index] {
        emit stepActionRequested(static_cast<core::SessionStep>(index + 1));
    });

    rl->addWidget(top);
    rl->addWidget(sw.status);
    rl->addWidget(sw.btn);

    layout_->addWidget(row);

    // Subtle separator
    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#1e2a36;");
    layout_->addWidget(sep);
}

void WorkflowPanel::setStep(core::SessionStep step)
{
    currentStep_ = step;
    updateStepStates(step);
}

void WorkflowPanel::updateStepStates(core::SessionStep current)
{
    const int cur = static_cast<int>(current);

    for (int i = 0; i < kStepCount; ++i) {
        auto& sw = steps_[i];
        const int stepVal = i + 1; // steps 1-6 correspond to SessionStep 1-6

        if (cur > stepVal) {
            // Done
            sw.icon->setText("✓");
            sw.icon->setStyleSheet("color:#44cc88; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#44cc88;");
            sw.btn->hide();
        } else if (cur == stepVal) {
            // Active
            sw.icon->setText("▶");
            sw.icon->setStyleSheet("color:#4488ff; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#4488ff;");
            sw.btn->show();
        } else {
            // Pending
            sw.icon->setText("○");
            sw.icon->setStyleSheet("color:#445566; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#445566;");
            sw.btn->hide();
        }
    }
}
