#ifndef CLIENT_H
#define CLIENT_H
#include "Protocol.h"
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
  explicit Client(QObject* parent, const QString& host, int port,
                  const DataFormat& protocol);
  ~Client();

signals:
  void finished();
  void started();

protected:
  void timerEvent(QTimerEvent* event) override;

private slots:
  void connected();
  void errorOccurred(QAbstractSocket::SocketError socketError);
  void disconnected();
  void readyRead();
  void sendStart();
  void sendStop();
  void stop();

private:
  QDataStream in;
  QTcpSocket* m_socket = nullptr;
  Protocol* m_protocol = nullptr;
  QString m_currentMessage;
  QString m_serverHost = "";
  int m_serverPort = 0;
  Logger m_logger;
  int m_timerId;
  QByteArray m_buffer;
  int m_id;
};

#endif // CLIENT_H
