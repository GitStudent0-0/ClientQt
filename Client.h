#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
class Client : public QObject
{
  Q_OBJECT
public:
  explicit Client(QObject* parent);
  ~Client();
  void connectToServer(const QString& host, int port, QString protocol);

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
  QString m_protocol = "";
  int m_serverPort = 0;
  Logger m_logger;
  int m_timerReconnect;
  int m_timerSession;
};

#endif // CLIENT_H
