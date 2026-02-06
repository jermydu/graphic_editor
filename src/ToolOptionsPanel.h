#ifndef TOOLOPTIONSPANEL_H
#define TOOLOPTIONSPANEL_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>

class ToolOptionsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ToolOptionsPanel(QWidget *parent = nullptr);

signals:
    void brushSizeChanged(int size);
    void brushColorChanged(const QColor &color);
    void eraserSizeChanged(int size);

public slots:
    void setCurrentTool(int toolType);

private:
    void setupUi();
    
    QWidget *m_brushOptions;
    QWidget *m_eraserOptions;
    QSlider *m_brushSizeSlider;
    QSpinBox *m_brushSizeSpin;
    QPushButton *m_colorButton;
    QSlider *m_eraserSizeSlider;
    QSpinBox *m_eraserSizeSpin;
    QStackedWidget *m_stack;
};

#endif // TOOLOPTIONSPANEL_H
