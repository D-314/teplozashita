#include "temperature_data.h"

temperature_data::oneDay temperature_data::OneDay(float thermalEnergy, float temperatureInside, float temperatureOutside) {
    oneDay temporary;
    temporary.thermalEnergy = thermalEnergy;
    temporary.temperatureInside = temperatureInside;
    temporary.temperatureOutside = temperatureOutside;
    return temporary;
}

void temperature_data::addElement(QDate day, float thermalEnergy, float temperatureInside, float temperatureOutside) {
    if (data.find(day) == data.end()) {
        data.insert(std::make_pair(day, OneDay(thermalEnergy,temperatureInside,temperatureOutside)));
    } else {
        data[day] = OneDay(thermalEnergy,temperatureInside,temperatureOutside);
    }
}

void temperature_data::writeThermalEnergy(QDate day, float thermalEnergy)
{
    if (data.find(day) == data.end()) {
        data.insert(std::make_pair(day, OneDay(thermalEnergy,24,INFINITY)));
    } else {
        data[day].thermalEnergy = thermalEnergy;
    }
}

void temperature_data::writeTemperatureInside(QDate day, float temperatureInside)
{
    if (data.find(day) == data.end()) {
        data.insert(std::make_pair(day, OneDay(0,temperatureInside,INFINITY)));
    } else {
        data[day].temperatureInside = temperatureInside;
    }
}

void temperature_data::writeTemperatureOutside(QDate day, float temperatureOutside)
{
    if (data.find(day) == data.end()) {
        data.insert(std::make_pair(day, OneDay(0,23,temperatureOutside)));
    } else {
        data[day].temperatureOutside = temperatureOutside;
    }
}

temperature_data::temperature_data()
{

}

float temperature_data::thermalEnergyPerHour(const QDate day)
{
    float q = 0;
    if (data.find(day) != data.end()) {
        q = data.at(day).thermalEnergy / 24;
    }
    return q;
}

float temperature_data::power(const QDate day)
{
    float p = 0;
    p =  thermalEnergyPerHour(day)/0.000860421;
    return p;
}

float temperature_data::deltaTemperature(const QDate day)
{
    float dT = 0;
    if ((data.find(day) != data.end()) &&(data.at(day).temperatureOutside != INFINITY)) {
        dT = data.at(day).temperatureInside - data.at(day).temperatureOutside;
    }
    return dT;
}

float temperature_data::kilowattHour(const QDate day)
{
    float G = 0;
    float dT = deltaTemperature(day);
    if(dT != 0) {
        G = (float)power(day)/(float)dT;
    }
    return G;
}
