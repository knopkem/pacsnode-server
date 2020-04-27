/*
 * Copyright (C) 2013 Michael Knopke
 *
 * See the COPYRIGHT file for terms of use.
 */

#include <QtGui>
#include <QtWidgets>

// core
#include <version_config.h>

// STL
//#include <iostream>

// own
#include "mainWindow.h"


QString readFile( const QString& path )
{
    QFile file(path);

    if(!file.open(QFile::ReadOnly)) {
        qWarning() << "Unable to read file" << path;
        return QString();
    }

    QString contents = file.readAll();
    file.close();
    return contents;
}


/**
 * Main application entry point
 */
int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(PN_ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(PN_ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(PN_APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(PN_VERSION_STRING);
    Q_INIT_RESOURCE(app);

    QApplication app(argc, argv);
    pacsnode::MainWindow window;

    qApp->setStyle("plastique");
    qApp->setStyleSheet(readFile(":/darkorange.stylesheet.qss"));

    QApplication::setQuitOnLastWindowClosed(true);
    window.show();
    return app.exec();
}

