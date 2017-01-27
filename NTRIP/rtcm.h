#ifndef RTCM_H
#define RTCM_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>

#include "rtklib.h"

class Rtcm : public QObject
{
    Q_OBJECT
public:
    explicit Rtcm();
    ~Rtcm();

protected:
    QThread * thread;

public Q_SLOTS:
    void slotDecodeRtcm(QByteArray rtcm);
Q_SIGNALS:
    void signalRtcmDecoded(rtcm_t rtcm);
};

#endif // RTCM_H
