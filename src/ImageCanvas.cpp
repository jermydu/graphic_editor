#include "ImageCanvas.h"
#include "ImageProcessor.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QtMath>

ImageCanvas::ImageCanvas(QWidget *parent)
    : QWidget(parent)
    , m_tool(ToolType::Select)
    , m_drawing(false)
    , m_cropMode(false)
    , m_brushSize(5)
    , m_brushColor(Qt::black)
    , m_eraserSize(20)
    , m_brightness(100)
    , m_contrast(100)
    , m_saturation(100)
    , m_filterIntensity(100)
    , m_selectedTextIndex(-1)
    , m_textInputMode(false)
    , m_zoomFactor(1.0)
    , m_modified(false)
{
    setMinimumSize(200, 200);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

bool ImageCanvas::loadImage(const QString &fileName)
{
    QImage loaded;
    if (!loaded.load(fileName)) return false;
    
    m_image = loaded.convertToFormat(QImage::Format_ARGB32);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_image.copy();
    m_undoStack.clear();
    m_redoStack.clear();
    m_textItems.clear();
    m_modified = false;
    
    zoomFit();
    update();
    return true;
}

bool ImageCanvas::loadImage(const QImage &image)
{
    if (image.isNull()) return false;
    
    m_image = image.convertToFormat(QImage::Format_ARGB32);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_image.copy();
    m_undoStack.clear();
    m_redoStack.clear();
    m_textItems.clear();
    m_modified = false;
    
    update();
    return true;
}

void ImageCanvas::setTool(ToolType tool)
{
    m_tool = tool;
    m_cropMode = (tool == ToolType::Crop);
    if (m_cropMode) m_cropRect = QRect();
    update();
}

void ImageCanvas::setBrushSize(int size) { m_brushSize = qBound(1, size, 100); }
void ImageCanvas::setBrushColor(const QColor &color) { m_brushColor = color; }
void ImageCanvas::setEraserSize(int size) { m_eraserSize = qBound(5, size, 200); }

void ImageCanvas::setBrightness(int value)
{
    m_brightness = value;
    m_adjustedImage = applyCurrentAdjustments(m_baseImage);
    m_displayImage = m_adjustedImage.copy();
    update();
}

void ImageCanvas::setContrast(int value)
{
    m_contrast = value;
    m_adjustedImage = applyCurrentAdjustments(m_baseImage);
    m_displayImage = m_adjustedImage.copy();
    update();
}

void ImageCanvas::setSaturation(int value)
{
    m_saturation = value;
    m_adjustedImage = applyCurrentAdjustments(m_baseImage);
    m_displayImage = m_adjustedImage.copy();
    update();
}

void ImageCanvas::resetAdjustments()
{
    m_brightness = 100;
    m_contrast = 100;
    m_saturation = 100;
    m_adjustedImage = m_baseImage.copy();
    m_displayImage = m_adjustedImage.copy();
    update();
}

void ImageCanvas::applyFilter(const QString &filterName)
{
    m_currentFilter = filterName;
    if (filterName.isEmpty()) {
        m_displayImage = m_adjustedImage.copy();
    } else {
        QImage base = m_adjustedImage;
        if (filterName == "grayscale") m_displayImage = ImageProcessor::applyGrayscale(base, m_filterIntensity);
        else if (filterName == "sepia") m_displayImage = ImageProcessor::applySepia(base, m_filterIntensity);
        else if (filterName == "blur") m_displayImage = ImageProcessor::applyBlur(base, m_filterIntensity / 20);
        else if (filterName == "sharpen") m_displayImage = ImageProcessor::applySharpen(base, m_filterIntensity);
        else if (filterName == "emboss") m_displayImage = ImageProcessor::applyEmboss(base, m_filterIntensity);
        else if (filterName == "invert") m_displayImage = ImageProcessor::applyInvert(base);
        else if (filterName == "warm") m_displayImage = ImageProcessor::applyWarm(base, m_filterIntensity);
        else if (filterName == "cool") m_displayImage = ImageProcessor::applyCool(base, m_filterIntensity);
        else if (filterName == "vintage") m_displayImage = ImageProcessor::applyVintage(base, m_filterIntensity);
        else m_displayImage = base;
    }
    update();
}

void ImageCanvas::setFilterIntensity(int value)
{
    m_filterIntensity = value;
    applyFilter(m_currentFilter);
}

void ImageCanvas::applyCropToCurrentRect()
{
    if (m_cropRect.isEmpty()) {
        emit statusMessage("请先用裁剪工具拖拽选择区域");
        return;
    }
    crop(m_cropRect);
}

void ImageCanvas::crop(const QRect &rect)
{
    if (rect.isEmpty() || !m_image.rect().contains(rect)) return;
    
    saveState();
    m_image = m_image.copy(rect);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_image.copy();
    m_cropRect = QRect();
    m_cropMode = false;
    m_modified = true;
    
    QList<TextItem> kept;
    for (const TextItem &t : m_textItems) {
        if (rect.contains(t.pos)) {
            TextItem nt = t;
            nt.pos -= rect.topLeft();
            kept.append(nt);
        }
    }
    m_textItems = kept;
    
    emit imageModified(m_image);
    update();
}

void ImageCanvas::rotate(int angle)
{
    if (m_image.isNull()) return;
    
    saveState();
    QTransform transform;
    transform.rotate(angle);
    m_image = m_image.transformed(transform, Qt::SmoothTransformation);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    
    QPoint center = m_image.rect().center();
    for (TextItem &t : m_textItems) {
        QPoint rel = t.pos - center;
        double rad = qDegreesToRadians(static_cast<double>(angle));
        int newX = center.x() + static_cast<int>(rel.x() * qCos(rad) - rel.y() * qSin(rad));
        int newY = center.y() + static_cast<int>(rel.x() * qSin(rad) + rel.y() * qCos(rad));
        t.pos = QPoint(newX, newY);
    }
    
    emit imageModified(m_image);
    update();
}

void ImageCanvas::flipHorizontal()
{
    if (m_image.isNull()) return;
    
    saveState();
    m_image = m_image.mirrored(true, false);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    
    for (TextItem &t : m_textItems) {
        t.pos.setX(m_image.width() - t.pos.x());
    }
    
    emit imageModified(m_image);
    update();
}

void ImageCanvas::flipVertical()
{
    if (m_image.isNull()) return;
    
    saveState();
    m_image = m_image.mirrored(false, true);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    
    for (TextItem &t : m_textItems) {
        t.pos.setY(m_image.height() - t.pos.y());
    }
    
    emit imageModified(m_image);
    update();
}

void ImageCanvas::resize(int width, int height)
{
    if (m_image.isNull() || width <= 0 || height <= 0) return;
    
    QSize oldSize = m_image.size();
    saveState();
    m_image = m_image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_baseImage = m_image.copy();
    m_adjustedImage = m_image.copy();
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    
    double sx = static_cast<double>(width) / oldSize.width();
    double sy = static_cast<double>(height) / oldSize.height();
    for (TextItem &t : m_textItems) {
        t.pos.setX(static_cast<int>(t.pos.x() * sx));
        t.pos.setY(static_cast<int>(t.pos.y() * sy));
    }
    
    emit imageModified(m_image);
    update();
}

void ImageCanvas::undo()
{
    if (!canUndo()) return;
    m_redoStack.push(m_image);
    m_image = m_undoStack.pop();
    m_baseImage = m_image.copy();
    m_adjustedImage = applyCurrentAdjustments(m_baseImage);
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    emit imageModified(m_image);
    update();
}

void ImageCanvas::redo()
{
    if (!canRedo()) return;
    m_undoStack.push(m_image);
    m_image = m_redoStack.pop();
    m_baseImage = m_image.copy();
    m_adjustedImage = applyCurrentAdjustments(m_baseImage);
    m_displayImage = m_adjustedImage.copy();
    m_modified = true;
    emit imageModified(m_image);
    update();
}

bool ImageCanvas::canUndo() const { return !m_undoStack.isEmpty(); }
bool ImageCanvas::canRedo() const { return !m_redoStack.isEmpty(); }

void ImageCanvas::copyToClipboard()
{
    if (!m_image.isNull()) {
        QApplication::clipboard()->setImage(m_image);
    }
}

void ImageCanvas::pasteFromClipboard()
{
    QImage img = QApplication::clipboard()->image();
    if (!img.isNull()) {
        loadImage(img);
        m_modified = true;
        emit imageModified(m_image);
    }
}

bool ImageCanvas::canPaste() const
{
    return !QApplication::clipboard()->image().isNull();
}

void ImageCanvas::addText(const QString &text, const QPoint &pos)
{
    if (text.isEmpty()) return;
    
    TextItem item;
    item.text = text;
    item.pos = pos;
    item.font = QFont("Arial", 24);
    item.color = Qt::black;
    QFontMetrics fm(item.font);
    item.boundingRect = fm.boundingRect(text);
    item.boundingRect.moveTopLeft(pos);
    m_textItems.append(item);
    m_modified = true;
    update();
}

void ImageCanvas::clearTextItems()
{
    m_textItems.clear();
    m_selectedTextIndex = -1;
    update();
}

void ImageCanvas::setZoomFactor(double factor)
{
    m_zoomFactor = qBound(0.1, factor, 10.0);
    if (!m_image.isNull()) {
        setFixedSize(QSize(
            qMax(width(), static_cast<int>(m_image.width() * m_zoomFactor)),
            qMax(height(), static_cast<int>(m_image.height() * m_zoomFactor))
        ));
    }
    update();
    emit statusMessage(QString("缩放: %1%").arg(static_cast<int>(m_zoomFactor * 100)));
}

void ImageCanvas::zoomIn() { setZoomFactor(m_zoomFactor * 1.25); }
void ImageCanvas::zoomOut() { setZoomFactor(m_zoomFactor / 1.25); }

void ImageCanvas::zoomFit()
{
    if (m_image.isNull()) return;
    QWidget *p = parentWidget();
    if (!p) return;
    QSize avail = p->size();
    double sx = static_cast<double>(avail.width()) / m_image.width();
    double sy = static_cast<double>(avail.height()) / m_image.height();
    m_zoomFactor = qBound(0.1, qMin(sx, sy), 10.0);
    setFixedSize(QSize(
        static_cast<int>(m_image.width() * m_zoomFactor),
        static_cast<int>(m_image.height() * m_zoomFactor)
    ));
    update();
}

QImage ImageCanvas::imageForExport() const
{
    if (m_image.isNull()) return QImage();
    QImage result = m_displayImage.copy();
    QPainter p(&result);
    for (const TextItem &t : m_textItems) {
        p.setFont(t.font);
        p.setPen(t.color);
        p.drawText(t.boundingRect, Qt::AlignLeft | Qt::AlignTop, t.text);
    }
    p.end();
    return result;
}

void ImageCanvas::zoomOriginal()
{
    m_zoomFactor = 1.0;
    if (!m_image.isNull()) {
        setFixedSize(m_image.size());
    }
    update();
}

QPoint ImageCanvas::mapToImage(const QPoint &pos) const
{
    if (m_image.isNull()) return QPoint();
    int x = static_cast<int>(pos.x() / m_zoomFactor);
    int y = static_cast<int>(pos.y() / m_zoomFactor);
    return QPoint(x, y);
}

QRect ImageCanvas::mapFromImage(const QRect &rect) const
{
    return QRect(
        static_cast<int>(rect.x() * m_zoomFactor),
        static_cast<int>(rect.y() * m_zoomFactor),
        static_cast<int>(rect.width() * m_zoomFactor),
        static_cast<int>(rect.height() * m_zoomFactor)
    );
}

QRect ImageCanvas::mapToImage(const QRect &rect) const
{
    return QRect(
        static_cast<int>(rect.x() / m_zoomFactor),
        static_cast<int>(rect.y() / m_zoomFactor),
        static_cast<int>(rect.width() / m_zoomFactor),
        static_cast<int>(rect.height() / m_zoomFactor)
    );
}

void ImageCanvas::saveState()
{
    pushState(m_image);
}

void ImageCanvas::pushState(const QImage &img)
{
    m_redoStack.clear();
    m_undoStack.push(img.copy());
    while (m_undoStack.size() > MAX_UNDO_STEPS) m_undoStack.removeFirst();
}

QImage ImageCanvas::applyCurrentAdjustments(const QImage &source) const
{
    QImage result = source;
    result = ImageProcessor::adjustBrightness(result, m_brightness);
    result = ImageProcessor::adjustContrast(result, m_contrast);
    result = ImageProcessor::adjustSaturation(result, m_saturation);
    return result;
}

void ImageCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), QColor(60, 60, 60));
    
    if (m_image.isNull()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, "打开或新建图像以开始编辑");
        return;
    }
    
    QRect imgRect(0, 0, static_cast<int>(m_image.width() * m_zoomFactor),
                  static_cast<int>(m_image.height() * m_zoomFactor));
    
    p.drawImage(imgRect, m_displayImage);
    
    if (m_cropMode && !m_cropRect.isEmpty()) {
        p.setPen(QPen(Qt::white, 2));
        p.setBrush(QColor(0, 0, 0, 80));
        p.drawRect(mapFromImage(m_cropRect));
    }
    
    for (int i = 0; i < m_textItems.size(); ++i) {
        const TextItem &t = m_textItems[i];
        p.setFont(t.font);
        p.setPen(t.color);
        QRect dr = mapFromImage(t.boundingRect);
        p.drawText(dr, Qt::AlignLeft | Qt::AlignTop, t.text);
        if (i == m_selectedTextIndex) {
            p.setPen(QPen(Qt::blue, 2));
            p.setBrush(Qt::NoBrush);
            p.drawRect(dr.adjusted(-2, -2, 2, 2));
        }
    }
}

