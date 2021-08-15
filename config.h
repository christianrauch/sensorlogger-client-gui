#ifndef CONFIG
#define CONFIG

#include <QState>
#include <QObject>
#include <glib.h>
#include <iostream>

#include <QDebug>

class Config: public QObject {
    Q_OBJECT

private:
    GKeyFile *cfg;

public:
    Config() {
        cfg = g_key_file_new();
    }

    ~Config() {
        g_key_file_free(cfg);
    }

    void checkConfigAvailable(const std::string filename = "sensorlogger-client/sensorlogger-client.conf") {
        const gchar* user_config_dir = g_get_user_config_dir();
        std::cout<<"searching in user dir: "<<std::string(user_config_dir)<<std::endl;

        if(g_key_file_load_from_dirs(cfg, filename.c_str(), &user_config_dir, NULL, G_KEY_FILE_NONE, NULL)) {
            // file found
            emit configFound();
        }
        else {
            // file not found
            QMessageLogger().debug("cfg file not found");
            emit configNotFound();
        }
    }

    void setDefaultConfig() {
        std::cout<<"setting default config"<<std::endl;
        //g_key_file_set_string(cfg, "connection", "host", "localhost");
        g_key_file_set_string(cfg, "connection", "host", "debian");
        g_key_file_set_integer(cfg, "connection", "port", 9090);
        emit defaultConfigSet();
    }

    std::string getHost() {
        return std::string(g_key_file_get_string(cfg, "connection", "host", NULL));
    }

    int getPort() {
        return g_key_file_get_integer(cfg, "connection", "port", NULL);
    }

public slots:
    void setHostPort(const std::string host, const int port) {
        QMessageLogger().debug()<<"update host: "<<QString::fromStdString(host)<<", port: "<<port;
        g_key_file_set_string(cfg, "connection", "host", host.c_str());
        g_key_file_set_integer(cfg, "connection", "port", port);
        emit updatedHostPort();
    }

    void setHostPort(const QString host, const int port) {
        setHostPort(host.toStdString(), port);
    }

signals:
    void configFound();
    void configNotFound();
    void defaultConfigSet();
    void updatedHostPort();
};

class CheckConfigState: public QState {
private:
    Config *cfg;
public:
    CheckConfigState(Config *c):cfg(c) {}

    virtual void onEntry(QEvent * event) {
        std::cout<<"entered CheckConfigState"<<std::endl;
        cfg->checkConfigAvailable();
    }

};

class DefaultConfigState: public QState {
private:
    Config *cfg;
public:
    DefaultConfigState(Config *c):cfg(c) {}

    virtual void onEntry(QEvent * event) {
        std::cout<<"entered DefaultConfigState"<<std::endl;
        cfg->setDefaultConfig();
    }

};

#endif // CONFIG

