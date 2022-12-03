#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <qqml.h>
#include <memory>
#include <QSettings>

class backend;
typedef std::shared_ptr<backend> BackendPtr;

class backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString azanTime READ azanTime WRITE setAzanTime NOTIFY setAzanTimeChanged)
    Q_PROPERTY(QString azanName READ azanName WRITE setAzanName NOTIFY setAzanNameChanged)
    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIPAddr NOTIFY setipAddrChanged)
    Q_PROPERTY(QString portAddr READ portAddr WRITE setPortAddr NOTIFY setPortAddrChanged)
    Q_PROPERTY(QString hourVal READ hourVal WRITE setHourVal NOTIFY setHourValChanged)
    Q_PROPERTY(QString minuteVal READ minuteVal WRITE setMinuteVal NOTIFY setMinuteValChanged)
    Q_PROPERTY(QString sendAlarm READ sendAlarm WRITE setSendAlarm NOTIFY setSendAlarmChanged)

    Q_PROPERTY(int hourClock READ hourClock WRITE setHourClock NOTIFY setHourClockChanged)
    Q_PROPERTY(int minuteClock READ minuteClock WRITE setMinuteClock NOTIFY setMinuteClockChanged)
    Q_PROPERTY(int hourAlarm READ hourAlarm WRITE setHourAlarm  NOTIFY setHourAlarmChanged)
    Q_PROPERTY(int minuteAlarm READ minuteAlarm WRITE setMinuteAlarm NOTIFY setMinuteAlarmChanged)



public:
    explicit backend(QObject *parent = nullptr);

    // configure azan time and name
    QString azanTime();
    QString azanName();


    // configure mqtt ip and port
    QString ipAddr();
    void setIPAddr(const QString &value);
    QString portAddr();
    void setPortAddr(const QString &value);

    // configure alarm hour and minutes
    QString hourVal();
    QString minuteVal();

    // configure mqtt alarm
    QString sendAlarm();


    //configure infotainment
    int hourClock();

    int minuteClock();

    int hourAlarm();

    int minuteAlarm();


public slots:
    void setAzanTime(const QString &value);
    void setAzanName(const QString &value);

    void setHourVal(const QString &value);
    void setMinuteVal(const QString &value);

    void setSendAlarm(const QString& value);

    void setHourClock(const int &value);
    void setMinuteClock(const int &value);
    void setHourAlarm(const int &value);
    void setMinuteAlarm(const int &value);

  signals:
      void setAzanTimeChanged();
      void setAzanNameChanged();
      void setipAddrChanged();
      void setPortAddrChanged();
      void setHourValChanged();
      void setMinuteValChanged();
      void setSendAlarmChanged();
      void sendAlarmMqtt(const QString& value);
      //configure infotainment

      void setHourClockChanged();
      void setMinuteClockChanged();
      void setHourAlarmChanged();
      void setMinuteAlarmChanged();


  private:
      QString m_azanTime;
      QString m_azanName;
      QString m_ipAddr;
      QString m_portAddr;
      QString m_hourVal;
      QString m_minuteVal;
      QString m_sendAlarm;

      int m_hourClock;
      int m_minuteClock;
      int m_hourAlarm;
      int m_minuteAlarm;

      std::unique_ptr<QSettings> m_settings;

};

#endif // BACKEND_H
