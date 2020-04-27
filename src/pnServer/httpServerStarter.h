#ifndef httpServerStarter_h__
#define httpServerStarter_h__

#include <QObject>

namespace pacsnode {

class HttpServerPrivate;
class HttpServerStarter  : public QObject
{
    Q_OBJECT
public:
    HttpServerStarter(QObject* parent = NULL);
    ~HttpServerStarter();

public Q_SLOTS:

    void start();

    void stop();

protected:

private:
    HttpServerPrivate* d;
};

}

#endif // httpServerStarter_h__
