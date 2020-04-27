#include "nodeWidget.h"

// Qt
#include <QUuid>
#include <QLabel>

namespace pacsnode {

NodeWidget::NodeWidget( QString aet, QString address, QString port, bool showButtons, QWidget *parent /*= NULL*/ ) : QWidget(parent)
{
    _uid = QUuid::createUuid().toString();
    _aetLE.setText(aet);
    _aetLE.setPlaceholderText(tr("AE-title"));
    _addLE.setText(address);
    _addLE.setPlaceholderText(tr("address or ip"));
    _addLE.setReadOnly(true);
    _portLE.setText(port);
    _portLE.setPlaceholderText(tr("port"));
    _removeButton.setText("remove");
    _echoButton.setText("echo");
    QLabel* label = new QLabel("DICOM server:");
    _hbox.addWidget(label);
    _hbox.addWidget(&_aetLE);
    _hbox.addWidget(&_addLE);
    _hbox.addWidget(&_portLE);
    if (showButtons) {
        _hbox.addWidget(&_echoButton);
        _hbox.addWidget(&_removeButton);
    }
    //_hbox.addStretch();
    setLayout(&_hbox);

    connect(&_removeButton, SIGNAL(clicked()), this, SLOT(onRemove()) );
    connect(&_echoButton, SIGNAL(clicked()), this, SLOT(onEcho()) );
    connect(&_aetLE, SIGNAL(editingFinished()), this, SLOT(commitRequest()) );
    connect(&_addLE, SIGNAL(editingFinished()), this, SLOT(commitRequest()) );
    connect(&_portLE, SIGNAL(editingFinished()), this, SLOT(commitRequest()) );

}

//--------------------------------------------------------------------------------------------

void NodeWidget::onRemove()
{
    Q_EMIT remove(_uid);
}

//--------------------------------------------------------------------------------------------

QString NodeWidget::uid()
{
    return _uid;
}

//--------------------------------------------------------------------------------------------

void NodeWidget::onEcho()
{
    Q_EMIT echo(_aetLE.text(), _addLE.text(), _portLE.text(), _uid);
}

//--------------------------------------------------------------------------------------------

void NodeWidget::commitRequest()
{
    if ( _aetLE.text().isEmpty() || _addLE.text().isEmpty() || _portLE.text().isEmpty() )
        return;

    Q_EMIT updateRequest();
}

//--------------------------------------------------------------------------------------------

}

