#ifndef findScu_h__
#define findScu_h__

#include <QList>

#include "iFindScu.h"
#include "pnDicom_Export.h"

namespace pacsnode {


class pnDicom_EXPORT FindScu : public IFindScu
{
public:
    FindScu();
    virtual ~FindScu();

    QList< DicomObject > executeFindRequest(const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes);

private:
    DicomObject m_attributes;
};

}
#endif // findScu_h__

