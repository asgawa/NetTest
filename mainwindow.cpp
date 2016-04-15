#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customprotocol.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      tcpSocket(NULL),
      udpSocket(NULL),
      multicastSocket(NULL),
      heartbeat(0)
{
    ui->setupUi(this);

    connect(ui->btnTcpSend, SIGNAL(clicked()), this, SLOT(slotBtnClickedTcpSend()));
    connect(ui->btnUdpSend, SIGNAL(clicked()), this, SLOT(slotBtnClickedUdpSend()));
    connect(ui->btnMulticastSend, SIGNAL(clicked()), this, SLOT(slotBtnClickedMulticastSend()));
    connect(ui->cboxTcpConnection, SIGNAL(clicked(bool)), this, SLOT(slotCboxClickedTcpConnection(bool)));
    connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(slotBtnClickedClear()));
    connect(ui->btnScrolltoBottom, SIGNAL(clicked()), this, SLOT(slotBtnClickedScrollToBottom()));
    ui->btnScrolltoBottom->hide();  // btnScrolltoBottom not used
    connect(ui->btnSelectProtocol, SIGNAL(clicked()), this, SLOT(slotBtnClickedEditProtocol()));
}

MainWindow::~MainWindow()
{
    delete ui;
    deallocTcp();
    deallocUdp();
    deallocMulticast();
}

bool MainWindow::createTestPacket(TEST_PACKET *packet)
{
    if (NULL == packet) {
        debugOut("packet is NULL");
        return false;
    }

    const size_t PACKET_LEN = sizeof(TEST_PACKET);

    memset(packet, 0, PACKET_LEN);
    packet->PacketLength = PACKET_LEN;
    packet->Command = 0x1;
    packet->DataValid = 0x1;
    packet->Watchdog = this->heartbeat;
    ++heartbeat;

    if (0xFF == heartbeat) heartbeat = 0;

    return true;
}

void MainWindow::slotCboxClickedTcpConnection(bool clicked)
{
    if (NULL == ui || NULL == ui->lblTcpIp || NULL == ui->lblTcpPort) {
        debugOut("ui | lblTcpIp | lblTcpPort is NULL");
        return;
    }

    ui->lblTcpIp->setEnabled(!clicked);
    ui->lblTcpPort->setEnabled(!clicked);

    if (!clicked) {
        if (tcpSocket) {
            if (QTcpSocket::ConnectedState == tcpSocket->state()) {
                debugOut("TCP disconnect");
                deallocTcp();
            }
        }
    }
}

void MainWindow::slotBtnClickedTcpSend()
{
    if (NULL == ui || NULL == ui->lblTcpIp || NULL == ui->lblTcpPort || NULL == ui->cboxTcpConnection) {
        debugOut("ui | lblTcpIp | lblTcpPort | cboxTcpConnection is NULL");
        return;
    }

    const bool preserveConnection = ui->cboxTcpConnection->isChecked();

    QString ip = ui->lblTcpIp->text();
    int port = ui->lblTcpPort->text().toInt();
    if (NULL == tcpSocket) {
        debugOut("TCP: create new one & connect to host");
        tcpSocket = new QTcpSocket;
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotTcpReadyRead()));
        tcpSocket->connectToHost(ip, port);
    } else {
        if (!preserveConnection) {
            if (QTcpSocket::ConnectedState != tcpSocket->state()) {
                debugOut("TCP: connect to host");
                tcpSocket->connectToHost(ip, port);
            }
        }
    }

    if (tcpSocket->waitForConnected(TCP_CONNECTION_TIMEOUT)) {
        TEST_PACKET *packet = new TEST_PACKET;
        if (createTestPacket(packet)) {
            int result = tcpSocket->write((const char *)packet, sizeof(TEST_PACKET));
            debugOut(QString("[%1:%2] result = %3").arg(__func__).arg(__LINE__).arg(result));

            delete packet;

            if (!preserveConnection) {
                debugOut("TCP: disconnect & delete TCP");
                deallocTcp();
            }
        }
    } else {
        debugOut(QString("TCP: cannot connect to [%1], delete TCP").arg(ip));
        deallocTcp();
    }
}

void MainWindow::slotBtnClickedUdpSend()
{
    if (NULL == ui || NULL == ui->lblUdpIp || NULL == ui->lblUdpPort) {
        debugOut("ui | lblUdpIp | lblUdpPort is NULL");
        return;
    }

    QString ip = ui->lblUdpIp->text();
    int port = ui->lblUdpPort->text().toInt();
    if (NULL == udpSocket) {
        udpSocket = new QUdpSocket;
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(slotUdpReadyRead()));
        udpSocket->bind(QHostAddress::Any, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
    }

    if (QUdpSocket::BoundState == udpSocket->state()) {
        TEST_PACKET *packet = new TEST_PACKET;
        if (createTestPacket(packet)) {
            int result = udpSocket->writeDatagram((const char *)packet, sizeof(TEST_PACKET), QHostAddress(ip), port);
            debugOut(QString("[%1:%2] result = %3").arg(__func__).arg(__LINE__).arg(result));
            delete packet;
        }
    } else {
        debugOut(QString("UDP: cannot bind to [%1]").arg(ip));
        deallocUdp();
    }
}

