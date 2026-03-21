#pragma once

#include <QDialog>
#include <QCheckBox>

/// Simple dialog to toggle which overlay types are visible on images.
struct MarkingsState {
    bool detectedStars = true;
    bool catalogStars  = true;
    bool kooObjects    = true;
    bool labels        = true;
};

class SelectMarkingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SelectMarkingsDialog(const MarkingsState& cur, QWidget* parent = nullptr);
    MarkingsState state() const;

private:
    QCheckBox* detectedChk_ = nullptr;
    QCheckBox* catalogChk_  = nullptr;
    QCheckBox* kooChk_      = nullptr;
    QCheckBox* labelsChk_   = nullptr;
};
