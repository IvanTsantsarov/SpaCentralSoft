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


signals:
    void commandSent(SensorData sensorData);
    void setSensor(SensorData sensorData);

public:
    void run();
    void setSocketDescriptor(quintptr socketDescriptor);
};

#endif // CONNECTIONTASK_H
