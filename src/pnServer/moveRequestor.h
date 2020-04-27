#ifndef moveRequestor_h__
#define moveRequestor_h__

#include <QObject>
#include <QString>
#include <QMutex>
#include <QStringList>

namespace pacsnode {

    class MoveRequestor : public QObject
    {
        Q_OBJECT
    public:
        static MoveRequestor* instance();

        void requestMove(const QString& studyUID, const QString& seriesUID, const QString& imageUID);

    private slots:
        void handleFinished();

    private:
        MoveRequestor();
        ~MoveRequestor();

        static MoveRequestor* m_Instance;
        static QMutex m_mutex;

        QStringList m_requested;
        QStringList m_resolved;
    };

}

#endif // moveRequestor_h__
