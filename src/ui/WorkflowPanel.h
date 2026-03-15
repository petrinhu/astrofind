#pragma once

#include "core/ImageSession.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <array>

/// Left panel showing the 6-step workflow with status indicators.
class WorkflowPanel : public QWidget {
    Q_OBJECT
public:
    explicit WorkflowPanel(QWidget* parent = nullptr);

public slots:
    void setStep(core::SessionStep step);

signals:
    void stepActionRequested(core::SessionStep step);

private:
    struct StepWidget {
        QLabel*      icon   = nullptr;
        QLabel*      name   = nullptr;
        QLabel*      status = nullptr;
        QPushButton* btn    = nullptr;
    };

    void buildStepRow(int index, const QString& number, const QString& name, const QString& desc);
    void updateStepStates(core::SessionStep current);

    static constexpr int kStepCount = 6;
    std::array<StepWidget, kStepCount> steps_;
    QVBoxLayout* layout_ = nullptr;
    core::SessionStep currentStep_ = core::SessionStep::Idle;
};
