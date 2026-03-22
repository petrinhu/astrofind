#pragma once

#include "core/Spectrum1D.h"
#include <QDialog>

class SpectrumDialog : public QDialog {
    Q_OBJECT
public:
    explicit SpectrumDialog(const core::Spectrum1D& spec, QWidget* parent = nullptr);
};
