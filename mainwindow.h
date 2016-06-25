#pragma once

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

    void showProgressBar(bool show);

private slots:
    void on_closeButton_clicked();

    void on_submitButton_clicked();

    void onResult(QNetworkReply* reply);
    void requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void on_MainWindow_destroyed();

private:
    Ui::MainWindow *ui;
};

