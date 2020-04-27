#ifndef EventFilter_H
#define EventFilter_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class EventFilter : public QObject, public QAbstractNativeEventFilter
{                 
    Q_OBJECT
public:
     EventFilter (QObject *parent = NULL);
    ~EventFilter();

    bool nativeEventFilter( const QByteArray& eventType, void* message, long* result );

signals:
    void closeEventReceived();

};

#endif
