#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_submitButton_clicked()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(ui->urlLineEdit->text())));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(requestDownloadProgress(qint64, qint64)));
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

void MainWindow::onResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString data = (QString) reply->readAll();
    ui->textBrowser->setText(data);
}

void MainWindow::requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    qint64 pourcent = (bytesReceived / bytesTotal) * 100;
    ui->progressBar->setValue(pourcent);
}
