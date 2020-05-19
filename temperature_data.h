#ifndef TEMPERATURE_DATA_H
#define TEMPERATURE_DATA_H

#include <math.h>
#include <vector>
#include <QDate>
#include <map>

class temperature_data
{
public:

    QString object = "";

    struct oneDay {
        float thermalEnergy = 0;
        float temperatureInside = 23;
        float temperatureOutside = INFINITY;
    };

    std::map<QDate,oneDay> data;

    oneDay OneDay(float thermalEnergy,float temperatureInside,float temperatureOutside);

    void addElement(QDate day, float thermalEnergy, float temperatureInside, float temperatureOutside);

    void writeThermalEnergy (QDate day, float thermalEnergy);

    void writeTemperatureInside (QDate day, float temperatureInside);

    void writeTemperatureOutside (QDate day, float temperatureOutside);

    temperature_data(const QString obj) {object = obj;}

    temperature_data();

    float thermalEnergyPerHour (const QDate day);

    float power (const QDate day);

    float deltaTemperature (const QDate day);

    float kilowattHour (const QDate day);

private:

};

#endif // TEMPERATURE_DATA_H
