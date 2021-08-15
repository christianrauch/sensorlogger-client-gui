#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    img_setting = NULL;
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

void MainWindow::page_add_setting() {
    page_change("setting_name_file");
}

void MainWindow::page_select_setting() {
    page_change("select_setting");
}

void MainWindow::page_add_sensors() {
    page_change("add_sensors");
}

void MainWindow::page_change(QString page_name) {
    for(int i = 0; i<ui->stackedWidget->count(); i++) {
        if(ui->stackedWidget->widget(i)->objectName() == page_name)
            ui->stackedWidget->setCurrentWidget(ui->stackedWidget->widget(i));
    }
    qDebug()<<"Widget changed to: "<<ui->stackedWidget->currentWidget()->objectName();
}

void MainWindow::on_update_address_released()
{
    ui->statusBar->showMessage("connecting ...");
    qDebug()<<ui->edit_host->property("text");
    qDebug()<<ui->edit_port->property("text");
    emit update_host_port(ui->edit_host->property("text").toString(), ui->edit_port->property("text").toInt());
    ui->statusBar->clearMessage();
}

void MainWindow::on_choose_image_clicked()
{
    QString img_file_path = QFileDialog::getOpenFileName(
                this, tr("Open Image"), QDir::homePath(),
                tr("Image Files (*.png *.jpg *.jpeg *.PNG *.JPEG *.JPG)"));
    qDebug()<<"img file: "<<img_file_path;

    img_setting = new QImage();
    img_setting->load(img_file_path);

    QPixmap img_bmp = QPixmap::fromImage(*img_setting);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->addPixmap(img_bmp);
    ui->graphicsView->setScene(scene);
}

void MainWindow::on_set_setting_clicked()
{
    emit set_setting(ui->setting_name_edit->text(), *img_setting);
}
