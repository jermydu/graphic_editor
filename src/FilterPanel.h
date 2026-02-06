#ifndef FILTERPANEL_H
#define FILTERPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QSlider>
#include <QLabel>

class FilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilterPanel(QWidget *parent = nullptr);
    
    QString currentFilter() const;
    int filterIntensity() const;
    void resetToDefault();

signals:
    void filterSelected(const QString &filterName);
    void intensityChanged(int value);
    void applyClicked();

private:
    void setupUi();
    
    QListWidget *m_filterList;
    QSlider *m_intensitySlider;
    QLabel *m_intensityLabel;
};

#endif // FILTERPANEL_H
