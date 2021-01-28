#ifndef ONEDRAGTIMER_H
#define ONEDRAGTIMER_H

#include <QTimer>

class OneDragTimer : public QTimer
{
    Q_OBJECT
public:
    explicit OneDragTimer(QObject *parent = nullptr):
        QTimer(parent)
    {
        setTimerType(Qt::PreciseTimer);
    }

signals:

public slots:
    void getInterval(int value) {setInterval(value);}
    void beginTimer() {start(10);}
    void stopTimer() {stop();}
};

#endif // ONEDRAGTIMER_H
