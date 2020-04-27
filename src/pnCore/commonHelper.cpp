#include "commonHelper.h"

// Qt
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QThread>
#include <QDirIterator>

#ifdef Q_OS_WIN
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#endif

namespace {

class SleepThread : protected QThread
{
public:
    static void msleep (unsigned long msecs) { QThread::msleep (msecs) ; }
};

}
namespace pacsnode {

bool CommonHelper::mkpath(const QString& dirPath)
{
    QDir dir;
    return(dir.mkpath(dirPath));
}

//--------------------------------------------------------------------------------------------

bool CommonHelper::rmpath(const QString& dirPath)
{
    QDir dir;
    return(dir.rmpath(dirPath));
}

//--------------------------------------------------------------------------------------

int CommonHelper::randomize()
{
    // seeding the randomizer
    const uint randomSeed = QDateTime::currentDateTime().toTime_t();
    qsrand( randomSeed );

    return rand();
}

//--------------------------------------------------------------------------------------

int CommonHelper::QStringToInt( const QString& input )
{
    return input.toInt();
}

//--------------------------------------------------------------------------------------

int CommonHelper::stdStringToInt( const std::string& input )
{
    return QStringToInt(QString::fromStdString(input));
}

//--------------------------------------------------------------------------------------

bool CommonHelper::isAppRunningAsAdminMode()
{
#ifdef Q_OS_WIN
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup)) {
            dwError = GetLastError();
            goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin)) {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup) {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError) {
        throw dwError;
    }

    return fIsRunAsAdmin;
#endif
    return false;
}

//--------------------------------------------------------------------------------------------

void CommonHelper::msleep( int msecs )
{
    SleepThread::msleep (msecs);
}

//--------------------------------------------------------------------------------------------

QStringList CommonHelper::traverseDir( QString path )
{
    QStringList fileList;
    QDir dir(path);
    dir.setFilter(QDir::Files);

    if ( dir.exists() ) {
        QDirIterator directory_walker(path, QDir::Files, QDirIterator::Subdirectories);
        while( directory_walker.hasNext() ) {
            QString file = directory_walker.next();
            fileList << file;
        }
    }
    else {
        qWarning() << "directory does not exist:" << dir;
    }

    fileList.sort();

    return fileList;
}

//--------------------------------------------------------------------------------------


}

