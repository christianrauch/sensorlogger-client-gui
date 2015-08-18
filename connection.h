#ifndef CONNECTION
#define CONNECTION

#include <QState>
#include <QObject>
#include <QDebug>

#include "config.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>

#include <SensorLoggerCommunication.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class Connection: public QObject {
    Q_OBJECT
private:
    boost::shared_ptr<TSocket> socket;

public:
    communication::SensorLoggerCommunicationClient *client;

    void setup(Config *cfg) {
        //QMessageLogger().debug()<<"setting "<<QString::fromStdString(cfg->getHost())<<":"<<cfg->getPort();
        socket = boost::shared_ptr<TSocket>(new TSocket(cfg->getHost(), cfg->getPort()));
    }

    void connect() {
        try{
            socket->open();
        }
        catch(TException& tx) {
            QMessageLogger().critical()<<"Connection ERROR: "<<tx.what();
            socket->close();
        }

        if(socket->isOpen()) {
            QMessageLogger().debug("connection OK");
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(socket));
            client = new communication::SensorLoggerCommunicationClient(protocol);
            emit connectionOK();
        }
        else {
            QMessageLogger().debug("connection FAIL");
            emit connectionFAIL(QString::fromStdString(socket->getHost()), socket->getPort());
        }
    }

signals:
    void connectionOK();
    void connectionFAIL(QString host, int port);
};


class ConnectionState: public QState {
private:
    Config *cfg;
    Connection *com;
public:
    ConnectionState(Config *conf, Connection *conn):cfg(conf),com(conn) {}

    virtual void onEntry(QEvent * event) {
        QMessageLogger().debug()<<"setting "<<QString::fromStdString(cfg->getHost())<<":"<<cfg->getPort();
        com->setup(cfg);
        QMessageLogger().debug("connecting ...");
        com->connect();
    }

    virtual void onExit(QEvent * event) {
        QMessageLogger().debug("exit connecting ...");
    }
};

class AskHostPortState: public QState {
private:
    Config *cfg;
public:
    AskHostPortState(Config *conf):cfg(conf) {}

    virtual void onEntry(QEvent * event) {
        QMessageLogger().debug("ask host:port");
    }
};

class ReqSettingsState: public QState {
private:
    Config *cfg;
    Connection *com;
public:
    ReqSettingsState(Config *conf, Connection *conn):cfg(conf),com(conn) {}

    virtual void onEntry(QEvent * event) {
        std::vector<std::string> settings;
        com->client->getSettings(settings);
        for(auto s: settings) {
            qDebug()<<QString::fromStdString(s);
        }
    }
};

#endif // CONNECTION

