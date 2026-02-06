#ifndef ADJUSTMENTPANEL_H
#define ADJUSTMENTPANEL_H

#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

class AdjustmentPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AdjustmentPanel(QWidget *parent = nullptr);

public slots:
    void resetValues();

signals:
    void brightnessChanged(int value);
    void contrastChanged(int value);
    void saturationChanged(int value);
    void resetRequested();

private:
    void setupUi();
    
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QSlider *m_saturationSlider;
    QSpinBox *m_brightnessSpin;
    QSpinBox *m_contrastSpin;
    QSpinBox *m_saturationSpin;
};

#endif // ADJUSTMENTPANEL_H
