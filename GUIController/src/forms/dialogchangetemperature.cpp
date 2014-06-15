#include "dialogchangetemperature.h"
#include "ui_dialogchangetemperature.h"

DialogChangeTemperature::DialogChangeTemperature(QWidget *parent,
                                                 QString dialogTitle,
                                                 bool temperatureIndicatorFunctional,
                                                 qreal temperatureDesired,
                                                 qreal temperatureCurrent) :
    QDialog(parent),
    ui(new Ui::DialogChangeTemperature)
{
    ui->setupUi(this);
    this->setWindowTitle(dialogTitle);

    QString temperature("");
    if (temperatureIndicatorFunctional) {
        temperature += QString("%1° /").arg(temperatureCurrent, 1);
    }
    else {
        temperature += QString("?° /");
    }

    ui->label_TemperatureCurrent->setText(temperature);
    ui->spinBox_TemperatureDesired->setValue(temperatureDesired);
}

qreal DialogChangeTemperature::getValue()
{
    return ui->spinBox_TemperatureDesired->value();
}

DialogChangeTemperature::~DialogChangeTemperature()
{
    delete ui;
}
