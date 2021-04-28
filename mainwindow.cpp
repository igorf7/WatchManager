#include "mainwindow.h"

#include "watchviewer.h"
#include "monitorviewer.h"

#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QTimerEvent>
#include <QFile>
#include <QDebug>

quint16 MainWindow::deviceAddress;
bool MainWindow::beeperStatus = false;

/**
 * @brief Class constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* mainWidget = new QWidget(this); // Контейнер главного окна приложения
    QHBoxLayout* mainLayout = new QHBoxLayout; // Компоновщик главного окна

    setWindowIcon(QIcon(":/images/wireless.png"));

    /* Setup left panel group widgets */
    QWidget* leftPanel = new QGroupBox(mainWidget);
    leftPanel->setFixedWidth(200);
    QLabel *connLabel = new QLabel("<H3>Соединение с пультом</H3>");
    connLabel->setStyleSheet("color: rgb(136, 136, 136);"
                                 "background-color: rgb(230, 235, 220)");
    connLabel->setAlignment(Qt::AlignCenter);
    connPushButton = new QPushButton("Подключиться");
    connPushButton->setAutoDefault(true);
    connect(connPushButton, SIGNAL(clicked()), this, SLOT(onConnPushButton_clicked()));
    QVBoxLayout* leftVLayout = new QVBoxLayout;
    leftVLayout->addWidget(connLabel);
    leftVLayout->addWidget(connPushButton);
    leftVLayout->addStretch();
    leftPanel->setLayout(leftVLayout);

    /* Setup Tab Widget (right panel) */
    mainTabWidget = new QTabWidget(mainWidget);
    QWidget* watchTab = new QWidget(mainTabWidget);   // watch viewer
    QWidget* monitorTab = new QWidget(mainTabWidget); // monitor viewer
    QWidget* remoteTab = new QWidget(mainTabWidget);  // remote viewer
    mainTabWidget->addTab(watchTab, "Дата / Время");
    mainTabWidget->addTab(monitorTab, "Мониторинг датчиков");
    mainTabWidget->addTab(remoteTab, "Пульт");

    /* Set mainWidget as central widget */
    mainLayout->addWidget(leftPanel, 0, Qt::AlignLeft);
    mainLayout->addWidget(mainTabWidget);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    /* Activate status bar */
    QFont font;
    font.setBold(true);
    font.setItalic(true);
    statusBar()->setFont(font);

    /* Create menu "Файл" */
    QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));
    QAction* quitAct = fileMenu->addAction("&Выход");
    connect(quitAct, SIGNAL(triggered()), this, SLOT(onQuit_triggered()));

    /* Create menu "Справка" */
    QMenu* helpMenu = menuBar()->addMenu(tr("&Справка"));
    QAction* manualAct = helpMenu->addAction("&Инструкция");
    connect(manualAct, SIGNAL(triggered()), this, SLOT(onManual_triggered()));
    QAction* aboutAct = helpMenu->addAction("&О программе");
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(onAbout_triggered()));
    QAction* beepAct = helpMenu->addAction("&Beeper");
    beepAct->setCheckable(true);
    beepAct->setChecked(false);
    connect(beepAct, SIGNAL(triggered(bool)), this, SLOT(onBeep_triggered(bool)));

    /* Add viewers into the Tabs */
    WatchViewer *watchViewer = new WatchViewer(this);
    MonitorViewer *moniViewer = new MonitorViewer(this);
    RemoteViewer *remoViewer = new RemoteViewer(this);
    QVBoxLayout *watchLayout = new QVBoxLayout;
    QVBoxLayout *moniLayout = new QVBoxLayout;
    QVBoxLayout *remoLayout = new QVBoxLayout;
    watchLayout->addWidget(watchViewer->watchWidget);
    moniLayout->addWidget(moniViewer->monitorWidget);
    remoLayout->addWidget(remoViewer->remoteWidget);
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
    QObject::connect(threadPort, &QThread::started, comPort, &ComPort::onPortStart);
    QObject::connect(comPort, &ComPort::quitComPort, threadPort, &QThread::deleteLater);
    QObject::connect(threadPort, &QThread::finished, comPort, &ComPort::deleteLater);
    QObject::connect(comPort, &ComPort::portOpenError, this, &MainWindow::onPortOpenError);
    QObject::connect(comPort, &ComPort::portSendError, this, &MainWindow::onPortSendError);
    QObject::connect(comPort, &ComPort::portOpened, this, &MainWindow::onPortOpened);
    QObject::connect(comPort, &ComPort::portClosed, this, &MainWindow::onPortClosed);
    QObject::connect(this, &MainWindow::connectClicked, comPort, &ComPort::onConnectClicked);
    QObject::connect(this, &MainWindow::disconnectClicked, comPort, &ComPort::onDisconnectClicked);
    QObject::connect(packHandler, &PacketHandler::sendDataPacket, comPort, &ComPort::onSendDataPacket);
    QObject::connect(packHandler, &PacketHandler::connectionEstablished, this, &MainWindow::onConnectionEstablished);
    QObject::connect(packHandler, &PacketHandler::showParams, moniViewer, &MonitorViewer::onShowParams);
    QObject::connect(packHandler, &PacketHandler::confirmCmd, moniViewer, &MonitorViewer::onConfirmCmd);
    QObject::connect(moniViewer, &MonitorViewer::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    QObject::connect(watchViewer, &WatchViewer::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    QObject::connect(remoViewer, &RemoteViewer::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    QObject::connect(this, &MainWindow::createAndSend, packHandler, &PacketHandler::onCreateAndSend);
    QObject::connect(comPort, &ComPort::parsingPacket, packHandler, &PacketHandler::onParsingPacket);

    connect(mainTabWidget, SIGNAL(currentChanged(int)), moniViewer, SLOT(onCurrentChanged(int)));
    connect(mainTabWidget, SIGNAL(currentChanged(int)), watchViewer, SLOT(onCurrentChanged(int)));

    threadPort->start(); // Start ComPort thread

    portNumber = 0;
    connTimeout = 0;
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
 * @brief Updates the list of available ports
 */
void MainWindow::updatePortList()
{
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();

    if (!infoList.size()) {
        qDebug() << "Нет подключенных устройств";
        return;
    }
    while (infoList.size() > 0) {
        QSerialPortInfo info = infoList.takeFirst();
        portList << info.portName();
    }    
}

/**
 * @brief Slot of the Connect button
 */
void MainWindow::onConnPushButton_clicked()
{
    if (isPortOpened) {
        emit disconnectClicked();
    }
    else {
        isConnected = false;
        portNumber = 0;
        portList.clear();
        this->updatePortList();
        connTimeout = this->startTimer(10);
    }
}

/**
 * @brief Notifies when a port is opened
 */
void MainWindow::onPortOpened(const QString &port)
{
    connPushButton->setText("Отключиться");
    isPortOpened = true;

    // Send cmd get_id
    portName = port;
    QByteArray payload;
    payload.insert(sizeof(RF_Header_t), GET_ADDRESS);
    emit createAndSend(CONNECT, payload);
    connTimeout = this->startTimer(100);
}

/**
 * @brief Notifies about the closure of the port
 */
void MainWindow::onPortClosed()
{
    connPushButton->setText("Подключиться");
    isPortOpened = false;
    if (isConnected) {
        isConnected = false;
        statusBar()->showMessage("Соединение разорвано");
        qDebug() << "Device disconnected";
    }
}

/**
 * @brief Shows port error message
 * @param msg - error message
 */
void MainWindow::onPortOpenError(const QString &msg)
{
    Q_UNUSED(msg)

    if (portNumber < portList.size()) {
        emit connectClicked(portList.at(portNumber++));
    }
}

/**
 * @brief MainWindow::onPortSendError
 * @param msg - error message
 */
void MainWindow::onPortSendError(const QString &msg)
{
    QMessageBox::warning(this, tr("Соединение"), msg);
    mainTabWidget->setCurrentIndex(0);
}

/**
 * @brief MainWindow::onConnectionEstablished
 */
void MainWindow::onConnectionEstablished(quint16 id)
{
    if (connTimeout != 0) {
        this->killTimer(connTimeout);
    }
    portNumber = 0;
    isConnected = true;
    statusBar()->showMessage("Соединение установлено " + portName);
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
        if (isPortOpened)
            emit disconnectClicked();
        if (portNumber < portList.size()) {
            emit connectClicked(portList.at(portNumber++));
        }
        else {
            statusBar()->showMessage("Пульт не найден");
        }
    }
}

/**
 * @brief Shows a message in the status bar
 * @param str - message to show
 * @param timeout - show string timeout
 */
void MainWindow::onWriteStatusBar(const QString &str, int timeout)
{
    statusBar()->showMessage(str, timeout);
}

/**
 * @brief Menu File->Quit
 */
void MainWindow::onQuit_triggered()
{
    close();
}

/**
 * @brief Menu Help->Manual
 */
void MainWindow::onManual_triggered()
{
    QDialog* oldWindow = this->findChild<QDialog*>("manualWindow");
    if (oldWindow != nullptr) {
        delete oldWindow;
    }

    QFile file(":text/manual.txt");
    file.open(QIODevice::ReadOnly);
    QDialog* manualWindow = new QDialog(this);
    manualWindow->setObjectName("manualWindow");
    manualWindow->setWindowTitle("Инструкция");
    manualWindow->resize(700, 500);
    manualWindow->setWindowFlags(Qt::Drawer);
    manualWindow->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* manualLayot = new QVBoxLayout;
    QTextEdit* textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    manualLayot->addWidget(textEdit);
    manualWindow->setLayout(manualLayot);
    QTextStream in(&file);
    QString line = in.readAll();
    textEdit->setPlainText(line);
    file.close();

    manualWindow->show();
}

/**
 * @brief Menu Help->About
 */
void MainWindow::onAbout_triggered()
{
    QDialog* aboutWindow = new QDialog(this);
    aboutWindow->setWindowTitle("О программе");
    aboutWindow->resize(370, 252);
    aboutWindow->setModal(true);
    aboutWindow->setWindowFlags(Qt::Drawer);
    QLabel* logoLabel = new QLabel;
    QPixmap logoPixmap(":/images/wireless.png");
    logoLabel->setPixmap(logoPixmap);
    aboutWindow->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* textLabel = new QLabel;
    QVBoxLayout* aboutLayot = new QVBoxLayout;
    textLabel->setText(tr("<h2>WatchManager</h2>"
                          "<h4>Версия 1.0.1</h4>"
                          "<p>Программа для работы с устройством «Метеочасы»</p>"
                          "<p>Copyright &copy; 2020, I.Filippov</p>"));

    aboutLayot->addWidget(logoLabel, 0, Qt::AlignCenter);
    aboutLayot->addWidget(textLabel, 0, Qt::AlignCenter);
    aboutWindow->setLayout(aboutLayot);

    aboutWindow->show();
}

/**
 * @brief MainWindow::onBeep_triggered
 * @param state
 */
void MainWindow::onBeep_triggered(bool state)
{
    beeperStatus = state;
}

/**
  * @brief MainWindow::onAddressEdited
  * @param text
  */
void MainWindow::onAddressEdited(const QString &text)
{
    this->deviceAddress = text.toUShort();
}
