#ifndef STRUCTURES_H
#define STRUCTURES_H

struct SensorData
{
    char byte00;
    quint8 byte01;
    qint8 byte02;
    quint8 byte03;

    SensorData()
    {
    }

    SensorData(char byte00, quint8 byte01, qint8 byte02, quint8 byte03)
    {
        this->byte00 = byte00;
        this->byte01 = byte01;
        this->byte02 = byte02;
        this->byte03 = byte03;
    }
};

#endif // STRUCTURES_H
