#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
class Client : public QObject
{
  Q_OBJECT
public:
  explicit Client(QObject* parent);
  ~Client();
  void connectToServer(const QString& host, int port);

private slots:
  void tryReconnect();
  void connected();
  void errorOccurred(QAbstractSocket::SocketError socketError);
  void disconnected();
  void readyRead();

private:
  QDataStream in;
  QTcpSocket* m_socket = nullptr;
  QString m_currentMessage;
  QTimer* m_timer = nullptr;
  QString serverHost = "";
  int serverPort = 0;
};

#endif // CLIENT_H
