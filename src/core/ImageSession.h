#pragma once

#include "FitsImage.h"
#include <QObject>
#include <QString>
#include <QVector>

namespace core {

/// Tracks the state of the current reduction pipeline for one session
enum class SessionStep {
    Idle            = 0,
    ImagesLoaded    = 1,
    DataReduced     = 2,
    OverlayApplied  = 3,
    Blinking        = 4,
    Measuring       = 5,
    ReportReady     = 6,
};

/// A session groups up to 4 FITS images from the same observation field
class ImageSession : public QObject {
    Q_OBJECT
public:
    explicit ImageSession(QObject* parent = nullptr);

    bool addImage(FitsImage img);
    void clear();

    int         imageCount()           const noexcept { return images_.size(); }
    bool        isEmpty()              const noexcept { return images_.isEmpty(); }
    const FitsImage& image(int i)      const          { return images_.at(i); }
    FitsImage&       image(int i)                     { return images_[i]; }

    SessionStep step()                 const noexcept { return step_; }
    void        setStep(SessionStep s);

    QString     sessionId()            const noexcept { return sessionId_; }
    void        setSessionId(const QString& id)       { sessionId_ = id; }

    /// True if all images are from the same field (RA/Dec within tolerance)
    bool        isCohesive()           const noexcept;

signals:
    void stepChanged(SessionStep step);
    void imagesChanged();

private:
    QVector<FitsImage> images_;
    SessionStep        step_      = SessionStep::Idle;
    QString            sessionId_;
};

} // namespace core
