#include "MainWindow.h"
#include <QMenuBar>
#include <QFileInfo>
#include <QDir>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <QApplication>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_canvas(nullptr)
    , m_scrollArea(nullptr)
    , m_adjustmentPanel(nullptr)
    , m_filterPanel(nullptr)
    , m_toolOptionsPanel(nullptr)
    , m_lastDirectory(QDir::homePath())
{
    setupUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    setWindowTitle("PhotoEditor - 图像编辑器");
    resize(1200, 800);
    
    m_canvas = new ImageCanvas(this);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_canvas);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setStyleSheet("QScrollArea { background: #2b2b2b; }");
    setCentralWidget(m_scrollArea);
    
    setupMenuBar();
    setupToolBar();
    setupDockWidgets();
    setupConnections();
    
    m_statusLabel = new QLabel("就绪");
    m_zoomLabel = new QLabel("100%");
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_zoomLabel);
    
    updateActionsState();
}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    QAction *newAction = fileMenu->addAction("新建(&N)");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newImage);
    
    QAction *openAction = fileMenu->addAction("打开(&O)");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);
    
    fileMenu->addSeparator();
    
    m_saveAction = fileMenu->addAction("保存(&S)");
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveImage);
    
    m_saveAsAction = fileMenu->addAction("另存为(&A)");
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveImageAs);
    
    fileMenu->addSeparator();
    
    QAction *printAction = fileMenu->addAction("打印(&P)");
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &MainWindow::printImage);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    QMenu *editMenu = menuBar()->addMenu("编辑(&E)");
    
    m_undoAction = editMenu->addAction("撤销(&U)");
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);
    
    m_redoAction = editMenu->addAction("重做(&R)");
    m_redoAction->setShortcut(QKeySequence::Redo);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);
    
    editMenu->addSeparator();
    
    m_copyAction = editMenu->addAction("复制(&C)");
    m_copyAction->setShortcut(QKeySequence::Copy);
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::copy);
    
    m_pasteAction = editMenu->addAction("粘贴(&V)");
    m_pasteAction->setShortcut(QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::paste);
    
    QMenu *imageMenu = menuBar()->addMenu("图像(&I)");
    
    m_cropAction = imageMenu->addAction("裁剪(&C)");
    m_cropAction->setShortcut(tr("Ctrl+Shift+C"));
    connect(m_cropAction, &QAction::triggered, this, &MainWindow::cropImage);
    
    imageMenu->addSeparator();
    
    QAction *rotateLeftAction = imageMenu->addAction("向左旋转90°(&L)");
    connect(rotateLeftAction, &QAction::triggered, this, &MainWindow::rotateLeft);
    
    QAction *rotateRightAction = imageMenu->addAction("向右旋转90°(&R)");
    connect(rotateRightAction, &QAction::triggered, this, &MainWindow::rotateRight);
    
    imageMenu->addSeparator();
    
    QAction *flipHAction = imageMenu->addAction("水平翻转(&H)");
    connect(flipHAction, &QAction::triggered, this, &MainWindow::flipHorizontal);
    
    QAction *flipVAction = imageMenu->addAction("垂直翻转(&V)");
    connect(flipVAction, &QAction::triggered, this, &MainWindow::flipVertical);
    
    imageMenu->addSeparator();
    
    QAction *resizeAction = imageMenu->addAction("调整大小(&Z)");
    connect(resizeAction, &QAction::triggered, this, &MainWindow::resizeImage);
    
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    QAction *aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    QAction *aboutQtAction = helpMenu->addAction("关于 Qt");
    connect(aboutQtAction, &QAction::triggered, this, &MainWindow::showAboutQt);
}

