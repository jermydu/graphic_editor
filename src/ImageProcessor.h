#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QColor>

class ImageProcessor
{
public:
    static QImage adjustBrightness(const QImage &image, int value);
    static QImage adjustContrast(const QImage &image, int value);
    static QImage adjustSaturation(const QImage &image, int value);
    
    static QImage applyGrayscale(const QImage &image, int intensity = 100);
    static QImage applySepia(const QImage &image, int intensity = 100);
    static QImage applyBlur(const QImage &image, int radius);
    static QImage applySharpen(const QImage &image, int intensity);
    static QImage applyEmboss(const QImage &image, int intensity);
    static QImage applyInvert(const QImage &image);
    static QImage applyWarm(const QImage &image, int intensity);
    static QImage applyCool(const QImage &image, int intensity);
    static QImage applyVintage(const QImage &image, int intensity);
};

#endif // IMAGEPROCESSOR_H
