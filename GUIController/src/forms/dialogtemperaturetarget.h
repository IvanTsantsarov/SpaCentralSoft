#ifndef DIALOGTEMPERATURETARGET_H
#define DIALOGTEMPERATURETARGET_H

#include <QDialog>

namespace Ui {
class DialogTemperatureTarget;
}

class DialogTemperatureTarget : public QDialog
{
    Q_OBJECT

private:
    quint8 _sensorId;

public:
    explicit DialogTemperatureTarget(QWidget* parent,
                                     QString dialogTitle,
                                     quint8 sensorId,
                                     bool temperatureIndicatorFunctional,
                                     qreal temperatureTarget,
                                     qreal temperatureCurrent);
    ~DialogTemperatureTarget();

private:
    Ui::DialogTemperatureTarget *ui;

public:
    qreal getValue();

};

#endif // DIALOGTEMPERATURETARGET_H
