#include "nodeEditWidget.h"

// Qt
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QFormLayout>
#include <QValidator>
#include <QCheckBox>

namespace pacsnode {

class NodeEditWidgetPrivate {
public:
    QLineEdit *aetitle;
    QLineEdit *address;
    QLineEdit *port;
    QCheckBox *serverBox;

    QPushButton *cancelButton;
    QPushButton *saveButton;
    QPushButton *deleteButton;

};

NodeEditWidget::NodeEditWidget( QWidget* parent/*= NULL*/ ) :
    QWidget(parent), d(new NodeEditWidgetPrivate)
{
    QValidator* validator = new QIntValidator( 0, 65535, this );

    d->aetitle = new QLineEdit("CONQUESTSRV1", this);
    d->address = new QLineEdit("localhost", this);
    d->port = new QLineEdit("5678", this);
    d->port->setValidator(validator);

    d->serverBox = new QCheckBox();
    d->serverBox->setChecked(true);

    d->cancelButton = new QPushButton (this);
    d->cancelButton->setText (tr("Cancel"));
    d->cancelButton->setFocusPolicy(Qt::NoFocus);
    d->cancelButton->setToolTip(tr("Cancel"));

    d->saveButton = new QPushButton (this);
    d->saveButton->setText (tr("Save"));
    d->saveButton->setFocusPolicy(Qt::NoFocus);
    d->saveButton->setToolTip(tr("Save"));

    d->deleteButton = new QPushButton (this);
    d->deleteButton->setText (tr("Delete"));
    d->deleteButton->setFocusPolicy(Qt::NoFocus);
    d->deleteButton->setToolTip(tr("Delete"));

    QHBoxLayout *buttons_layout = new QHBoxLayout;
    buttons_layout->setSpacing(0);
    buttons_layout->addWidget(d->deleteButton);
    buttons_layout->addWidget(d->cancelButton);
    buttons_layout->addWidget(d->saveButton);
    buttons_layout->addStretch();

    connect (d->deleteButton, SIGNAL(clicked()),
             this, SIGNAL(deleteNode()));

    connect (d->cancelButton, SIGNAL(clicked()),
             this, SIGNAL(cancel()));

    connect (d->saveButton, SIGNAL(clicked()),
             this, SIGNAL(save()));

    QFormLayout *form = new QFormLayout;
    form->addRow("AE Title", d->aetitle);
    form->addRow("Address",  d->address);
    form->addRow("Port",     d->port);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addLayout(buttons_layout);
}

NodeEditWidget::~NodeEditWidget()
{
    delete d;
}

void NodeEditWidget::setAETitle(const QString &value)
{
    d->aetitle->setText(value);
}

QString NodeEditWidget::aeTitle() const
{
    return d->aetitle->text();
}

void NodeEditWidget::setAddress(const QString &value)
{
    d->address->setText(value);
}

QString NodeEditWidget::address() const
{
    return d->address->text();
}

void NodeEditWidget::setPort(const QString &value)
{
    d->port->setText(value);
}

QString NodeEditWidget::port() const
{
    return d->port->text();
}


}

