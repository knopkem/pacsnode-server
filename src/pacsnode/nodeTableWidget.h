#ifndef nodeTableWidget_h__
#define nodeTableWidget_h__

#include <QWidget>
#include <QTableWidgetItem>

#include "dicomNode.h"

namespace pacsnode {

/**
 * Item used in NodeTableWidget
 */
class NodeListWidgetItem : public QTableWidgetItem
{
public:
    NodeListWidgetItem(int type = Type) : QTableWidgetItem(type) {};
    virtual ~NodeListWidgetItem(void) {};

    enum nodeRole {
        PACS_AETITLE = Qt::UserRole + 1,
        PACS_ADDRESS,
        PACS_PORT,
        PACS_ENABLED,
        PACS_STATUS,
        PACS_FETCH
    };
};

class NodeTableWidgetPrivate;

/**
 * A widget that shows a table with DICOM node information
 */
class NodeTableWidget : public QWidget
{
    Q_OBJECT
public:
    NodeTableWidget(QWidget* parent = NULL);
    virtual ~NodeTableWidget();

    enum ItemColumns {
        TitleColumn,
        AdressColumn,
        PortColumn,
        ButtonColumn
    };

Q_SIGNALS:
    void echoRequest(QString aet, QString address, QString port);

public Q_SLOTS:
    void read( void );
    void write( void );
    void clear( void );

protected:
    QList<DicomNode> readList();
    void writeList(QList<DicomNode> nodes);

    void fillWidget (const QList<DicomNode> &nodes);
    
protected Q_SLOTS:
    void onAdd( void );
    void onDelete( void );
    void onCancel( void );
    void onSave( void );
  
    /** Setup node editor with node identified by its id. */
    void setupNodeEditor (int id); 

private:
    NodeTableWidgetPrivate* d;
};

}
#endif // nodeTableWidget_h__