void MainWindow::slotBtnClickedMulticastSend()
{
    if (NULL == ui || NULL == ui->lblMulticastIp || NULL == ui->lblMulticastPort) {
        debugOut("ui | lblMulticastIp | lblMulticastPort is NULL");
        return;
    }

    QString ip = ui->lblMulticastIp->text();
    int port = ui->lblMulticastPort->text().toInt();

    if (NULL == multicastSocket) {
        multicastSocket = new QUdpSocket;
        multicastSocket->setSocketOption(QUdpSocket::MulticastLoopbackOption, QVariant(0));
        multicastSocket->bind(port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        connect(multicastSocket, SIGNAL(readyRead()), this, SLOT(slotMulticastReadyRead()));
        if (QUdpSocket::BoundState != multicastSocket->state()) {
            debugOut(QString("Multicast: cannot bind to [%1]").arg(ip));
            deallocMulticast();
            return;
        }

        multicastSocket->joinMulticastGroup(QHostAddress(ip));
    }

    TEST_PACKET *packet = new TEST_PACKET;
    if (createTestPacket(packet)) {
        int result = multicastSocket->writeDatagram((const char *)packet, sizeof(TEST_PACKET), QHostAddress(ip), port);
        debugOut(QString("[%1:%2] result = %3").arg(__func__).arg(__LINE__).arg(result));
        delete packet;
    }
}

void MainWindow::slotBtnClickedClear()
{
    if (NULL == ui || NULL == ui->tboxOutput) {
        debugOut("ui | tboxOutput is NULL");
        return;
    }

    ui->tboxOutput->clear();
}

void MainWindow::slotBtnClickedScrollToBottom()
{
    if (NULL == ui || NULL == ui->tboxOutput) {
        debugOut("ui | tboxOutput is NULL");
        return;
    }

    QTextCursor cursor(ui->tboxOutput->document());
    cursor.movePosition(QTextCursor::End);
    ui->tboxOutput->setTextCursor(cursor);
}

void MainWindow::slotBtnClickedEditProtocol()
{
    CustomProtocol *customProtocol = new CustomProtocol;
    connect(customProtocol, SIGNAL(finished(int)), customProtocol, SLOT(deleteLater()));
    connect(customProtocol, SIGNAL(finished(int)), this, SLOT(slotSetEnabled()));
    setEnabled(false);
    customProtocol->show();
}

void MainWindow::slotTcpReadyRead()
{
    debugOut(QString("[%1:%2]").arg(__func__).arg(__LINE__));
    if (tcpSocket) {
        QByteArray data = tcpSocket->readAll();
        qDebug() << "result =" << data.size();
    }
}

void MainWindow::slotUdpReadyRead()
{
    debugOut(QString("[%1:%2]").arg(__func__).arg(__LINE__));
    if (udpSocket) {
        QByteArray data(udpSocket->pendingDatagramSize(), 0);
        const qint64 result = udpSocket->readDatagram(data.data(), data.size());
        qDebug() << "result =" << result;
    }
}

void MainWindow::slotMulticastReadyRead()
{
    debugOut(QString("[%1:%2]").arg(__func__).arg(__LINE__));
    if (multicastSocket) {
        QByteArray data(multicastSocket->pendingDatagramSize(), 0);
        const qint64 result = multicastSocket->readDatagram(data.data(), data.size());
        qDebug() << "result =" << result;
    }
}

void MainWindow::slotSetEnabled(bool enabled)
{
    QMainWindow::setEnabled(enabled);
}

void MainWindow::debugOut(QString msg)
{
    if (NULL == ui || NULL == ui->tboxOutput) {
        qDebug("ui | tboxOutput is NULL");
        return;
    }

    //append() let scrollbar move to the last line
    ui->tboxOutput->append(msg);
}

void MainWindow::deallocTcp()
{
    if (tcpSocket) {
        delete tcpSocket;
        tcpSocket = NULL;
    }
}

void MainWindow::deallocUdp()
{
    if (udpSocket) {
        delete udpSocket;
        udpSocket = NULL;
    }
}

void MainWindow::deallocMulticast()
{
    if (multicastSocket) {
        delete multicastSocket;
        multicastSocket = NULL;
    }
}
