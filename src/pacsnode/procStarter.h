#ifndef ProcStarter_h__
#define ProcStarter_h__

#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>

namespace pacsnode {

class ProcStarterPrivate;
class ProcStarter : public QObject
{
    Q_OBJECT
public:

    ProcStarter(const QString& processName, const QString& filePath, QObject* parent = NULL);
    ~ProcStarter();

    bool isRunning() const;

    void waitForFinished(int waitTime);

    QProcess* processInstance() const;

    static QString errorToString(QProcess::ProcessError error);

    static QString exitStatusToString(QProcess::ExitStatus status);

    void setProcessEnvironment(const QProcessEnvironment& env);

    QString processName() const;

public slots:
    void start(const QStringList& argv= QStringList(), int waitTime = 0);
    void stop();
    
signals:
    void started();
    void finished();
    void notify(const QString& msg);

protected slots:
    void logProcessOutput();
    void logProcessError();
    void onProcessStarted();
    void onProcessError( QProcess::ProcessError error );
    void onProcessFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void forwardMessage(const QString& message);

private:
    ProcStarterPrivate* d;
};

}

#endif // ProcStarter_h__
