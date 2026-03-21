#pragma once

#include "core/ImageSession.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <array>

/// Left panel showing the 6-step workflow with status indicators.
class WorkflowPanel : public QWidget {
    Q_OBJECT
public:
    enum class Status { Idle, InProgress, Ok, Error };

    explicit WorkflowPanel(QWidget* parent = nullptr);

    void setStepStatus(int index, Status status);
    void setStepLabel(int index, const QString& name, const QString& desc);

    bool autoWorkflow() const;
    void setAutoWorkflow(bool on);

public slots:
    void setStep(core::SessionStep step);

signals:
    void stepActionRequested(core::SessionStep step);
    void autoWorkflowChanged(bool on);

private:
    struct StepWidget {
        QLabel*      icon          = nullptr;
        QLabel*      name          = nullptr;
        QLabel*      status        = nullptr;
        QLabel*      downloadLabel = nullptr;
        QPushButton* btn           = nullptr;
    };

    void buildStepRow(int index, const QString& number, const QString& name, const QString& desc);
    void updateStepStates(core::SessionStep current);

    static constexpr int kStepCount = 6;
    std::array<StepWidget, kStepCount> steps_;
    QVBoxLayout* layout_  = nullptr;
    QCheckBox*   autoBox_ = nullptr;
    core::SessionStep currentStep_ = core::SessionStep::Idle;
};
