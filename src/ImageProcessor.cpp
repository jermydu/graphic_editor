#include "ImageProcessor.h"
#include <QtMath>

QImage ImageProcessor::adjustBrightness(const QImage &image, int value)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    int pixels = result.width() * result.height();
    int brightness = value - 100;
    
    for (int i = 0; i < pixels; ++i) {
        QRgb *pixel = reinterpret_cast<QRgb*>(result.bits()) + i;
        int r = qBound(0, qRed(*pixel) + brightness, 255);
        int g = qBound(0, qGreen(*pixel) + brightness, 255);
        int b = qBound(0, qBlue(*pixel) + brightness, 255);
        *pixel = qRgba(r, g, b, qAlpha(*pixel));
    }
    return result;
}

QImage ImageProcessor::adjustContrast(const QImage &image, int value)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = (value - 100.0) / 100.0;
    factor = (factor >= 0) ? (1 + factor) : (1.0 / (1 - factor));
    
    int pixels = result.width() * result.height();
    for (int i = 0; i < pixels; ++i) {
        QRgb *pixel = reinterpret_cast<QRgb*>(result.bits()) + i;
        int r = qBound(0, static_cast<int>((qRed(*pixel) - 128) * factor + 128), 255);
        int g = qBound(0, static_cast<int>((qGreen(*pixel) - 128) * factor + 128), 255);
        int b = qBound(0, static_cast<int>((qBlue(*pixel) - 128) * factor + 128), 255);
        *pixel = qRgba(r, g, b, qAlpha(*pixel));
    }
    return result;
}

QImage ImageProcessor::adjustSaturation(const QImage &image, int value)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = value / 100.0;
    
    int pixels = result.width() * result.height();
    for (int i = 0; i < pixels; ++i) {
        QRgb *pixel = reinterpret_cast<QRgb*>(result.bits()) + i;
        int r = qRed(*pixel);
        int g = qGreen(*pixel);
        int b = qBlue(*pixel);
        
        int gray = (r * 299 + g * 587 + b * 114) / 1000;
        r = qBound(0, static_cast<int>(gray + (r - gray) * factor), 255);
        g = qBound(0, static_cast<int>(gray + (g - gray) * factor), 255);
        b = qBound(0, static_cast<int>(gray + (b - gray) * factor), 255);
        *pixel = qRgba(r, g, b, qAlpha(*pixel));
    }
    return result;
}

QImage ImageProcessor::applyGrayscale(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double blend = intensity / 100.0;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int gray = (qRed(line[x]) * 299 + qGreen(line[x]) * 587 + qBlue(line[x]) * 114) / 1000;
            int r = qBound(0, static_cast<int>(qRed(line[x]) * (1 - blend) + gray * blend), 255);
            int g = qBound(0, static_cast<int>(qGreen(line[x]) * (1 - blend) + gray * blend), 255);
            int b = qBound(0, static_cast<int>(qBlue(line[x]) * (1 - blend) + gray * blend), 255);
            line[x] = qRgba(r, g, b, qAlpha(line[x]));
        }
    }
    return result;
}

QImage ImageProcessor::applySepia(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double blend = intensity / 100.0;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int r = qRed(line[x]);
            int g = qGreen(line[x]);
            int b = qBlue(line[x]);
            
            int tr = qBound(0, static_cast<int>(r * 0.393 + g * 0.769 + b * 0.189), 255);
            int tg = qBound(0, static_cast<int>(r * 0.349 + g * 0.686 + b * 0.168), 255);
            int tb = qBound(0, static_cast<int>(r * 0.272 + g * 0.534 + b * 0.131), 255);
            
            r = static_cast<int>(r * (1 - blend) + tr * blend);
            g = static_cast<int>(g * (1 - blend) + tg * blend);
            b = static_cast<int>(b * (1 - blend) + tb * blend);
            line[x] = qRgba(qBound(0, r, 255), qBound(0, g, 255), qBound(0, b, 255), qAlpha(line[x]));
        }
    }
    return result;
}

