#ifndef iMoveScu_h__
#define iMoveScu_h__

#include "dicomElement.h"
#include "dicomNode.h"

namespace pacsnode {

    class IMoveScu
    {
    public:
        virtual ~IMoveScu() {}

        virtual bool executeMoveRequest(const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes) = 0;

    };

}
#endif // iMoveScu_h__

