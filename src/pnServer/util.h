#ifndef util_h__
#define util_h__

#include <QMap>
#include <QString>
#include <QStringList>

namespace pacsnode {

class Util
{
public:
    typedef QMap<QString, QString> ParameterMap;

    static ParameterMap parseFromUrlEncoded(const QString& str);
};

}

#endif // util_h__
