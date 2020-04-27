#ifndef dicomServer_h__
#define dicomServer_h__

#include <QObject>

#include "pnDicom_Export.h"

namespace pacsnode {

class SqlDatabase;

class DicomServerPrivate;
class pnDicom_EXPORT DicomServer : public QObject
{
    Q_OBJECT
public:
    DicomServer(QObject* parent = NULL);
    ~DicomServer();

public Q_SLOTS:

    void start();

    void stop();

private:
    Q_DISABLE_COPY(DicomServer)
    DicomServerPrivate* d;

};

}

#endif // dicomServer_h__
