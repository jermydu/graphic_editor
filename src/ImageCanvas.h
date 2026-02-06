#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QPen>
#include <QStack>
#include <QString>

enum class ToolType {
    Select,
    Crop,
    Brush,
    Eraser,
    Text,
    Pipette
};

struct TextItem {
    QString text;
    QPoint pos;
    QFont font;
    QColor color;
    QRect boundingRect;
};

class ImageCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCanvas(QWidget *parent = nullptr);
    
    bool loadImage(const QString &fileName);
    bool loadImage(const QImage &image);
    const QImage& image() const { return m_image; }
    QImage imageCopy() const { return m_image; }
    QImage imageForExport() const;
    
    void setTool(ToolType tool);
    ToolType tool() const { return m_tool; }
    
    void setBrushSize(int size);
    void setBrushColor(const QColor &color);
    void setEraserSize(int size);
    
    void setBrightness(int value);
    void setContrast(int value);
    void setSaturation(int value);
    void resetAdjustments();
    
    void applyFilter(const QString &filterName);
    void setFilterIntensity(int value);
    
    void crop(const QRect &rect);
    void applyCropToCurrentRect();
    void rotate(int angle);
    void flipHorizontal();
    void flipVertical();
    void resize(int width, int height);
    
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    
    void copyToClipboard();
    void pasteFromClipboard();
    bool canPaste() const;
    
    void addText(const QString &text, const QPoint &pos);
    void clearTextItems();
    
    double zoomFactor() const { return m_zoomFactor; }
    void setZoomFactor(double factor);
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomOriginal();
    
    bool isModified() const { return m_modified; }
    void setModified(bool modified) { m_modified = modified; }
    
    bool hasImage() const { return !m_image.isNull(); }
    QSize imageSize() const { return m_image.size(); }

signals:
    void imageModified(const QImage &image);
    void pixelColorPicked(const QColor &color);
    void statusMessage(const QString &message);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPoint mapToImage(const QPoint &pos) const;
    QRect mapFromImage(const QRect &rect) const;
    QRect mapToImage(const QRect &rect) const;
    void saveState();
    void pushState(const QImage &img);
    QImage applyCurrentAdjustments(const QImage &source) const;
    
    QImage m_image;
    QImage m_displayImage;
    QImage m_baseImage;
    QImage m_adjustedImage;
    ToolType m_tool;
    
    QPoint m_lastPoint;
    bool m_drawing;
    QRect m_cropRect;
    bool m_cropMode;
    
    int m_brushSize;
    QColor m_brushColor;
    int m_eraserSize;
    
    int m_brightness;
    int m_contrast;
    int m_saturation;
    
    QString m_currentFilter;
    int m_filterIntensity;
    
    QList<TextItem> m_textItems;
    int m_selectedTextIndex;
    bool m_textInputMode;
    
    QStack<QImage> m_undoStack;
    QStack<QImage> m_redoStack;
    static const int MAX_UNDO_STEPS = 50;
    
    double m_zoomFactor;
    bool m_modified;
};

#endif // IMAGECANVAS_H
