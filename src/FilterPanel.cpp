#include "FilterPanel.h"
#include <QVBoxLayout>
#include <QPushButton>

FilterPanel::FilterPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void FilterPanel::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    m_filterList = new QListWidget();
    m_filterList->addItem("无");
    m_filterList->addItem("灰度");
    m_filterList->addItem("怀旧/复古");
    m_filterList->addItem("黑白");
    m_filterList->addItem("暖色");
    m_filterList->addItem("冷色");
    m_filterList->addItem("锐化");
    m_filterList->addItem("模糊");
    m_filterList->addItem("浮雕");
    m_filterList->addItem("反相");
    m_filterList->setCurrentRow(0);
    
    layout->addWidget(new QLabel("滤镜:"));
    layout->addWidget(m_filterList);
    
    m_intensityLabel = new QLabel("强度: 100%");
    layout->addWidget(m_intensityLabel);
    
    m_intensitySlider = new QSlider(Qt::Horizontal);
    m_intensitySlider->setRange(1, 100);
    m_intensitySlider->setValue(100);
    layout->addWidget(m_intensitySlider);
    
    connect(m_filterList, &QListWidget::currentTextChanged, this, [this](const QString &text) {
        QString filter = "";
        if (text == "灰度") filter = "grayscale";
        else if (text == "怀旧/复古") filter = "vintage";
        else if (text == "黑白") filter = "sepia";
        else if (text == "暖色") filter = "warm";
        else if (text == "冷色") filter = "cool";
        else if (text == "锐化") filter = "sharpen";
        else if (text == "模糊") filter = "blur";
        else if (text == "浮雕") filter = "emboss";
        else if (text == "反相") filter = "invert";
        emit filterSelected(filter);
    });
    
    connect(m_intensitySlider, &QSlider::valueChanged, this, [this](int value) {
        m_intensityLabel->setText(QString("强度: %1%").arg(value));
        emit intensityChanged(value);
    });
    
    layout->addStretch();
}

QString FilterPanel::currentFilter() const
{
    QString text = m_filterList->currentItem() ? m_filterList->currentItem()->text() : "";
    if (text == "灰度") return "grayscale";
    if (text == "怀旧/复古") return "vintage";
    if (text == "黑白") return "sepia";
    if (text == "暖色") return "warm";
    if (text == "冷色") return "cool";
    if (text == "锐化") return "sharpen";
    if (text == "模糊") return "blur";
    if (text == "浮雕") return "emboss";
    if (text == "反相") return "invert";
    return "";
}

int FilterPanel::filterIntensity() const
{
    return m_intensitySlider->value();
}

void FilterPanel::resetToDefault()
{
    m_filterList->setCurrentRow(0);
    m_intensitySlider->setValue(100);
}
