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

    // ── Auto-workflow checkbox ────────────────────────────────────────────────
    auto* autoSep = new QFrame(this);
    autoSep->setFrameShape(QFrame::HLine);
    autoSep->setStyleSheet("color: #334455;");
    layout_->addWidget(autoSep);

    autoBox_ = new QCheckBox(tr("Fluxo automático"), this);
    autoBox_->setStyleSheet("color:#8899aa; font-size:9px;");
    autoBox_->setToolTip(tr("Executa as fases 2, 3 e 4 automaticamente após carregar as imagens"));
    connect(autoBox_, &QCheckBox::toggled, this, &WorkflowPanel::autoWorkflowChanged);
    layout_->addWidget(autoBox_);

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

    sw.downloadLabel = new QLabel(QString(), row);
    sw.downloadLabel->setStyleSheet("font-size:9px; padding-left:22px;");
    sw.downloadLabel->hide();

    sw.btn = new QPushButton(tr("▶ Do it"), row);
    sw.btn->setObjectName(QString("stepBtn%1").arg(index));
    sw.btn->setFixedHeight(24);
    sw.btn->hide();
    connect(sw.btn, &QPushButton::clicked, this, [this, index] {
        emit stepActionRequested(static_cast<core::SessionStep>(index + 1));
    });

    rl->addWidget(top);
    rl->addWidget(sw.status);
    rl->addWidget(sw.downloadLabel);
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

bool WorkflowPanel::autoWorkflow() const
{
    return autoBox_ && autoBox_->isChecked();
}

void WorkflowPanel::setAutoWorkflow(bool on)
{
    if (autoBox_) autoBox_->setChecked(on);
}

void WorkflowPanel::setStepLabel(int index, const QString& name, const QString& desc)
{
    if (index < 0 || index >= kStepCount) return;
    steps_[index].name->setText(name);
    steps_[index].status->setText(desc);
}

void WorkflowPanel::setStepStatus(int index, Status status)
{
    if (index < 0 || index >= kStepCount) return;
    auto& sw = steps_[index];

    // ── Download status label ─────────────────────────────────────────────────
    switch (status) {
    case Status::Idle:
        sw.downloadLabel->hide();
        break;
    case Status::InProgress:
        sw.downloadLabel->setText(tr("⏳ baixando..."));
        sw.downloadLabel->setStyleSheet("color:#ffaa44; font-size:9px; padding-left:22px;");
        sw.downloadLabel->show();
        break;
    case Status::Ok:
        sw.downloadLabel->setText(tr("✓ concluído"));
        sw.downloadLabel->setStyleSheet("color:#44cc88; font-size:9px; padding-left:22px;");
        sw.downloadLabel->show();
        break;
    case Status::Error:
        sw.downloadLabel->setText(tr("✗ erro"));
        sw.downloadLabel->setStyleSheet("color:#ff5555; font-size:9px; padding-left:22px;");
        sw.downloadLabel->show();
        break;
    }

    // ── Action button state ───────────────────────────────────────────────────
    // Step 1 (Data Reduction, index 1): supports cancel → show "⏹ Parar" while busy.
    // Step 2 (Known Objects, index 2): no cancel → hide button while busy.
    if (index == 1) {
        if (status == Status::InProgress) {
            sw.btn->setText(tr("⏹ Parar"));
            sw.btn->setVisible(true);
        } else {
            sw.btn->setText(tr("▶ Do it"));
            // Visibility is restored by updateStepStates on next setStep().
            // Force show on Error so the user can retry without advancing step.
            if (status == Status::Error)
                sw.btn->setVisible(true);
        }
    } else if (index == 2) {
        if (status == Status::InProgress) {
            sw.btn->setVisible(false);
        } else if (status == Status::Error) {
            sw.btn->setVisible(true);
        }
    }
}

void WorkflowPanel::updateStepStates(core::SessionStep current)
{
    // cur is the number of steps COMPLETED.
    // The NEXT step to act on is cur+1.
    // Examples: Idle(0)→step 1 active; ImagesLoaded(1)→step 2 active; etc.
    const int cur = static_cast<int>(current);

    for (int i = 0; i < kStepCount; ++i) {
        auto& sw = steps_[i];
        const int stepVal = i + 1;

        if (cur >= stepVal) {
            // Completed
            sw.icon->setText("✓");
            sw.icon->setStyleSheet("color:#44cc88; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#44cc88;");
            sw.btn->hide();
        } else if (cur + 1 == stepVal) {
            // Next action — active
            sw.icon->setText("▶");
            sw.icon->setStyleSheet("color:#4488ff; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#4488ff;");
            sw.btn->show();
        } else {
            // Not yet reachable
            sw.icon->setText("○");
            sw.icon->setStyleSheet("color:#445566; font-size:12px;");
            sw.name->setStyleSheet("font-weight:bold; color:#445566;");
            sw.btn->hide();
        }
    }
}
