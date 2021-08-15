#ifndef CONNECTION
#define CONNECTION

#include <QState>
#include <QObject>
#include <QBuffer>
#include <QImage>
#include <QDebug>

#include "config.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>

#include <SensorLoggerCommunication.h>
#include <SensorLoggerCommunication_types.h>

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


class CheckConnectionState: public QState {
private:
    Config *cfg;
    Connection *com;
public:
    CheckConnectionState(Config *conf, Connection *conn):cfg(conf),com(conn) {}

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
    Q_OBJECT
private:
    Connection *com;
public:
    ReqSettingsState(Connection *conn):com(conn) {}

    virtual void onEntry(QEvent * event) {
        std::vector<std::string> settings;
        com->client->getSettings(settings);
        for(auto s: settings) {
            qDebug()<<QString::fromStdString(s);
        }

        switch (settings.size()) {
        case 0:
            qDebug()<<"found no setting";
            emit settingsZero();
            break;
        case 1:
            qDebug()<<"found: "<<QString::fromStdString(settings[0]);
            emit settingsOne(settings[0]);
            break;
        default:
            qDebug()<<"found multiple settings:";
            for(auto s: settings)
                qDebug()<<QString::fromStdString(s);

            emit settingsMultiple(settings);
            break;
        }
    }
signals:
    void settingsZero();
    void settingsOne(std::string setting_name);
    void settingsMultiple(std::vector<std::string> settings);
};

class AddNewSettingState: public QState {
    Q_OBJECT
private:
    Config *cfg;
    Connection *com;
public:
    AddNewSettingState(Config *conf, Connection *conn):cfg(conf),com(conn) {}

    virtual void onEntry(QEvent * event) {

    }
public slots:
    void set_setting(QString name, QImage img) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, "PNG");
        qDebug()<<"add setting: "<<name;
        com->client->addSetting(name.toStdString(), ba.toBase64().toStdString());
    }
};

class AddSensorsState: public QState {
    Q_OBJECT
private:
    Connection *com;
public:
    AddSensorsState(Connection *conn):com(conn) {}

    virtual void onEntry(QEvent * event) {
        std::vector<communication::Sensor> sensors_ow;
        com->client->getSensorOnline(sensors_ow);
        qDebug()<<"sensors online: "<<sensors_ow.size();
        for(auto s: sensors_ow) {
            qDebug()<<s.time<<" "<<QString::fromStdString(s.type);
        }
    }
};

class ChooseSettingState: public QState {
private:
    Config *cfg;
    Connection *com;
public:
    ChooseSettingState(Config *conf, Connection *conn):cfg(conf),com(conn) {}

    virtual void onEntry(QEvent * event) {

    }
};

#endif // CONNECTION

