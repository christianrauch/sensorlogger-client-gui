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
    void page_select_setting();
    void on_update_address_released();

signals:
    void update_host_port(const QString host, const int port);

private:
    Ui::MainWindow *ui;
    void page_change(QString page_name);
};

#endif // MAINWINDOW_H