void MainWindow::setupToolBar()
{
    QToolBar *mainToolBar = addToolBar("主工具栏");
    mainToolBar->setMovable(false);
    
    mainToolBar->addAction(QIcon(), "打开", this, &MainWindow::openImage);
    mainToolBar->addAction(QIcon(), "保存", this, &MainWindow::saveImage);
    mainToolBar->addSeparator();
    
    mainToolBar->addAction(QIcon(), "撤销", this, &MainWindow::undo);
    mainToolBar->addAction(QIcon(), "重做", this, &MainWindow::redo);
    mainToolBar->addSeparator();
    
    m_toolGroup = new QActionGroup(this);
    
    QAction *selectAction = mainToolBar->addAction("选择");
    selectAction->setCheckable(true);
    selectAction->setChecked(true);
    selectAction->setData(static_cast<int>(ToolType::Select));
    m_toolGroup->addAction(selectAction);
    
    QAction *cropAction = mainToolBar->addAction("裁剪");
    cropAction->setCheckable(true);
    cropAction->setData(static_cast<int>(ToolType::Crop));
    m_toolGroup->addAction(cropAction);
    
    QAction *brushAction = mainToolBar->addAction("画笔");
    brushAction->setCheckable(true);
    brushAction->setData(static_cast<int>(ToolType::Brush));
    m_toolGroup->addAction(brushAction);
    
    QAction *eraserAction = mainToolBar->addAction("橡皮");
    eraserAction->setCheckable(true);
    eraserAction->setData(static_cast<int>(ToolType::Eraser));
    m_toolGroup->addAction(eraserAction);
    
    QAction *textAction = mainToolBar->addAction("文字");
    textAction->setCheckable(true);
    textAction->setData(static_cast<int>(ToolType::Text));
    m_toolGroup->addAction(textAction);
    
    QAction *pipetteAction = mainToolBar->addAction("取色");
    pipetteAction->setCheckable(true);
    pipetteAction->setData(static_cast<int>(ToolType::Pipette));
    m_toolGroup->addAction(pipetteAction);
    
    connect(m_toolGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        m_canvas->setTool(static_cast<ToolType>(action->data().toInt()));
        m_toolOptionsPanel->setCurrentTool(action->data().toInt());
        updateActionsState();
    });
    
    mainToolBar->addSeparator();
    mainToolBar->addAction(QIcon(), "放大", m_canvas, [this]() { m_canvas->zoomIn(); updateZoomLabel(); });
    mainToolBar->addAction(QIcon(), "缩小", m_canvas, [this]() { m_canvas->zoomOut(); updateZoomLabel(); });
}

void MainWindow::updateZoomLabel()
{
    if (m_canvas && m_canvas->hasImage()) {
        m_zoomLabel->setText(QString("%1%").arg(static_cast<int>(m_canvas->zoomFactor() * 100)));
    } else {
        m_zoomLabel->setText("100%");
    }
}

void MainWindow::setupDockWidgets()
{
    m_adjustmentPanel = new AdjustmentPanel(this);
    QDockWidget *adjustDock = new QDockWidget("亮度/对比度/饱和度", this);
    adjustDock->setWidget(m_adjustmentPanel);
    addDockWidget(Qt::RightDockWidgetArea, adjustDock);
    
    m_filterPanel = new FilterPanel(this);
    QDockWidget *filterDock = new QDockWidget("滤镜", this);
    filterDock->setWidget(m_filterPanel);
    addDockWidget(Qt::RightDockWidgetArea, filterDock);
    
    m_toolOptionsPanel = new ToolOptionsPanel(this);
    QDockWidget *toolDock = new QDockWidget("工具选项", this);
    toolDock->setWidget(m_toolOptionsPanel);
    addDockWidget(Qt::RightDockWidgetArea, toolDock);
}

