#pragma once

#include "core/FitsImage.h"
#include <QDialog>

/// 2-D power spectrum dialog.
///
/// Computes log₁₀(1 + |FFT|²) of the image, shifts DC to centre,
/// and renders it as a grayscale map.  Useful for diagnosing periodic
/// noise (horizontal/vertical stripe artefacts, vibrations, guiding
/// problems) and for assessing focus quality via the spectral profile.
///
/// Computation uses a 2-D real-to-complex FFT (fftw3).  Images larger
/// than 1024 pixels per side are downsampled before the FFT to keep the
/// computation fast; no pixel data is stored after construction.
class PowerSpectrumDialog : public QDialog {
    Q_OBJECT
public:
    explicit PowerSpectrumDialog(const core::FitsImage& img, QWidget* parent = nullptr);
};
