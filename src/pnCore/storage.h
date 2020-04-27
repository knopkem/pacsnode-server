#ifndef storage_h__
#define storage_h__

#include <QCoreApplication>
#include <QStandardPaths>

namespace pacsnode {

    static inline QString storageLocation() {
        return QCoreApplication::applicationDirPath() + "/archive";
    }

}
#endif // storage_h__

