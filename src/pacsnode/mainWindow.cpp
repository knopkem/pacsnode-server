#include "mainWindow.h"

#include <version_config.h>

// Qt
#include <QtGui>
#include <QtWidgets>

#include "procStarter.h"
#include "nodeTableWidget.h"
#include "dicomNodeConfig.h"

namespace pacsnode {

//--------------------------------------------------------------------------------------

class MainWindowPrivate {
public:

    ProcStarter* pStarter;
    NodeTableWidget* table;
    QTextEdit* loggerWidget;
    QSpinBox* httpPort;
    QSpinBox* dicomPort;
    QPushButton* restartButton;
};

//--------------------------------------------------------------------------------------

MainWindow::MainWindow() : d(new MainWindowPrivate)
{
    QString title = QString(PN_APPLICATION_NAME) + QString(" - ") + QString(PN_VERSION_STRING) + " DICOM Configuration";
    this->setWindowTitle( title );

    int SIZE_X = 800;
    const int SIZE_Y = 600;

    resize(SIZE_X, SIZE_Y);
    this->setMaximumSize(SIZE_X, SIZE_Y);
    this->setMinimumSize(SIZE_X, SIZE_Y);

    QVBoxLayout *vbox = new QVBoxLayout;

    DicomNodeConfig cfg;
    const int hPort = cfg.httpPort();
    const int dPort = cfg.hostNode().port();

    QHBoxLayout* hbox = new QHBoxLayout;
    QLabel* httpPortLabel = new QLabel("HTTP Port:", this);
    d->httpPort = new QSpinBox(this);
    d->httpPort->setMaximumWidth(100);
    d->httpPort->setMaximum(99999);
    d->httpPort->setValue(hPort);
    hbox->addWidget(httpPortLabel, 0, Qt::AlignRight);
    hbox->addWidget(d->httpPort);


    QLabel* dicomPortLabel = new QLabel("DICOM Port:", this);
    d->dicomPort = new QSpinBox(this);
    d->dicomPort->setMaximum(99999);
    d->dicomPort->setMaximumWidth(100);
    d->dicomPort->setValue(dPort);
    hbox->addWidget(dicomPortLabel, 0, Qt::AlignRight);
    hbox->addWidget(d->dicomPort);

    connect(d->httpPort, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChange(int)));
    connect(d->dicomPort, SIGNAL(valueChanged(int)), this, SLOT(spinBoxChange(int)));


    d->restartButton = new QPushButton("Restart to apply", this);
    d->restartButton->setMaximumWidth(100);
    d->restartButton->setEnabled(false);

    connect(d->restartButton, &QPushButton::clicked, this, &MainWindow::restartServer);
    hbox->addWidget(d->restartButton);
    hbox->addStretch(1);

    vbox->addLayout(hbox);

    d->table = new NodeTableWidget(this);
    vbox->addWidget(d->table);

    // start service
    d->pStarter = new ProcStarter("pnServer", QCoreApplication::applicationDirPath() +"/pnServer", this);

    connect(d->pStarter, &ProcStarter::started, this, &MainWindow::onProcessStarted);
    connect(d->pStarter, &ProcStarter::notify, this, &MainWindow::onNotify);

    d->pStarter->start();


    QGroupBox* gbox = new QGroupBox(tr("Logging"), this);
    gbox->setMinimumHeight(300);
    gbox->setMaximumHeight(300);
    d->loggerWidget = new QTextEdit(this);
    QVBoxLayout* vboxIntro = new QVBoxLayout;
    vboxIntro->addWidget(d->loggerWidget);

    gbox->setLayout(vboxIntro);
    vbox->addWidget(gbox);
    QPushButton* browserButton = new QPushButton("Open Viewer", this);
    browserButton->setMaximumWidth(100);

    connect(browserButton, &QPushButton::clicked, this, &MainWindow::openBrowser);

    vbox->addWidget(browserButton);

    this->setLayout(vbox);
}

//--------------------------------------------------------------------------------------

MainWindow::~MainWindow( void )
{
    d->pStarter->stop();
    delete d;
    d= NULL;
}

//--------------------------------------------------------------------------------------

void MainWindow::onProcessStarted()
{
    d->table->read();
}

//--------------------------------------------------------------------------------------

void MainWindow::onNotify(const QString& msg)
{
    d->loggerWidget->append(msg);
}

//--------------------------------------------------------------------------------------

void MainWindow::openBrowser()
{
    DicomNodeConfig cfg;
    const int port = cfg.httpPort();
    QString urlString = QString("http://localhost:%1").arg(port);
    QDesktopServices::openUrl(QUrl(urlString, QUrl::TolerantMode));
}

//--------------------------------------------------------------------------------------

void MainWindow::restartServer()
{
    DicomNodeConfig cfg;
    cfg.clearSettings();
    cfg.setHttpPort(d->httpPort->value());
    DicomNode host = cfg.hostNode();
    host.setPort(d->dicomPort->value());
    cfg.setHostNode(host);
    d->restartButton->setEnabled(false);
    d->pStarter->stop();
    d->pStarter->start();
}

void MainWindow::spinBoxChange(int )
{
    d->restartButton->setEnabled(true);
}

//--------------------------------------------------------------------------------------

}