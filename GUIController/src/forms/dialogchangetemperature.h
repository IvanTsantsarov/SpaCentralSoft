#ifndef DIALOGCHANGETEMPERATURE_H
#define DIALOGCHANGETEMPERATURE_H

#include <QDialog>

namespace Ui {
class DialogChangeTemperature;
}

class DialogChangeTemperature : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangeTemperature(QWidget *parent,
                                     QString dialogTitle,
                                     bool temperatureIndicatorFunctional,
                                     qreal temperatureDesired,
                                     qreal temperatureCurrent);
    qreal getValue();
    ~DialogChangeTemperature();

private:
    Ui::DialogChangeTemperature *ui;
};

#endif // DIALOGCHANGETEMPERATURE_H
