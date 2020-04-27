#ifndef TableCreator_h__
#define TableCreator_h__

#include <QtCore>

#include "tableElement.h"

#include "pnDicom_Export.h"

namespace pacsnode{

class TableCreatorPrivate;

class pnDicom_EXPORT TableCreator
{
public:
    TableCreator(const QList<TableElement>& findList);
    ~TableCreator();

    void init();

protected:

    bool createPatientTable();

    bool createStudyTable();

    bool createSeriesTable();

    bool createImageTable();

    bool createNodesTable();

    bool createSettingTable();

private:
    TableCreator();
    TableCreator(const TableCreator&);
    TableCreatorPrivate* d;
};

}
#endif // TableCreator_h__

