#pragma once

#include <QString>
#include <QDateTime>

class State
{
public:
    State();
    virtual QString toString();

    QString id;
    QDateTime DateHeure;
    float Longitude;
    float Latitude;
    float Gyro_yaw;
    float Gyro_pitch;
    float Gyro_roll;
    float Accel_x;
    float Accel_y;
    float Accel_z;
    float Temperature;
    int Humidite;
};
