#include "backend.h"
#include <QDebug>

backend::backend(QObject *parent) : QObject(parent)
{
    m_azanTime = "13:40";
    m_azanName = "Dhur";
    m_ipAddr = "192.168.1.1";
    m_portAddr = "1883";

    m_hourVal = "00";
    m_minuteVal = "00";

    m_hourClock = 20;
    m_minuteClock = 54;
    m_hourAlarm = 13;
    m_minuteAlarm = 12;
}

QString backend::azanTime()
{
    return m_azanTime;
}

void backend::setAzanTime(const QString &value)
{
    m_azanTime = value;
    emit setAzanTimeChanged();
}

QString backend::azanName()
{
    return m_azanName;
}

void backend::setAzanName(const QString &value)
{
    m_azanName = value;
    emit setAzanNameChanged();
}

QString backend::ipAddr()
{
    return m_ipAddr;
}

void backend::setIPAddr(const QString &value)
{
     m_ipAddr = value;
     qDebug() << "[IP ADDRESS]: " << value;
     emit setipAddrChanged();
}

QString backend::portAddr()
{
    return m_portAddr;
}

void backend::setPortAddr(const QString &value)
{
     m_portAddr = value;
     emit setPortAddrChanged();
}

QString backend::hourVal()
{
    return m_hourVal;
}

void backend::setHourVal(const QString &value)
{
     m_portAddr = value;
     emit setHourValChanged();
}

QString backend::minuteVal()
{
    return m_minuteVal;
}

QString backend::sendAlarm()
{
    return m_sendAlarm;
}

void backend::setMinuteVal(const QString &value)
{
     m_minuteVal = value;
     emit setMinuteValChanged();
}

void backend::setSendAlarm(const QString &value)
{
    m_sendAlarm = value;
    emit setSendAlarmChanged();
    emit sendAlarmMqtt(value);
}

int backend::hourClock()
{
    return m_hourClock;
}

void backend::setHourClock(const int &value)
{
    m_hourClock = value;
    emit setHourClockChanged();
}

int backend::minuteClock()
{
    return m_minuteClock;
}

void backend::setMinuteClock(const int &value)
{
    m_minuteClock = value;
    emit setMinuteClockChanged();
}

int backend::hourAlarm()
{
    return m_hourAlarm;
}

void backend::setHourAlarm(const int &value)
{
    m_hourAlarm = value;
    emit setHourAlarmChanged();
}

int backend::minuteAlarm()
{
    return m_minuteAlarm;
}

void backend::setMinuteAlarm(const int &value)
{
    m_minuteAlarm = value;
    emit setMinuteAlarmChanged();
}

