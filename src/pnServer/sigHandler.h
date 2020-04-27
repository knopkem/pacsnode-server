#ifndef SigHandler_H
#define SigHandler_H

#include <QObject>

class SigHandler : public QObject
{
    Q_OBJECT
public:
     SigHandler(int mask = SIG_INT | SIG_TERM | SIG_CLOSE, QObject *parent = NULL);
    ~SigHandler();

    //! Enum to map platform-specific signal ID to a common interface
    enum SIGNALS
    {
        SIG_UNHANDLED   = 0,    // Physical signal not supported by this class
        SIG_NOOP        = 1,    // The application is requested to do a no-op (only a target that platform-specific signals map to when they can't be raised anyway)
        SIG_INT         = 2,    // Control+C (should terminate but consider that it's a normal way to do so; can delay a bit)
        SIG_TERM        = 4,    // Control+Break (should terminate now without regarding the consquences)
        SIG_CLOSE       = 8,    // Container window closed (should perform normal termination, like Ctrl^C) [Windows only; on Linux it maps to SIG_TERM]
        SIG_RELOAD      = 16,   // Reload the configuration [Linux only, physical signal is SIGHUP; on Windows it maps to SIG_NOOP]
        SIG_COUNT       = 6
    };

    virtual bool handleSignal(int signal);

signals:
    void signalReceived(int signal);

private:
    Q_DISABLE_COPY(SigHandler)
    int _mask;
};

#endif
