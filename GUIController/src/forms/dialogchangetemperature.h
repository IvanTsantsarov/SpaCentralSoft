#ifndef DIALOGCHANGETEMPERATURE_H
#define DIALOGCHANGETEMPERATURE_H

#include <QDialog>

namespace Ui {
class DialogChangeTemperature;
}

class DialogChangeTemperature : public QDialog
{
    Q_OBJECT

private:
    quint8 _sensorId;

public:
    explicit DialogChangeTemperature(QWidget* parent,
                                     QString dialogTitle,
                                     quint8 sensorId,
                                     bool temperatureIndicatorFunctional,
                                     qreal temperatureDesired,
                                     qreal temperatureCurrent);
    ~DialogChangeTemperature();

private:
    Ui::DialogChangeTemperature *ui;

public:
    qreal getValue();

};

#endif // DIALOGCHANGETEMPERATURE_H
