#ifndef CONNECTIONTASK_H
#define CONNECTIONTASK_H

#include <QObject>
#include <QRunnable>

#include "structures.h"

class ConnectionTask : public QObject, public QRunnable
{
    Q_OBJECT

private:

    quintptr _socketDescriptor;

    const QList<SensorData>& _sensors;

    SensorData _sensorData;

public:
    explicit ConnectionTask(quintptr socketDescritor,
                            const QList<SensorData>& sensors);

private:
    void readDataFromSensor(quint8 sensorId, QByteArray& dataArray);

signals:
    void commandSent(SensorData sensorData);
    void sensorSet(SensorData sensorData);

public:
    void run();
    void setSocketDescriptor(quintptr socketDescriptor);
};

#endif // CONNECTIONTASK_H