QImage ImageProcessor::applyBlur(const QImage &image, int radius)
{
    if (image.isNull() || radius <= 0) return image;
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    int size = radius * 2 + 1;
    int total = size * size;
    
    for (int y = radius; y < result.height() - radius; ++y) {
        for (int x = radius; x < result.width() - radius; ++x) {
            int sr = 0, sg = 0, sb = 0, sa = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    QRgb p = result.pixel(x + dx, y + dy);
                    sr += qRed(p);
                    sg += qGreen(p);
                    sb += qBlue(p);
                    sa += qAlpha(p);
                }
            }
            result.setPixel(x, y, qRgba(sr / total, sg / total, sb / total, sa / total));
        }
    }
    return result;
}

QImage ImageProcessor::applySharpen(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    static int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    double factor = intensity / 100.0;
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    QImage temp = result.copy();
    
    for (int y = 1; y < result.height() - 1; ++y) {
        for (int x = 1; x < result.width() - 1; ++x) {
            int r = 0, g = 0, b = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    QRgb p = temp.pixel(x + dx, y + dy);
                    int k = kernel[dy + 1][dx + 1];
                    r += qRed(p) * k;
                    g += qGreen(p) * k;
                    b += qBlue(p) * k;
                }
            }
            QRgb orig = temp.pixel(x, y);
            r = qBound(0, static_cast<int>(qRed(orig) + (r - qRed(orig)) * factor), 255);
            g = qBound(0, static_cast<int>(qGreen(orig) + (g - qGreen(orig)) * factor), 255);
            b = qBound(0, static_cast<int>(qBlue(orig) + (b - qBlue(orig)) * factor), 255);
            result.setPixel(x, y, qRgba(r, g, b, qAlpha(orig)));
        }
    }
    return result;
}

QImage ImageProcessor::applyEmboss(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    static int kernel[3][3] = {{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}};
    double factor = intensity / 100.0;
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    QImage temp = result.copy();
    
    for (int y = 1; y < result.height() - 1; ++y) {
        for (int x = 1; x < result.width() - 1; ++x) {
            int gray = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    QRgb p = temp.pixel(x + dx, y + dy);
                    int g = (qRed(p) + qGreen(p) + qBlue(p)) / 3;
                    gray += g * kernel[dy + 1][dx + 1];
                }
            }
            gray = 128 + static_cast<int>(gray * factor);
            gray = qBound(0, gray, 255);
            QRgb orig = temp.pixel(x, y);
            int r = qBound(0, static_cast<int>(qRed(orig) * (1 - factor) + gray * factor), 255);
            int g = qBound(0, static_cast<int>(qGreen(orig) * (1 - factor) + gray * factor), 255);
            int b = qBound(0, static_cast<int>(qBlue(orig) * (1 - factor) + gray * factor), 255);
            result.setPixel(x, y, qRgba(r, g, b, qAlpha(orig)));
        }
    }
    return result;
}

QImage ImageProcessor::applyInvert(const QImage &image)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    int pixels = result.width() * result.height();
    
    for (int i = 0; i < pixels; ++i) {
        QRgb *pixel = reinterpret_cast<QRgb*>(result.bits()) + i;
        int r = 255 - qRed(*pixel);
        int g = 255 - qGreen(*pixel);
        int b = 255 - qBlue(*pixel);
        *pixel = qRgba(r, g, b, qAlpha(*pixel));
    }
    return result;
}

QImage ImageProcessor::applyWarm(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = intensity / 100.0;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int r = qBound(0, static_cast<int>(qRed(line[x]) + 30 * factor), 255);
            int b = qBound(0, static_cast<int>(qBlue(line[x]) - 20 * factor), 255);
            line[x] = qRgba(r, qGreen(line[x]), b, qAlpha(line[x]));
        }
    }
    return result;
}

QImage ImageProcessor::applyCool(const QImage &image, int intensity)
{
    if (image.isNull()) return QImage();
    
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = intensity / 100.0;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int r = qBound(0, static_cast<int>(qRed(line[x]) - 20 * factor), 255);
            int b = qBound(0, static_cast<int>(qBlue(line[x]) + 30 * factor), 255);
            line[x] = qRgba(r, qGreen(line[x]), b, qAlpha(line[x]));
        }
    }
    return result;
}

QImage ImageProcessor::applyVintage(const QImage &image, int intensity)
{
    QImage result = applySepia(image, 70);
    result = adjustContrast(result, 90 + intensity / 5);
    result = applyWarm(result, intensity);
    return result;
}
