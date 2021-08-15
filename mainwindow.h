#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void page_connection_edit(QString host, int port);
    void page_add_setting();
    void page_add_sensors();
    void page_select_setting();

    void on_update_address_released();
    void on_choose_image_clicked();

    void on_set_setting_clicked();

signals:
    void update_host_port(const QString host, const int port);
    void set_setting(QString, QImage);

private:
    Ui::MainWindow *ui;
    QImage *img_setting;
    void page_change(QString page_name);
};

#endif // MAINWINDOW_H
