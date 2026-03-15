#include "ImageSession.h"
#include <spdlog/spdlog.h>
#include <cmath>

namespace core {

ImageSession::ImageSession(QObject* parent)
    : QObject(parent)
{}

bool ImageSession::addImage(FitsImage img)
{
    if (images_.size() >= 4) {
        spdlog::warn("Session already has 4 images");
        return false;
    }
    images_.append(std::move(img));
    emit imagesChanged();
    return true;
}

void ImageSession::clear()
{
    images_.clear();
    step_ = SessionStep::Idle;
    sessionId_.clear();
    emit imagesChanged();
    emit stepChanged(step_);
}

void ImageSession::setStep(SessionStep s)
{
    if (step_ != s) {
        step_ = s;
        emit stepChanged(step_);
    }
}

bool ImageSession::isCohesive() const noexcept
{
    if (images_.size() < 2) return true;
    constexpr double kTol = 1.0; // degrees
    const double ra0  = images_[0].ra;
    const double dec0 = images_[0].dec;
    for (const auto& img : images_) {
        if (std::abs(img.ra  - ra0)  > kTol) return false;
        if (std::abs(img.dec - dec0) > kTol) return false;
    }
    return true;
}

} // namespace core
