
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

#include "mainServer.h"
#include "sigHandler.h"
#include "eventFilter.h"
#include "storage.h"

#include <Logger.h>
#include <ConsoleAppender.h>
#include <RollingFileAppender.h>

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    cuteLogger::Logger* instance = cuteLogger::loggerInstance();
    cuteLogger::ConsoleAppender* console = new cuteLogger::ConsoleAppender;
    instance->registerAppender(console);
    cuteLogger::RollingFileAppender* fileapp = new cuteLogger::RollingFileAppender;
    fileapp->setFileName("logfile.txt");
    instance->registerAppender(fileapp);

    QDir().mkdir(pacsnode::storageLocation());

    pacsnode::MainServer mainSrv;

    SigHandler sigHndl(SigHandler::SIG_INT | SigHandler::SIG_TERM | SigHandler::SIG_CLOSE);
    QObject::connect (&sigHndl, SIGNAL(signalReceived(int)), &mainSrv, SLOT(stop())); 

    EventFilter eFilter;
    QObject::connect (&eFilter, SIGNAL(closeEventReceived()), &mainSrv, SLOT(stop()));
    a.installNativeEventFilter(&eFilter);

    // start all
    mainSrv.start();

    return a.exec();
}

