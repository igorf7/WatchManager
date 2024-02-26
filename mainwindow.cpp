#include "mainwindow.h"
#include "watchview.h"
#include "monitorview.h"

#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QTimerEvent>
#include <QDebug>

quint16 MainWindow::deviceAddress;

/**
 * @brief Class constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* mainWidget = new QWidget(this); // Main window container
    QHBoxLayout* mainLayout = new QHBoxLayout; // Main window layout
    setWindowIcon(QIcon(":/images/wireless.png"));

    /* Setup left panel group widgets */
    QWidget* leftPanel = new QGroupBox(mainWidget);
    leftPanel->setFixedWidth(180);
    QLabel *connLabel = new QLabel("<H3>Device connection</H3>");
    connLabel->setStyleSheet("color: rgb(136, 136, 136);"
                                 "background-color: rgb(230, 235, 220)");
    QStringList brList;
    brList << "9600" << "19200" << "57600" << "115200";
    connLabel->setAlignment(Qt::AlignCenter);
    connLabel->setMinimumHeight(24);
    QHBoxLayout *pbrlayout = new QHBoxLayout;
    portComboBox = new QComboBox;
    brComboBox = new QComboBox;
    pbrlayout->addWidget(portComboBox);
    pbrlayout->addWidget(brComboBox);
    brComboBox->addItems(brList);
    brComboBox->setCurrentIndex(3);
    connPushButton = new QPushButton("Connect");
    connPushButton->setAutoDefault(true);
    connect(connPushButton, SIGNAL(clicked()), this, SLOT(onConnPushButtonClicked()));
    QVBoxLayout* leftVLayout = new QVBoxLayout;
    leftVLayout->addWidget(connLabel);
    leftVLayout->addLayout(pbrlayout);
    leftVLayout->addWidget(connPushButton);
    leftVLayout->addStretch();
    leftPanel->setLayout(leftVLayout);

    /* Setup Tab Widget (right panel) */
    mainTabWidget = new QTabWidget(mainWidget);
    QWidget* watchTab = new QWidget(mainTabWidget);   // watch view
    QWidget* monitorTab = new QWidget(mainTabWidget); // monitor view
    QWidget* remoteTab = new QWidget(mainTabWidget);  // remote view
    mainTabWidget->addTab(watchTab, tr("Date / Time"));
    mainTabWidget->addTab(monitorTab, tr("Sensors"));
    mainTabWidget->addTab(remoteTab, "Remote control");

    /* Set mainWidget as central widget */
    mainLayout->addWidget(leftPanel, 0, Qt::AlignLeft);
    mainLayout->addWidget(mainTabWidget);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    /* Adding views to tabs */
    WatchView *watchView = new WatchView(this);
    MonitorView *moniView = new MonitorView(this);
    RemoteView *remoView = new RemoteView(this);
    QVBoxLayout *watchLayout = new QVBoxLayout;
    QVBoxLayout *moniLayout = new QVBoxLayout;
    QVBoxLayout *remoLayout = new QVBoxLayout;
    watchLayout->addWidget(watchView->watchWidget);
    moniLayout->addWidget(moniView->monitorWidget);
    remoLayout->addWidget(remoView->remoteWidget);
    mainTabWidget->widget(CLOCK)->setLayout(watchLayout);
    mainTabWidget->widget(MONITOR)->setLayout(moniLayout);
    mainTabWidget->widget(REMOTE)->setLayout(remoLayout);

    PacketHandler *packHandler = new PacketHandler(this);

    /* Create ComPort thread */
    ComPort *comPort = new ComPort;
    threadPort = new QThread(this);
    threadPort->setObjectName("threadPort");
    comPort->moveToThread(threadPort);
    comPort->serialPort.moveToThread(threadPort);

    /* Connect signals with slots */
    connect(threadPort, &QThread::started, comPort, &ComPort::onPortStart);
    connect(comPort, &ComPort::quitComPort, threadPort, &QThread::deleteLater);
    connect(threadPort, &QThread::finished, comPort, &ComPort::deleteLater);
    connect(comPort, &ComPort::portError, this, &MainWindow::onPortError);
    connect(comPort, &ComPort::portOpened, this, &MainWindow::onPortOpened);
    connect(comPort, &ComPort::portClosed, this, &MainWindow::onPortClosed);
    connect(this, &MainWindow::connectClicked, comPort, &ComPort::onConnectClicked);
    connect(this, &MainWindow::disconnectClicked, comPort, &ComPort::onDisconnectClicked);
    connect(packHandler, &PacketHandler::sendDataPacket, comPort, &ComPort::onSendDataPacket);
    connect(packHandler, &PacketHandler::connectionEstablished,
            this, &MainWindow::onConnectionEstablished);
    connect(packHandler, &PacketHandler::showParams, moniView, &MonitorView::onShowParams);
    connect(packHandler, &PacketHandler::confirmCmd, moniView, &MonitorView::onConfirmCmd);
    connect(moniView, &MonitorView::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    connect(moniView, &MonitorView::writeStatusBar, this, &MainWindow::onWriteStatusBar);
    connect(watchView, &WatchView::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    connect(remoView, &RemoteView::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    connect(this, &MainWindow::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    connect(comPort, &ComPort::parsingPacket, packHandler, &PacketHandler::onParsingPacket);

    connect(brComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onBrChanged(int)));
    connect(mainTabWidget, SIGNAL(currentChanged(int)), moniView, SLOT(onCurrentChanged(int)));
    connect(mainTabWidget, SIGNAL(currentChanged(int)), watchView, SLOT(onCurrentChanged(int)));

    this->updatePortList();
    threadPort->start(); // Start ComPort thread

    /* Activate status bar */
    QFont font;
    font.setBold(true);
    font.setItalic(true);
    statusBar()->setFont(font);
    statusBar()->showMessage("Disconnected");

    deviceAddress = 0x0901; // адрес базы по умолчанию
    qDebug() << "Hello from" << this;
}

/**
 * @brief Class dectructor
 */
MainWindow::~MainWindow()
{
    // Close com-port thread if running
    if( threadPort != nullptr ) threadPort->quit();
    qDebug() << "By-by from" << this;
}

/**
 * @brief MainWindow::updatePortList
 */
void MainWindow::updatePortList()
{
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();

    while (infoList.size() > 0) {
        QSerialPortInfo info = infoList.takeFirst();
        portComboBox->addItem(info.portName());
    }
}

/**
 * @brief MainWindow::onConnPushButtonClicked
 */
void MainWindow::onConnPushButtonClicked()
{
    if (isPortOpened) {
        emit disconnectClicked();
    }
    else {
        emit connectClicked(portComboBox->currentText(),
                            brComboBox->currentText().toInt());
    }
}

/**
 * @brief MainWindow::onBrIndexChanged
 * @param index
 */
void MainWindow::onBrChanged(int index)
{
    Q_UNUSED(index)

    if (isPortOpened) {
        emit disconnectClicked();
    }
}

/**
 * @brief Notifies when a port is opened
 */
void MainWindow::onPortOpened(const QString &port)
{
    Q_UNUSED(port)

    connPushButton->setText(tr("Disconnect"));
    isPortOpened = true;
    statusBar()->showMessage(tr("Connected ") + port);
    portComboBox->setEnabled(false);

    QByteArray payload;
    payload.insert(sizeof(TRFHeader), GET_ADDRESS);
    emit createAndSend(CONNECT, payload);
    connTimeout = this->startTimer(100);
}

/**
 * @brief Notifies when a port is closed
 */
void MainWindow::onPortClosed()
{
    connPushButton->setText(tr("Connect"));
    isPortOpened = false;
    statusBar()->showMessage(tr("Disconnected"));
    portComboBox->setEnabled(true);
}

/**
 * @brief Shows port error message
 * @param msg - error message
 */
void MainWindow::onPortError(const QString &msg)
{
    QMessageBox::warning(this, tr(""), msg);
}

/**
 * @brief MainWindow::onWriteStatusBar
 * @param str
 */
void MainWindow::onWriteStatusBar(const QString &str)
{
    statusBar()->showMessage(str);
}

/**
 * @brief MainWindow::onConnectionEstablished
 * @param id
 */
void MainWindow::onConnectionEstablished(quint16 id)
{
    if (connTimeout != 0) {
        this->killTimer(connTimeout);
    }
    statusBar()->showMessage("Device connected" + portName);
    qDebug() << "Device connected 0x" + QString::number(id, 16).toUpper();
}

/**
 * @brief MainWindow::timerEvent
 * @param event
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == connTimeout) {
        this->killTimer(connTimeout);
        connTimeout = 0;
        emit disconnectClicked();
        QMessageBox::warning(this, tr("Connection"), "Device not found.");
    }
}

/**
 * @brief Shows a message in the status bar
 * @param str - message to show
 * @param timeout - show string timeout

void MainWindow::onWriteStatusBar(const QString &str, int timeout)
{
    statusBar()->showMessage(str, timeout);
} */

/**
  * @brief MainWindow::onAddressEdited
  * @param text

void MainWindow::onAddressEdited(const QString &text)
{
    this->deviceAddress = text.toUShort();
}  */
