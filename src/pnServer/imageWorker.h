#ifndef imageWorker_h__
#define imageWorker_h__

#include <QRunnable>

namespace pacsnode {

class ImageWorkerPrivate;
class ImageWorker : public QRunnable
{
public:
    ImageWorker(const QString& filePath, qlonglong id);
    ~ImageWorker();

    void run();

private:
    ImageWorker();
    Q_DISABLE_COPY(ImageWorker)
    ImageWorkerPrivate* d;
};

}

#endif // imageWorker_h__

