#ifndef multiFinder_h__
#define multiFinder_h__

#include <QList>
#include "dicomNode.h"
#include "dicomElement.h"
#include "pnDicom_Export.h"

namespace pacsnode {

class FinderPrivate;
class SqlDatabase;

//! the multi finder performs multiple find requests to different nodes and returns a merged set
class pnDicom_EXPORT MultiFinder
{
public:
    MultiFinder();
    ~MultiFinder();

    //! threaded but synchronous query on multiple nodes, returns a merged set
    QList< DicomObject > doFind(const QList<DicomNode>& nodes, const DicomObject& searchParams);

private:
    FinderPrivate* d;
};

}
#endif // multiFinder_h__
