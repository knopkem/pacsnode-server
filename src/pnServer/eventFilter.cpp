#include "eventFilter.h"

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

EventFilter::EventFilter (QObject *parent)
    : QObject (parent)
{
}

EventFilter::~EventFilter()
{
}

bool EventFilter::nativeEventFilter( const QByteArray& eventType, void* message, long* result )
{
#ifdef Q_OS_WIN32
    MSG* msg = reinterpret_cast<MSG*>( message );
    if ( msg->message == WM_CLOSE ) {
        emit closeEventReceived();
        return true;
    }

    return false;
#endif
}
