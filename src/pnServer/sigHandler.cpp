#include "sigHandler.h"

#include <assert.h>

#ifndef _WIN32
#include <signal.h>
#else
#include <windows.h>
#include <set>
#endif //!_WIN32

#include <QDebug>

namespace { // protect in anonymous namespace

    // There can be only ONE SigHandler per process
    SigHandler* g_handler(NULL);

#ifdef _WIN32
    std::set<int> g_registry;
#endif

#ifdef _WIN32
DWORD WIN32_logicalToPhysical(int signal)
{
    switch (signal)
    {
    case SigHandler::SIG_INT:   return CTRL_C_EVENT;
    case SigHandler::SIG_TERM:  return CTRL_BREAK_EVENT;
    case SigHandler::SIG_CLOSE: return CTRL_CLOSE_EVENT;
    default: 
        return ~(unsigned int)0; // SIG_ERR = -1
    }
}
#else
int POSIX_logicalToPhysical(int signal)
{
    switch (signal)
    {
    case SigHandler::SIG_INT:  return SIGINT;
    case SigHandler::SIG_TERM: return SIGTERM;
    // In case the client asks for a SIG_CLOSE handler, accept and
    // bind it to a SIGTERM. Anyway the signal will never be raised
    case SigHandler::SIG_CLOSE:  return SIGTERM;
    case SigHandler::SIG_RELOAD: return SIGHUP;
    default: 
        return -1; // SIG_ERR = -1
    }
}
#endif //_WIN32


#ifdef _WIN32
int WIN32_physicalToLogical(DWORD signal)
{
    switch (signal)
    {
    case CTRL_C_EVENT:     return SigHandler::SIG_INT;
    case CTRL_BREAK_EVENT: return SigHandler::SIG_TERM;
    case CTRL_CLOSE_EVENT: return SigHandler::SIG_CLOSE;
    default:
        return SigHandler::SIG_UNHANDLED;
    }
}
#else
int POSIX_physicalToLogical(int signal)
{
    switch (signal)
    {
    case SIGINT: return SigHandler::SIG_INT;
    case SIGTERM: return SigHandler::SIG_TERM;
    case SIGHUP: return SigHandler::SIG_RELOAD;
    default:
        return SigHandler::SIG_UNHANDLED;
    }
}
#endif //_WIN32

#ifdef _WIN32
BOOL WINAPI WIN32_handleFunc(DWORD signal)
{
    if (!g_handler)
        return FALSE;

    int signo = WIN32_physicalToLogical(signal);
    // The std::set is thread-safe in const reading access and we never
    // write to it after the program has started so we don't need to 
    // protect this search by a mutex
    std::set<int>::const_iterator found = g_registry.find(signo);
    if ( (signo != -1) && (found != g_registry.end()) ) {
        return g_handler->handleSignal(signo) ? TRUE : FALSE;
    }
    
    return FALSE;
}
#else
void POSIX_handleFunc(int signal)
{
    if (!g_handler)
        return;

    int signo = POSIX_physicalToLogical(signal);
    g_handler->handleSignal(signo);
}
#endif //_WIN32
}

SigHandler::SigHandler(int mask, QObject *parent) :
  QObject(parent), _mask(mask)
{    
    Q_ASSERT_X( (g_handler==NULL), "Warning!", "only one instance of SigHandler can live at a time, go fix your code");

    g_handler = this;

#ifdef _WIN32
    SetConsoleCtrlHandler(WIN32_handleFunc, TRUE);
#endif //_WIN32

    for (int i=0; i<SigHandler::SIG_COUNT; i++) {
        int logical = 0x1 << i;
        if (_mask & logical) {
#ifdef _WIN32
            g_registry.insert(logical);
#else
            int sig = POSIX_logicalToPhysical(logical);
            if (signal(sig, POSIX_handleFunc) == SIG_ERR) {
                qWarning() << "cannot install handler on signal" << logical;
            }
#endif //_WIN32
        }
    }
}

SigHandler::~SigHandler()
{
#ifdef _WIN32
    SetConsoleCtrlHandler(WIN32_handleFunc, FALSE);
    g_registry.clear();
#else
    for (int i=0; i<SigHandler::SIG_COUNT; i++) {
        int logical = 0x1 << i;
        if (_mask & logical) {
            signal(POSIX_logicalToPhysical(logical), SIG_DFL);
        }
    }
#endif //_WIN32
    g_handler = NULL;
}

bool SigHandler::handleSignal(int signal)
{
    emit signalReceived(signal);
    return true;
}
