#ifndef mainServer_h__
#define mainServer_h__

#include <QObject>

namespace pacsnode {

class MainServerPrivate;
class MainServer : public QObject
{
    Q_OBJECT
public:
    MainServer(QObject* parent = NULL);
    ~MainServer();

public Q_SLOTS:

    void start();

    void stop();

private:
    Q_DISABLE_COPY(MainServer)
    MainServerPrivate* d;
};

}
#endif // mainServer_h__

