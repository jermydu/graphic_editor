#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QLabel>
#include <QImage>
#include <QActionGroup>
#include <QSlider>
#include <QSpinBox>
#include <QScrollArea>
#include "ImageCanvas.h"
#include "AdjustmentPanel.h"
#include "FilterPanel.h"
#include "ToolOptionsPanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openImage();
    void saveImage();
    void saveImageAs();
    void newImage();
    void printImage();
    void undo();
    void redo();
    void copy();
    void paste();
    
    void cropImage();
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();
    void resizeImage();
    
    void setToolSelect();
    void setToolCrop();
    void setToolBrush();
    void setToolEraser();
    void setToolText();
    void setToolPipette();
    
    void applyFilters();
    void updatePreview();
    void updateImageFromCanvas(const QImage &image);
    void updateStatusBar(const QString &message);
    
    void showAbout();
    void showAboutQt();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidgets();
    void setupConnections();
    void updateActionsState();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    ImageCanvas *m_canvas;
    QScrollArea *m_scrollArea;
    AdjustmentPanel *m_adjustmentPanel;
    FilterPanel *m_filterPanel;
    ToolOptionsPanel *m_toolOptionsPanel;
    
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_cropAction;
    QActionGroup *m_toolGroup;
    
    QLabel *m_statusLabel;
    QLabel *m_zoomLabel;
    
    void updateZoomLabel();
    
    QString m_currentFile;
    QString m_lastDirectory;
};

#endif // MAINWINDOW_H
