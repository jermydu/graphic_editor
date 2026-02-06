#include "AdjustmentPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

AdjustmentPanel::AdjustmentPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void AdjustmentPanel::resetValues()
{
    m_brightnessSlider->setValue(100);
    m_contrastSlider->setValue(100);
    m_saturationSlider->setValue(100);
}

void AdjustmentPanel::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QGroupBox *group = new QGroupBox("图像调整");
    QVBoxLayout *groupLayout = new QVBoxLayout(group);
    
    auto addSlider = [this, groupLayout](const QString &label, QSlider *&slider, QSpinBox *&spin, 
                                          int minVal, int maxVal, int defVal,
                                          void (AdjustmentPanel::*signal)(int)) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *lbl = new QLabel(label);
        lbl->setFixedWidth(60);
        row->addWidget(lbl);
        
        slider = new QSlider(Qt::Horizontal);
        slider->setRange(minVal, maxVal);
        slider->setValue(defVal);
        row->addWidget(slider, 1);
        
        spin = new QSpinBox();
        spin->setRange(minVal, maxVal);
        spin->setValue(defVal);
        spin->setFixedWidth(60);
        row->addWidget(spin);
        
        connect(slider, &QSlider::valueChanged, spin, &QSpinBox::setValue);
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), slider, &QSlider::setValue);
        connect(slider, &QSlider::valueChanged, this, signal);
        
        groupLayout->addLayout(row);
    };
    
    addSlider("亮度", m_brightnessSlider, m_brightnessSpin, 0, 200, 100, &AdjustmentPanel::brightnessChanged);
    addSlider("对比度", m_contrastSlider, m_contrastSpin, 0, 200, 100, &AdjustmentPanel::contrastChanged);
    addSlider("饱和度", m_saturationSlider, m_saturationSpin, 0, 200, 100, &AdjustmentPanel::saturationChanged);
    
    QPushButton *resetBtn = new QPushButton("重置");
    connect(resetBtn, &QPushButton::clicked, this, [this]() {
        resetValues();
        emit resetRequested();
    });
    groupLayout->addWidget(resetBtn);
    
    layout->addWidget(group);
    layout->addStretch();
}
