#ifndef CommonHelper_h__
#define CommonHelper_h__

#include "pnCore_Export.h"

#include <QString>
#include <iostream>

namespace pacsnode {

/**
 * Helper for common tasks,
 * should be extended
 */
class pnCore_EXPORT CommonHelper
{
public:


    //! Create directory and any parent directories if needed.
    static bool mkpath(const QString& dirPath);

    //! Removes the directory path dirPath
    static bool rmpath(const QString& dirPath);

    //! returns a (truly) random int using time as seed
    static int randomize();

    static int QStringToInt(const QString& input);

    static int stdStringToInt(const std::string& input);

    static bool isAppRunningAsAdminMode();

    static void msleep(int msecs);

    static QStringList traverseDir(QString path);


};

}
#endif // CommonHelper_h__

