#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::page_connection_edit(QString host, int port)
{
    QLabel *message = ui->stackedWidget->findChild<QLabel*>("not_connected_message");
    message->setText("Could not connect to server: <b>"+host+"</b> on port: <b>"+QString::number(port)+"</b> failed!");
    page_change("connection_edit");
}

void MainWindow::page_select_setting() {
    page_change("select_setting");
}

void MainWindow::page_change(QString page_name) {
    for(int i = 0; i<ui->stackedWidget->count(); i++) {
        if(ui->stackedWidget->widget(i)->objectName() == page_name)
            ui->stackedWidget->setCurrentWidget(ui->stackedWidget->widget(i));
    }
}

void MainWindow::on_update_address_released()
{
    qDebug()<<ui->edit_host->property("text");
    qDebug()<<ui->edit_port->property("text");
    emit update_host_port(ui->edit_host->property("text").toString(), ui->edit_port->property("text").toInt());
}
