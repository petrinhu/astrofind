#include "ExportImage.h"

#include <QImage>
#include <QFileInfo>

namespace core {

QString exportImage(const FitsImage& img, const QString& filePath)
{
    if (!img.isValid())
        return QStringLiteral("Image has no data");

    // Build 8-bit grayscale QImage using the current display stretch
    QImage out(img.width, img.height, QImage::Format_Grayscale8);
    for (int y = 0; y < img.height; ++y) {
        uchar* row = out.scanLine(y);
        for (int x = 0; x < img.width; ++x)
            row[x] = stretchPixel(img.pixelAt(x, y), img.displayMin, img.displayMax);
    }

    const QString ext = QFileInfo(filePath).suffix().toLower();
    int quality = -1;   // default
    if (ext == QLatin1String("jpg") || ext == QLatin1String("jpeg"))
        quality = 92;

    if (!out.save(filePath, nullptr, quality))
        return QStringLiteral("Could not write file: %1").arg(filePath);

    return {};
}

} // namespace core