void ImageCanvas::mousePressEvent(QMouseEvent *event)
{
    QPoint ip = mapToImage(event->pos());
    if (!m_image.rect().contains(ip)) return;
    
    if (m_tool == ToolType::Crop) {
        m_cropRect.setTopLeft(ip);
        m_cropRect.setSize(QSize(0, 0));
        m_drawing = true;
    } else if (m_tool == ToolType::Brush) {
        saveState();
        m_lastPoint = ip;
        m_drawing = true;
    } else if (m_tool == ToolType::Eraser) {
        saveState();
        m_lastPoint = ip;
        m_drawing = true;
    } else if (m_tool == ToolType::Text) {
        bool ok;
        QString text = QInputDialog::getText(this, "添加文字", "请输入文字:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            QFont font = QFontDialog::getFont(&ok, QFont("Arial", 24), this);
            QColor color = QColorDialog::getColor(Qt::black, this, "选择文字颜色");
            if (!color.isValid()) color = Qt::black;
            
            TextItem item;
            item.text = text;
            item.pos = ip;
            item.font = ok ? font : QFont("Arial", 24);
            item.color = color;
            QFontMetrics fm(item.font);
            item.boundingRect = fm.boundingRect(text);
            item.boundingRect.moveTopLeft(ip);
            m_textItems.append(item);
            m_modified = true;
            update();
        }
    } else if (m_tool == ToolType::Pipette) {
        QColor c = m_image.pixelColor(ip);
        emit pixelColorPicked(c);
    }
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QPoint ip = mapToImage(event->pos());
    
    if (m_drawing && m_tool == ToolType::Crop) {
        m_cropRect.setBottomRight(ip);
        m_cropRect = m_cropRect.normalized();
        update();
    } else if (m_drawing && m_tool == ToolType::Brush) {
        QPainter painter(&m_image);
        painter.setPen(QPen(m_brushColor, m_brushSize, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(m_lastPoint, ip);
        painter.end();
        m_baseImage = m_image.copy();
        m_adjustedImage = applyCurrentAdjustments(m_baseImage);
        applyFilter(m_currentFilter);
        m_lastPoint = ip;
        m_modified = true;
        update();
    } else if (m_drawing && m_tool == ToolType::Eraser) {
        QPainter painter(&m_image);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(QPen(Qt::transparent, m_eraserSize, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(m_lastPoint, ip);
        painter.end();
        m_baseImage = m_image.copy();
        m_adjustedImage = applyCurrentAdjustments(m_baseImage);
        applyFilter(m_currentFilter);
        m_lastPoint = ip;
        m_modified = true;
        update();
    }
    
    if (m_image.rect().contains(ip)) {
        emit statusMessage(QString("位置: %1, %2").arg(ip.x()).arg(ip.y()));
    }
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_drawing && m_tool == ToolType::Crop) {
        m_drawing = false;
        m_cropRect = m_cropRect.intersected(m_image.rect());
        update();
    } else {
        m_drawing = false;
    }
}

void ImageCanvas::wheelEvent(QWheelEvent *event)
{
    if (m_image.isNull()) return;
    
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) zoomIn();
        else zoomOut();
        event->accept();
    }
}

void ImageCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && m_selectedTextIndex >= 0 && m_selectedTextIndex < m_textItems.size()) {
        m_textItems.removeAt(m_selectedTextIndex);
        m_selectedTextIndex = -1;
        m_modified = true;
        update();
    }
}
