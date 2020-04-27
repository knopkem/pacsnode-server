#include "moveRequestor.h"

#include <QMutexLocker>
#include <QtConcurrentRun>
#include <QScopedPointer>

#include "dicomElement.h"
#include "dicomNodeConfig.h"
#include "moveScu.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"

namespace pacsnode {


    QString threadedMove(const QString& studyUID, const QString& seriesUID, const QString& imageUID) {
        QScopedPointer<DicomNodeConfig> cfg(new DicomNodeConfig);

        DicomObject mAttr;
        if (!imageUID.isEmpty()) {
            mAttr.append(DicomElement(DCM_QueryRetrieveLevel, "IMAGE"));
            mAttr.append(DicomElement(DCM_StudyInstanceUID, studyUID));
            mAttr.append(DicomElement(DCM_SeriesInstanceUID, seriesUID));
            mAttr.append(DicomElement(DCM_SOPInstanceUID, imageUID));
        }
        else if (!seriesUID.isEmpty()) {
            mAttr.append(DicomElement(DCM_QueryRetrieveLevel, "SERIES"));
            mAttr.append(DicomElement(DCM_StudyInstanceUID, studyUID));
            mAttr.append(DicomElement(DCM_SeriesInstanceUID, seriesUID));
        }

        QScopedPointer<MoveScu> move(new MoveScu);
        if (!move->executeMoveRequest(cfg->hostNode(), cfg->nodes().first(), mAttr)) {
            qWarning() << "MOVE failed, cannot serve data";
        }
        return studyUID+seriesUID+imageUID;
    }


    MoveRequestor* MoveRequestor::m_Instance = NULL;
    QMutex MoveRequestor::m_mutex;

    MoveRequestor* MoveRequestor::instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new MoveRequestor;

            mutex.unlock();
        }

        return m_Instance;
    }

    void MoveRequestor::requestMove(const QString& studyUID, const QString& seriesUID, const QString& imageUID)
    {
        QMutexLocker locker(&m_mutex);

        // clean up finished requests
        foreach(const QString& uid, m_resolved) {
            m_requested.removeAll(uid);
        }
        m_resolved.clear();

        // check if already requested and running
        if (m_requested.contains(studyUID+seriesUID+imageUID)) {
            return;
        }

        // request
        m_requested << studyUID+seriesUID+imageUID;
        QFutureWatcher< QString >* watcher = new QFutureWatcher< QString >();
        QObject::connect(watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
        watcher->setFuture(QtConcurrent::run(threadedMove, studyUID, seriesUID, imageUID));

    }

    MoveRequestor::MoveRequestor()
    {
    }

    MoveRequestor::~MoveRequestor()
    {

    }

    void MoveRequestor::handleFinished()
    {
        QFutureWatcher<QString> * watcher = static_cast< QFutureWatcher<QString>* >(sender());
        if( watcher ){
            QFuture<QString> future = watcher->future();
            m_resolved << future.result();
            delete watcher;
        }
    }

}
