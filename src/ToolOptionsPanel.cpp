#include "ToolOptionsPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QStackedWidget>

static const int TOOL_SELECT = 0;
static const int TOOL_CROP = 1;
static const int TOOL_BRUSH = 2;
static const int TOOL_ERASER = 3;
static const int TOOL_TEXT = 4;
static const int TOOL_PIPETTE = 5;

ToolOptionsPanel::ToolOptionsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ToolOptionsPanel::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QStackedWidget *stack = new QStackedWidget();
    
    m_brushOptions = new QWidget();
    QVBoxLayout *brushLayout = new QVBoxLayout(m_brushOptions);
    brushLayout->addWidget(new QLabel("画笔大小:"));
    QHBoxLayout *brushRow = new QHBoxLayout();
    m_brushSizeSlider = new QSlider(Qt::Horizontal);
    m_brushSizeSlider->setRange(1, 50);
    m_brushSizeSlider->setValue(5);
    brushRow->addWidget(m_brushSizeSlider, 1);
    m_brushSizeSpin = new QSpinBox();
    m_brushSizeSpin->setRange(1, 50);
    m_brushSizeSpin->setValue(5);
    brushRow->addWidget(m_brushSizeSpin);
    brushLayout->addLayout(brushRow);
    m_colorButton = new QPushButton("颜色");
    m_colorButton->setStyleSheet("background-color: black;");
    connect(m_colorButton, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(m_colorButton->palette().button().color(), this, "选择画笔颜色");
        if (c.isValid()) {
            m_colorButton->setStyleSheet(QString("background-color: %1;").arg(c.name()));
            emit brushColorChanged(c);
        }
    });
    brushLayout->addWidget(m_colorButton);
    brushLayout->addStretch();
    stack->addWidget(m_brushOptions);
    
    m_eraserOptions = new QWidget();
    QVBoxLayout *eraserLayout = new QVBoxLayout(m_eraserOptions);
    eraserLayout->addWidget(new QLabel("橡皮大小:"));
    QHBoxLayout *eraserRow = new QHBoxLayout();
    m_eraserSizeSlider = new QSlider(Qt::Horizontal);
    m_eraserSizeSlider->setRange(5, 100);
    m_eraserSizeSlider->setValue(20);
    eraserRow->addWidget(m_eraserSizeSlider, 1);
    m_eraserSizeSpin = new QSpinBox();
    m_eraserSizeSpin->setRange(5, 100);
    m_eraserSizeSpin->setValue(20);
    eraserRow->addWidget(m_eraserSizeSpin);
    eraserLayout->addLayout(eraserRow);
    eraserLayout->addStretch();
    stack->addWidget(m_eraserOptions);
    
    QWidget *emptyWidget = new QWidget();
    QVBoxLayout *emptyLayout = new QVBoxLayout(emptyWidget);
    emptyLayout->addWidget(new QLabel("选择工具以显示选项"));
    emptyLayout->addStretch();
    stack->addWidget(emptyWidget);
    
    layout->addWidget(stack);
    
    connect(m_brushSizeSlider, &QSlider::valueChanged, m_brushSizeSpin, &QSpinBox::setValue);
    connect(m_brushSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_brushSizeSlider, &QSlider::setValue);
    connect(m_brushSizeSlider, &QSlider::valueChanged, this, &ToolOptionsPanel::brushSizeChanged);
    
    connect(m_eraserSizeSlider, &QSlider::valueChanged, m_eraserSizeSpin, &QSpinBox::setValue);
    connect(m_eraserSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_eraserSizeSlider, &QSlider::setValue);
    connect(m_eraserSizeSlider, &QSlider::valueChanged, this, &ToolOptionsPanel::eraserSizeChanged);
    
    m_stack = stack;
    setCurrentTool(TOOL_SELECT);
}

void ToolOptionsPanel::setCurrentTool(int toolType)
{
    if (!m_stack) return;
    
    if (toolType == TOOL_BRUSH) {
        m_stack->setCurrentWidget(m_brushOptions);
    } else if (toolType == TOOL_ERASER) {
        m_stack->setCurrentWidget(m_eraserOptions);
    } else {
        m_stack->setCurrentIndex(2);
    }
}