void MainWindow::setupConnections()
{
    connect(m_canvas, &ImageCanvas::imageModified, this, &MainWindow::updateImageFromCanvas);
    connect(m_canvas, &ImageCanvas::statusMessage, this, &MainWindow::updateStatusBar);
    connect(m_canvas, &ImageCanvas::pixelColorPicked, this, [this](const QColor &c) {
        m_canvas->setBrushColor(c);
        updateStatusBar(QString("已取色: RGB(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue()));
    });
    
    connect(m_adjustmentPanel, &AdjustmentPanel::brightnessChanged, m_canvas, &ImageCanvas::setBrightness);
    connect(m_adjustmentPanel, &AdjustmentPanel::contrastChanged, m_canvas, &ImageCanvas::setContrast);
    connect(m_adjustmentPanel, &AdjustmentPanel::saturationChanged, m_canvas, &ImageCanvas::setSaturation);
    connect(m_adjustmentPanel, &AdjustmentPanel::resetRequested, m_canvas, &ImageCanvas::resetAdjustments);
    
    connect(m_filterPanel, &FilterPanel::filterSelected, m_canvas, &ImageCanvas::applyFilter);
    connect(m_filterPanel, &FilterPanel::intensityChanged, m_canvas, &ImageCanvas::setFilterIntensity);
    
    connect(m_toolOptionsPanel, &ToolOptionsPanel::brushSizeChanged, m_canvas, &ImageCanvas::setBrushSize);
    connect(m_toolOptionsPanel, &ToolOptionsPanel::brushColorChanged, m_canvas, &ImageCanvas::setBrushColor);
    connect(m_toolOptionsPanel, &ToolOptionsPanel::eraserSizeChanged, m_canvas, &ImageCanvas::setEraserSize);
    
    connect(m_toolGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        m_canvas->setTool(static_cast<ToolType>(action->data().toInt()));
        m_toolOptionsPanel->setCurrentTool(action->data().toInt());
        updateActionsState();
    });
    
    connect(m_canvas, &ImageCanvas::imageModified, this, [this]() { updateZoomLabel(); });
}

void MainWindow::updateActionsState()
{
    bool hasImage = m_canvas->hasImage();
    m_saveAction->setEnabled(hasImage && m_canvas->isModified());
    m_saveAsAction->setEnabled(hasImage);
    m_undoAction->setEnabled(m_canvas->canUndo());
    m_redoAction->setEnabled(m_canvas->canRedo());
    m_copyAction->setEnabled(hasImage);
    m_pasteAction->setEnabled(m_canvas->canPaste());
    m_cropAction->setEnabled(hasImage);
    
    ToolType t = m_canvas->tool();
    bool needBrush = (t == ToolType::Brush);
    bool needEraser = (t == ToolType::Eraser);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::openImage()
{
    if (!maybeSave()) return;
    
    QString fileName = QFileDialog::getOpenFileName(this, "打开图像", m_lastDirectory,
        "图像文件 (*.png *.jpg *.jpeg *.bmp *.gif *.webp);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        loadFile(fileName);
        m_lastDirectory = QFileInfo(fileName).absolutePath();
    }
}

void MainWindow::saveImage()
{
    if (m_currentFile.isEmpty()) {
        saveImageAs();
        return;
    }
    saveFile(m_currentFile);
}

void MainWindow::saveImageAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "另存为", m_currentFile.isEmpty() ? m_lastDirectory : m_currentFile,
        "PNG 图像 (*.png);;JPEG 图像 (*.jpg *.jpeg);;BMP 图像 (*.bmp);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        saveFile(fileName);
        m_lastDirectory = QFileInfo(fileName).absolutePath();
    }
}

void MainWindow::newImage()
{
    if (!maybeSave()) return;
    
    bool ok;
    int w = QInputDialog::getInt(this, "新建图像", "宽度:", 800, 1, 10000, 1, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "新建图像", "高度:", 600, 1, 10000, 1, &ok);
    if (!ok) return;
    
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::white);
    m_canvas->loadImage(img);
    setCurrentFile(QString());
    updateActionsState();
}

void MainWindow::printImage()
{
    if (!m_canvas->hasImage()) return;
    
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = m_canvas->imageForExport().size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(m_canvas->imageForExport().rect());
        painter.drawImage(0, 0, m_canvas->imageForExport());
    }
}

