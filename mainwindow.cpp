#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QEasingCurve>

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
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(ui->urlLineEdit->currentText())));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(requestDownloadProgress(qint64, qint64)));

    showProgressBar(true);
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

void MainWindow::onResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Network error", reply->errorString());
        showProgressBar(false);
        return;
    }

    ui->resultTextEdit->clear();

    QString download = (QString) reply->readAll();

    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(download.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Error found in Json document", err.errorString() + " Position : " + QString::number(err.offset));
        showProgressBar(false);
        return;
    }

    ui->resultTextEdit->setPlainText(json.toJson());

    showProgressBar(false);
}

void MainWindow::requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal < 1024) {
        ui->progressBar->setFormat("%v / %m");
        ui->progressBar->setValue(bytesReceived);
        ui->progressBar->setMaximum(bytesTotal);
    }
    else {
        ui->progressBar->setFormat("%v kio / %m kio");
        ui->progressBar->setValue(bytesReceived / 1024);
        ui->progressBar->setMaximum(bytesTotal / 1024);
    }
}

void MainWindow::on_MainWindow_destroyed()
{

}

void MainWindow::showProgressBar(bool show = true)
{
    if (show) {
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(0);
        QPropertyAnimation* a = new QPropertyAnimation(ui->progressBar, "maximumSize");
        a->setDuration(1000);
        a->setEndValue(QSize(16777215, 32));
        a->setEasingCurve(QEasingCurve::OutExpo);
        a->start();
    }
    else {
        ui->progressBar->setFormat("");
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(0);
        QPropertyAnimation* a = new QPropertyAnimation(ui->progressBar, "maximumSize");
        a->setDuration(1000);
        a->setEndValue(QSize(16777215, 0));
        a->setEasingCurve(QEasingCurve::OutExpo);
        a->start();
    }
}
