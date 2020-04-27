#ifndef iFindScu_h__
#define iFindScu_h__

#include "dicomElement.h"
#include "dicomNode.h"

namespace pacsnode {

class IFindScu
{
public:
    virtual ~IFindScu() {}

    virtual QList< DicomObject > executeFindRequest(const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes) = 0;

};

}
#endif // iFindScu_h__
