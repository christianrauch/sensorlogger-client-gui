#include "mainwindow.h"
#include <QApplication>

#include <QtCore>
#include <QStatusBar>
#include <QLabel>

#include <QDebug>

#include "config.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QStatusBar *sbar = w.statusBar();
    QLabel *status_label = new QLabel("Message");
    sbar->addWidget(status_label);

    Config cfg;
    Connection com;

    QObject::connect(&com, SIGNAL(connectionFAIL(QString,int)), &w, SLOT(page_connection_edit(QString,int)));
    QObject::connect(&w, SIGNAL(update_host_port(QString,int)), &cfg, SLOT(setHostPort(QString,int)));

    QStateMachine machine;
    QState *check_config = new CheckConfigState(&cfg);
    QState *default_config = new DefaultConfigState(&cfg);
    QState *connect_server = new ConnectionState(&cfg, &com);
    QState *conn_settings = new AskHostPortState(&cfg);
    QState *req_settings = new ReqSettingsState(&cfg, &com);

    check_config->assignProperty(status_label, "text","Check config file...");
    check_config->addTransition(&cfg, SIGNAL(configFound()), connect_server);
    check_config->addTransition(&cfg, SIGNAL(configNotFound()), default_config);

    default_config->assignProperty(status_label, "text","Setting default configuration");
    default_config->addTransition(&cfg, SIGNAL(defaultConfigSet()), connect_server);

    connect_server->assignProperty(status_label, "text","Connecting to server");
    connect_server->addTransition(&com, SIGNAL(connectionFAIL(QString,int)), conn_settings);
    connect_server->addTransition(&com, SIGNAL(connectionOK()), req_settings);

    conn_settings->assignProperty(status_label, "text","Connection failed.");
    conn_settings->addTransition(&cfg, SIGNAL(updatedHostPort()), connect_server);

    req_settings->assignProperty(status_label, "text","Req: settings");

    QObject::connect(req_settings, SIGNAL(entered()), &w, SLOT(page_select_setting()));

    machine.addState(check_config);
    machine.addState(default_config);
    machine.addState(connect_server);
    machine.addState(conn_settings);
    machine.addState(req_settings);

    machine.setInitialState(check_config);

    machine.start();


    return a.exec();
}
