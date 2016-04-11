#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "define_packet.h"

#include <QtGui>
#include <QTcpSocket>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    static const int TCP_CONNECTION_TIMEOUT = 1000;

    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QUdpSocket *multicastSocket;
    unsigned char heartbeat;

    bool createTestPacket(TEST_PACKET *packet);

    void debugOut(QString msg);
    void deallocTcp();
    void deallocUdp();
    void deallocMulticast();

private slots:
    void slotCboxClickedTcpConnection(bool clicked);
    void slotBtnClickedTcpSend();
    void slotBtnClickedUdpSend();
    void slotBtnClickedMulticastSend();
    void slotBtnClickedClear();
    void slotBtnClickedScrollToBottom();
    void slotBtnClickedEditProtocol();

    void slotTcpReadyRead();
    void slotUdpReadyRead();
    void slotMulticastReadyRead();

    void slotSetEnabled(bool enable = true);
};

#endif // MAINWINDOW_H
