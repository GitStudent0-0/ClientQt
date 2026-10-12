#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>

class Client : public QObject
{
  Q_OBJECT
public:
  explicit Client(QObject* parent);
  ~Client();
  void connectToServer(const QString& host, int port);

protected:
  void timerEvent(QTimerEvent* event) override;
private slots:
  void connected();
  void errorOccurred(QAbstractSocket::SocketError socketError);
  void disconnected();
  void readyRead();

private:
  QDataStream in;
  QTcpSocket* m_socket = nullptr;
  QString m_currentMessage;
  QString m_serverHost = "";
  int m_serverPort = 0;
};

#endif // CLIENT_H
