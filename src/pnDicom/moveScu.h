#ifndef moveScu_h__
#define moveScu_h__

#include "iMoveScu.h"
#include "dicomElement.h"
#include "dicomNode.h"
#include "pnDicom_Export.h"

namespace pacsnode {

class MoveScuPrivate;
class pnDicom_EXPORT MoveScu : public IMoveScu
{
public:
    MoveScu();
    ~MoveScu();

    bool executeMoveRequest(const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes);

private:
    MoveScuPrivate* d;
};
}
#endif // moveScu_h__

