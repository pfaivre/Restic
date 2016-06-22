#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonArray>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "route.h"
#include "state.h"

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
    ui->plainTextEdit->clear();

    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString download = (QString) reply->readAll();
//    ui->plainTextEdit->setText(data);

    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(download.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Erreur", err.errorString() + " Position : " + err.offset);
        return;
    }

    QJsonObject root = json.object();

    Route route;
    route.IdGroupeExars = root["IdGroupeExars"].toString();
    route.NomCentreInit = root["NomCentreInit"].toString();
    route.NomCentreDest = root["NomCentreDest"].toString();

    ui->plainTextEdit->appendPlainText(route.IdGroupeExars + " : " + route.NomCentreInit + " -> " + route.NomCentreDest);

    QJsonArray data = root["DATA"].toArray();

    for (int i = 0 ; i < data.count() ; i++) {
        QJsonObject s = data.at(i).toObject();

        State state;

        state.id = s["id"].toString();
        state.DateHeure = QDateTime::fromString(s["DateHeure"].toString(), Qt::ISODate);
        state.Longitude = s["Longitude"].toDouble();
        state.Latitude = s["Latitude"].toDouble();
        state.Gyro_yaw = s["Gyro_yaw"].toDouble();
        state.Gyro_pitch = s["Gyro_pitch"].toDouble();
        state.Gyro_roll = s["Gyro_roll"].toDouble();
        state.Accel_x = s["Accel_x"].toDouble();
        state.Accel_y = s["Accel_y"].toDouble();
        state.Accel_z = s["Accel_z"].toDouble();
        state.Temperature = s["Temperature"].toDouble();
        state.Humidite = s["Humidite"].toInt();

        ui->plainTextEdit->appendPlainText(state.toString());
    }
}

void MainWindow::requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    qint64 pourcent = (bytesReceived / bytesTotal) * 100;
    ui->progressBar->setValue(pourcent);
}

void MainWindow::on_urlLineEdit_returnPressed()
{
    on_submitButton_clicked();
}