void MainWindow::undo() { m_canvas->undo(); updateActionsState(); }
void MainWindow::redo() { m_canvas->redo(); updateActionsState(); }
void MainWindow::copy() { m_canvas->copyToClipboard(); }
void MainWindow::paste() { m_canvas->pasteFromClipboard(); updateActionsState(); }

void MainWindow::cropImage()
{
    m_canvas->applyCropToCurrentRect();
    updateActionsState();
}

void MainWindow::rotateLeft() { m_canvas->rotate(-90); updateActionsState(); }
void MainWindow::rotateRight() { m_canvas->rotate(90); updateActionsState(); }
void MainWindow::flipHorizontal() { m_canvas->flipHorizontal(); updateActionsState(); }
void MainWindow::flipVertical() { m_canvas->flipVertical(); updateActionsState(); }

void MainWindow::resizeImage()
{
    if (!m_canvas->hasImage()) return;
    
    QSize size = m_canvas->imageSize();
    bool ok;
    int w = QInputDialog::getInt(this, "调整大小", "宽度:", size.width(), 1, 10000, 1, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "调整大小", "高度:", size.height(), 1, 10000, 1, &ok);
    if (!ok) return;
    
    m_canvas->resize(w, h);
    updateActionsState();
}

void MainWindow::setToolSelect() { m_canvas->setTool(ToolType::Select); updateActionsState(); }
void MainWindow::setToolCrop() { m_canvas->setTool(ToolType::Crop); updateActionsState(); }
void MainWindow::setToolBrush() { m_canvas->setTool(ToolType::Brush); updateActionsState(); }
void MainWindow::setToolEraser() { m_canvas->setTool(ToolType::Eraser); updateActionsState(); }
void MainWindow::setToolText() { m_canvas->setTool(ToolType::Text); updateActionsState(); }
void MainWindow::setToolPipette() { m_canvas->setTool(ToolType::Pipette); updateActionsState(); }

void MainWindow::applyFilters()
{
}

void MainWindow::updatePreview()
{
}

void MainWindow::updateImageFromCanvas(const QImage &image)
{
    Q_UNUSED(image);
    updateActionsState();
}

void MainWindow::updateStatusBar(const QString &message)
{
    m_statusLabel->setText(message);
}

bool MainWindow::maybeSave()
{
    if (!m_canvas->isModified()) return true;
    
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "PhotoEditor",
        "图像已被修改，是否保存更改？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);
    
    if (ret == QMessageBox::Save) {
        saveImage();
        return !m_canvas->isModified();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    if (!m_canvas->imageForExport().save(fileName)) {
        QMessageBox::warning(this, "PhotoEditor", "无法保存文件: " + fileName);
        return false;
    }
    setCurrentFile(fileName);
    m_canvas->setModified(false);
    updateActionsState();
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    if (!m_canvas->loadImage(fileName)) {
        QMessageBox::warning(this, "PhotoEditor", "无法打开文件: " + fileName);
        return;
    }
    m_adjustmentPanel->resetValues();
    m_canvas->resetAdjustments();
    m_filterPanel->resetToDefault();
    setCurrentFile(fileName);
    updateActionsState();
    updateZoomLabel();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    setWindowModified(m_canvas->isModified());
    QString shownName = fileName.isEmpty() ? "未命名" : QFileInfo(fileName).fileName();
    setWindowTitle(QString("%1 - PhotoEditor").arg(shownName));
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于 PhotoEditor",
        "<h3>PhotoEditor 1.0</h3>"
        "<p>基于 Qt 的 2D 图像编辑器，类似美图秀秀的桌面工具。</p>"
        "<p>功能：打开/保存、裁剪、旋转、翻转、调整亮度/对比度/饱和度、"
        "多种滤镜、画笔、橡皮擦、文字、取色器等。</p>");
}

void MainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this);
}
