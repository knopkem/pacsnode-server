#include "imageWorker.h"

#include "sqlDatabase.h"
#include "dicomHelper.h"
#include "storage.h"

#include <QtCore>
#include <QtSql>

namespace pacsnode {

class ImageWorkerPrivate
{
public:
    QString filePath;
    qlonglong id;
};

ImageWorker::ImageWorker( const QString& filePath, qlonglong id ) : d(new ImageWorkerPrivate)
{
    d->filePath = filePath;
    d->id = id;
}

ImageWorker::~ImageWorker()
{
    delete d;
    d = 0;
}

void ImageWorker::run()
{
    QScopedPointer<SqlDatabase> sqlDb(new SqlDatabase(pacsnode::storageLocation()));
    // using the original filepath as base for thumbnails
    QString thumbFolder = DicomHelper::thumbFolder(d->filePath);

    // create subdirectory
    if (!QDir().exists(thumbFolder)) {
        if (!QDir().mkdir(thumbFolder)) {
            qCritical() << "failed to create directory" << thumbFolder;
            return;
        }
    }

    // qDebug() << "processing file" << d->filePath << thumbFolder;

    // create png representation of the file
    QString bufferPath = DicomHelper::dcm2png(d->filePath, thumbFolder);
    QString success = "true";
    if (bufferPath.isEmpty()) {
        qCritical() << "failed to process dicom image" << d->filePath;
        success = "false";
    }

    // query prepare
    QSqlQuery query(sqlDb->openConnection());
    QString prepare = "UPDATE image SET processed = '" + success + "' WHERE id = " + QString::number(d->id);

    // print warning
    if (!query.prepare(prepare)) {
        qWarning() << prepare;
        qWarning() << query.lastError();
    }

    // query exec
    EXEC_QUERY(query);
}

}
