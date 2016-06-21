#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

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
    void on_closeButton_clicked();

    void on_submitButton_clicked();

    void onResult(QNetworkReply* reply);
    void requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
