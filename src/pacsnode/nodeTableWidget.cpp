#include "nodeTableWidget.h"

#include <QtCore>
#include <QtWidgets>

#include "nodeEditWidget.h"

// core
#include "sqlDatabase.h"
#include "storage.h"
#include "dicomNodeConfig.h"

namespace pacsnode {

class NodeTableWidgetPrivate {
public:
    QTableWidget* nodesTable;
    QSignalMapper* editMapper;
    NodeEditWidget* nodeEditor;
    int currentNode;
};

//--------------------------------------------------------------------------------------------

NodeTableWidget::NodeTableWidget(QWidget* parent /*= NULL*/ ) :
    QWidget(parent), d(new NodeTableWidgetPrivate)
{
    // Distant PACS Settings widgets
    d->currentNode = -1;

    d->editMapper  = new QSignalMapper(this);

    d->nodesTable  = new QTableWidget (this);
    d->nodesTable->setColumnCount(4);

    d->nodesTable->setColumnWidth(TitleColumn,  275);
    d->nodesTable->setColumnWidth(AdressColumn, 275);
    d->nodesTable->setColumnWidth(PortColumn,   100);
    d->nodesTable->setColumnWidth(ButtonColumn, 100);
    d->nodesTable->setShowGrid(false);
    d->nodesTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->nodesTable->setSelectionMode (QAbstractItemView::NoSelection);
    d->nodesTable->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    d->nodesTable->setEditTriggers(QAbstractItemView::EditTriggers(0));
    d->nodesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    d->nodesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    d->nodesTable->verticalHeader()->setVisible(false);
    d->nodesTable->setFixedWidth(760);

    d->nodeEditor  = new NodeEditWidget(this);
    d->nodeEditor->setFixedWidth(760);
    d->nodeEditor->hide();

    QHBoxLayout * nodesLayout = new QHBoxLayout;
    nodesLayout->addWidget(d->nodesTable);
    nodesLayout->addStretch();
    nodesLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout * editorLayout = new QHBoxLayout;
    editorLayout->addWidget(d->nodeEditor);
    editorLayout->addStretch();
    editorLayout->setContentsMargins(0, 0, 0, 0);

    // Global layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(nodesLayout);
    layout->addLayout(editorLayout);
    layout->addStretch();

    this->setLayout(layout);

    connect (d->editMapper, SIGNAL(mapped(int)), this, SLOT(setupNodeEditor(int)));
    connect (d->editMapper, SIGNAL(mapped(int)), d->nodeEditor, SLOT(show()));

    connect (d->nodeEditor, SIGNAL(deleteNode()), this, SLOT(onDelete()));
    connect (d->nodeEditor, SIGNAL(cancel()),     this, SLOT(onCancel()));
    connect (d->nodeEditor, SIGNAL(save()),       this, SLOT(onSave()));
}

//--------------------------------------------------------------------------------------------

NodeTableWidget::~NodeTableWidget()
{
    delete d;
}

//--------------------------------------------------------------------------------------------

QList<DicomNode> NodeTableWidget::readList()
{
    DicomNodeConfig cfg;
    return cfg.nodes();

}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::writeList( QList<DicomNode> nodes)
{
    DicomNodeConfig cfg;
    cfg.clear();
    foreach(const DicomNode& node, nodes) {
        cfg.addNode(node);
    }
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::read( void )
{
    fillWidget( readList() );
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::write( void )
{
    this->onSave();
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::clear( void )
{
    d->nodesTable->clear();
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::fillWidget( const QList<DicomNode> &nodes )
{
    d->nodesTable->clear();

    QStringList headers;
    headers << "AET";
    headers << "Address";
    headers << "Port";
    headers << "Edit";

    d->nodesTable->setHorizontalHeaderLabels(headers);

    QList<NodeListWidgetItem*> itemList;

    foreach(const DicomNode& node, nodes) {
        NodeListWidgetItem* item = new NodeListWidgetItem;
        item->setData (NodeListWidgetItem::PACS_AETITLE, node.title() );
        item->setData (NodeListWidgetItem::PACS_ADDRESS, node.address() );
        item->setData (NodeListWidgetItem::PACS_PORT,    node.port() );
        item->setData (Qt::DisplayRole,                  node.title() );
        itemList << item;
    }

    int index = 0;
    d->nodesTable->setRowCount (itemList.count() + 1);

    foreach(NodeListWidgetItem *item, itemList) {
        // Add item itself to item line table
        d->nodesTable->setItem (index, 0, item);

        d->nodesTable->setCellWidget(index, AdressColumn, new QLabel(item->data(NodeListWidgetItem::PACS_ADDRESS).toString(), this));
        d->nodesTable->setCellWidget(index, PortColumn,   new QLabel(item->data(NodeListWidgetItem::PACS_PORT).toString(),    this));

        // Add button to the item line table
        QPushButton *button = new QPushButton (this);
        button->setText(tr("Edit"));
        button->setFocusPolicy(Qt::NoFocus);
        button->setToolTip(tr("Edit"));
        connect (button, SIGNAL(clicked()), d->editMapper, SLOT(map()));
        d->nodesTable->setCellWidget(index, ButtonColumn, button);
        d->editMapper->setMapping(button, index);

        index++;
    }

    // create a dummy node
    NodeListWidgetItem* dummyItem = new NodeListWidgetItem;
    dummyItem->setData (NodeListWidgetItem::PACS_AETITLE, "");
    dummyItem->setData (NodeListWidgetItem::PACS_ADDRESS, "");
    dummyItem->setData (NodeListWidgetItem::PACS_PORT,    "");

    d->nodesTable->setItem (index, 0, dummyItem);

    QPushButton *button = new QPushButton (this);
    button->setText(tr("Add"));
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(tr("Add"));
    connect (button, SIGNAL(clicked()), this, SLOT(onAdd()));

    d->nodesTable->setCellWidget(index, ButtonColumn, button);
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::setupNodeEditor( int id )
{
    if (QTableWidgetItem *item = d->nodesTable->item(id, 0)) {

        if ( id < d->nodesTable->rowCount() - 1 ) // skip last item (dummy item)
            d->currentNode = id;
        else
            d->currentNode = -1;

        d->nodeEditor->setAETitle(item->data(NodeListWidgetItem::PACS_AETITLE).toString());
        d->nodeEditor->setAddress(item->data(NodeListWidgetItem::PACS_ADDRESS).toString());
        d->nodeEditor->setPort(item->data(NodeListWidgetItem::PACS_PORT).toString());
    }
    else {
        qWarning() << "mapping error";
    }
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::onAdd( void )
{
    d->currentNode = -1;
    d->nodeEditor->show();
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::onDelete( void )
{
    d->nodeEditor->hide();

    QList<DicomNode> nodes =  readList();
    nodes.removeAt(d->currentNode);
    d->currentNode = -1;
    writeList(nodes);

    read();
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::onCancel( void )
{
    d->nodeEditor->hide();
    d->currentNode = -1;
}

//--------------------------------------------------------------------------------------------

void NodeTableWidget::onSave( void )
{
    QString aetitle     = d->nodeEditor->aeTitle();
    QString address     = d->nodeEditor->address();
    QString port        = d->nodeEditor->port();

    QList<DicomNode> nodes =  readList();

    DicomNode newNode;
    newNode.setTitle(aetitle);
    newNode.setAddress(address);
    newNode.setPortAsString(port);
    newNode.setFetchable(true);

    if (d->currentNode >= 0 && d->currentNode < nodes.count()) {
        nodes[d->currentNode] = newNode;
    }
    else { // new node
        nodes.append(newNode);
    }

    writeList(nodes);
    read();

    d->currentNode = -1;
    d->nodeEditor->hide();

}

//--------------------------------------------------------------------------------------------


}
