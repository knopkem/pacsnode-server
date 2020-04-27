#include "procStarter.h"

#include <QtCore>

namespace pacsnode {

class ProcStarterPrivate
{
public:
    QString   processName;
    QString   processPath;
    QProcess* processInstance;
    QStringList argv;
};

//--------------------------------------------------------------------------------------------

ProcStarter::ProcStarter( const QString& processName, const QString& filePath, QObject* parent ) :
    QObject(parent), d(new ProcStarterPrivate)
{
    d->processName = processName;

#ifdef Q_OS_WIN
    d->processPath = filePath + ".exe";
#else
    d->processPath = filePath;
#endif

    if (!QFile::exists(d->processPath))
        qCritical() << "Process not found: " << d->processPath;


    d->processInstance = new QProcess(this);
    connect(d->processInstance, SIGNAL(readyReadStandardOutput()), this, SLOT(logProcessOutput()));
    connect(d->processInstance, SIGNAL(readyReadStandardError()), this, SLOT(logProcessError()));
    connect(d->processInstance, SIGNAL(started()), this, SLOT(onProcessStarted())); 
    connect(d->processInstance, SIGNAL(started()), this, SIGNAL(started())); 
    connect(d->processInstance, SIGNAL(error( QProcess::ProcessError)), this, SLOT(onProcessError( QProcess::ProcessError))); 
    connect(d->processInstance, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));
    connect(d->processInstance, SIGNAL(finished(int,QProcess::ExitStatus)), this, SIGNAL(finished()));
}

//--------------------------------------------------------------------------------------------

ProcStarter::~ProcStarter()
{
    delete d;
}

//--------------------------------------------------------------------------------------------

void ProcStarter::logProcessOutput()
{
    QByteArray bytes = d->processInstance->readAllStandardOutput();
    QString message(bytes.constData());
    forwardMessage(message);
}

//--------------------------------------------------------------------------------------------

void ProcStarter::logProcessError()
{
    QByteArray bytes = d->processInstance->readAllStandardError();
    QString message(bytes.constData());
    forwardMessage(message);
}

//--------------------------------------------------------------------------------------------

void ProcStarter::forwardMessage( const QString& message )
{
    // QStringList msg = message.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    emit notify(message);
}

//--------------------------------------------------------------------------------------------

void ProcStarter::onProcessError( QProcess::ProcessError error )
{
    qCritical() << "process "<< d->processName << " reports error:" << errorToString(error);
}

//--------------------------------------------------------------------------------------------

void ProcStarter::onProcessStarted()
{
    qDebug() << "process "<< d->processName << " started (" << d->processPath << ")";
}

//--------------------------------------------------------------------------------------------

void ProcStarter::onProcessFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    if (exitCode == 0) {
        qDebug() << "process " << d->processName << " finished normally";
    } else {
        qWarning() << "process "<< d->processName << " finished with exit code:" << exitCode << " and status:" << exitStatusToString(exitStatus);
    }
}

//--------------------------------------------------------------------------------------------

void ProcStarter::start(const QStringList& argv, int waitTime)
{
    d->argv = argv;
    d->processInstance->start(d->processPath, argv);
    if (waitTime > 0) {
        if( !d->processInstance->waitForStarted(waitTime) ) {
            qWarning() << "waitForStarted timed out for process" << d->processName;
        }
    }
}

//--------------------------------------------------------------------------------------------

void ProcStarter::stop()
{   
    if ( !isRunning() ) {
        qWarning() << "cannot stop process " << d->processName << " which is not running";
        return;
    }

    d->processInstance->terminate ();

    if( !d->processInstance->waitForFinished( 10000 ) ) {
        qWarning() << "Process" << d->processName << " did not quit, will terminate now";
    }

    if ( isRunning() ) {
        qWarning() << "terminating process" << d->processName;
        d->processInstance->kill();
    }
}

//--------------------------------------------------------------------------------------------

bool ProcStarter::isRunning() const
{
    return (d->processInstance->state() == QProcess::Running);
}

//--------------------------------------------------------------------------------------------

void ProcStarter::waitForFinished( int waitTime )
{
    d->processInstance->waitForFinished(waitTime);
}

//--------------------------------------------------------------------------------------------
//static
QString ProcStarter::errorToString(QProcess::ProcessError error)
{
    switch(error) {
    case QProcess::FailedToStart:
        return "failed to start";

    case QProcess::Crashed:
        return "failed to start";

    case QProcess::Timedout:
        return "timed out";

    case QProcess::ReadError:
        return "read error";

    case QProcess::WriteError:
        return "write error";

    case QProcess::UnknownError:
        return "unknown error";

    default:
        return "unknown or invalid";
    }

    //Unreachable.
}

//--------------------------------------------------------------------------------------------
//static
QString ProcStarter::exitStatusToString( QProcess::ExitStatus status )
{
    switch(status) 
    {
        case QProcess::CrashExit:
            return "CrashExit";

        case QProcess::NormalExit:
            return "NormalExit";

        default:
            return "unknown";
    }
    // Unreachable.
}

//--------------------------------------------------------------------------------------------

QProcess* ProcStarter::processInstance() const
{
    return d->processInstance;
}

//--------------------------------------------------------------------------------------------

QString ProcStarter::processName() const
{
    return d->processName;
}

//--------------------------------------------------------------------------------------------

void ProcStarter::setProcessEnvironment( const QProcessEnvironment& env )
{
    d->processInstance->setProcessEnvironment(env);
}

//--------------------------------------------------------------------------------------------

}
