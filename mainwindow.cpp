#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonArray>
#include <QDateTime>
#include <QStringList>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "route.h"
#include "state.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->urlLineEdit->addItem("");
    ui->urlLineEdit->addItem("http://sky-net.fr:5984/blackbox/ea6b5cc84253db9af86f2dc776002301");
    ui->urlLineEdit->addItem("http://sky-net.fr:5984/blackbox/ea6b5cc84253db9af86f2dc776002741");

    QStandardItemModel *model = new QStandardItemModel(0, 12, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Id")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Date")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Latitude")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("Longitude")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("Temperature")));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString("Humidite")));
    model->setHorizontalHeaderItem(6, new QStandardItem(QString("Accel_x")));
    model->setHorizontalHeaderItem(7, new QStandardItem(QString("Accel_y")));
    model->setHorizontalHeaderItem(8, new QStandardItem(QString("Accel_z")));
    model->setHorizontalHeaderItem(9, new QStandardItem(QString("Gyro_yaw")));
    model->setHorizontalHeaderItem(10, new QStandardItem(QString("Gyro_pitch")));
    model->setHorizontalHeaderItem(11, new QStandardItem(QString("Gyro_roll")));

    ui->tableView->setModel(model);
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
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)));

    QPropertyAnimation* animation = new QPropertyAnimation(ui->label, "maximumSize");
    animation->setDuration(1000);
    animation->setEndValue(QSize(16777215, 0));
    animation->setEasingCurve(QEasingCurve::OutExpo);
    animation->start();
    QPropertyAnimation* animation2 = new QPropertyAnimation(ui->labelErrors, "maximumSize");
    animation2->setDuration(1000);
    animation2->setEndValue(QSize(16777215, 0));
    animation2->setEasingCurve(QEasingCurve::OutExpo);
    animation2->start();
    QPropertyAnimation* animation3 = new QPropertyAnimation(ui->progressBar, "maximumSize");
    animation3->setDuration(1000);
    animation3->setEndValue(QSize(16777215, 32));
    animation3->setEasingCurve(QEasingCurve::OutExpo);
    animation3->start();
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

/**
 * Slot appelé quand la requête a aboutit et que l'on a téléchargé les données
 * @brief MainWindow::onResult
 * @param reply
 */
void MainWindow::onResult(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        return;
        QPropertyAnimation* animation3 = new QPropertyAnimation(ui->progressBar, "maximumSize");
        animation3->setDuration(1000);
        animation3->setEndValue(QSize(16777215, 0));
        animation3->setEasingCurve(QEasingCurve::OutExpo);
        animation3->start();
    }

    // Vide la liste si elle avait déjà des données
    ((QStandardItemModel*)ui->tableView->model())->setRowCount(0);

    // Extrait le contenu du téléchargement
    QString download = (QString) reply->readAll();

    // Tente de parser le json
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(download.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Erreur", err.errorString() + " Position : " + QString::number(err.offset));
        ui->progressBar->setValue(0);
        return;
    }

    // Récupère l'objet racine du document Json
    QJsonObject root = json.object();

    // Instancie un objet Route avec les informations du Json
    Route route;
    route.IdGroupeExars = root["IdGroupeExars"].toString();
    route.NomCentreInit = root["NomCentreInit"].toString();
    route.NomCentreDest = root["NomCentreDest"].toString();

    ui->label->setText(route.IdGroupeExars + " : " + route.NomCentreInit + " -> " + route.NomCentreDest);
    QPropertyAnimation* animation = new QPropertyAnimation(ui->label, "maximumSize");
    animation->setDuration(1000);
    //animation->setStartValue(QSize(16777215, 0));
    animation->setEndValue(QSize(16777215, 32));
    animation->setEasingCurve(QEasingCurve::OutExpo);
    animation->start();
    QPropertyAnimation* animation2 = new QPropertyAnimation(ui->labelErrors, "maximumSize");
    animation2->setDuration(1000);
    //animation2->setStartValue(QSize(16777215, 0));
    animation2->setEndValue(QSize(16777215, 32));
    animation2->setEasingCurve(QEasingCurve::OutExpo);
    animation2->start();

    // Récupère le tableau Json contenant les enregistrements
    QJsonArray data = root["DATA"].toArray();

    // Instancie la liste les enregistrements
    State* state = new State();

    long errorCount = 0;

    // Parcours les enregistrements du Json
    for (int i = 0 ; i < data.count() ; i++) {
        QJsonObject s = data.at(i).toObject();

        // Remplit l'objet State
        state->id = s["id"].toString();
        state->DateHeure = QDateTime::fromString(s["DateHeure"].toString(), Qt::ISODate);
        state->Longitude = s["Longitude"].toDouble();
        state->Latitude = s["Latitude"].toDouble();
        state->Gyro_yaw = s["Gyro_yaw"].toDouble();
        state->Gyro_pitch = s["Gyro_pitch"].toDouble();
        state->Gyro_roll = s["Gyro_roll"].toDouble();
        state->Accel_x = s["Accel_x"].toDouble();
        state->Accel_y = s["Accel_y"].toDouble();
        state->Accel_z = s["Accel_z"].toDouble();
        state->Temperature = s["Temperature"].toDouble();
        state->Humidite = s["Humidite"].toInt();

        route.states.append(*state);

        // Affiche cet objet State dans le tableau, avec des couleurs ;)
        QBrush redBrush(Qt::red, Qt::SolidPattern);
        QBrush whiteBrush(Qt::white, Qt::SolidPattern);

        QStandardItem *row = new QStandardItem(state->id);
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 0, row);

        row = new QStandardItem(state->DateHeure.toString(Qt::ISODate));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 1, row);

        row = new QStandardItem(QString::number(state->Latitude));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 2, row);
        if (state->Latitude == 0) {
            row->setForeground(redBrush);
        }

        row = new QStandardItem(QString::number(state->Longitude));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 3, row);
        if (state->Longitude == 0) {
            row->setForeground(redBrush);
        }

        row = new QStandardItem(QString::number(state->Temperature));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 4, row);
        if (state->Temperature == 0) {
            row->setForeground(redBrush);
        }
        else if (state->Temperature > 303) {
            row->setBackground(redBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }
        else if (state->Temperature < 274) {
            row->setBackground(redBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }

        row = new QStandardItem(QString::number(state->Humidite));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 5, row);
        if (state->Humidite == 0) {
            row->setForeground(redBrush);
        }
        else if (state->Humidite > 60) {
            row->setBackground(redBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }

        row = new QStandardItem(QString::number(state->Accel_x));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 6, row);
        if (state->Accel_x == 0) {
            row->setForeground(redBrush);
        }
        else if (state->Accel_x > 20) {
            row->setBackground(redBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }
        else if (state->Accel_x < -20) {
            row->setBackground(redBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }

        row = new QStandardItem(QString::number(state->Accel_y));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 7, row);
        if (state->Accel_y == 0) {
            row->setForeground(redBrush);
        }
        else if (state->Accel_y > 20) {
            row->setBackground(whiteBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }
        else if (state->Accel_y < -20) {
            row->setBackground(whiteBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }

        row = new QStandardItem(QString::number(state->Accel_z));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 8, row);
        if (state->Accel_z == 0) {
            row->setForeground(redBrush);
        }
        else if (state->Accel_z > 20) {
            row->setBackground(whiteBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }
        else if (state->Accel_z < -20) {
            row->setBackground(whiteBrush);
            row->setForeground(whiteBrush);
            errorCount++;
        }

        row = new QStandardItem(QString::number(state->Gyro_yaw));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 9, row);
        if (state->Gyro_yaw == 0) {
            row->setForeground(redBrush);
        }

        row = new QStandardItem(QString::number(state->Gyro_pitch));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 10, row);
        if (state->Gyro_pitch == 0) {
            row->setForeground(redBrush);
        }

        row = new QStandardItem(QString::number(state->Gyro_roll));
        ((QStandardItemModel*)ui->tableView->model())->setItem(i, 11, row);
        if (state->Gyro_roll == 0) {
            row->setForeground(redBrush);
        }

        // Affiche la progression de l'analyse du fichier Json (50% -> 100%)
        ui->progressBar->setValue(((i * 1.0f / data.count()) * 100) / 2 + 50);
    }

    ui->tableView->setVisible(false);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setVisible(true);

    if (errorCount > 0)
        ui->labelErrors->setText("Valeurs hors normes : " + QString::number(errorCount));
    else
        ui->labelErrors->setText("");

    ui->progressBar->setValue(100);
    QPropertyAnimation* animation3 = new QPropertyAnimation(ui->progressBar, "maximumSize");
    animation3->setDuration(1000);
    animation3->setEndValue(QSize(16777215, 0));
    animation3->setEasingCurve(QEasingCurve::OutExpo);
    animation3->start();

    delete state;
}

/**
 * Slot appelé quand le téléchargement progresse
 * @brief MainWindow::requestDownloadProgress
 * @param bytesReceived
 * @param bytesTotal
 */
void MainWindow::requestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    int pourcent = (bytesReceived * 1.0f / bytesTotal) * 100 / 2; // (0% -> 50%)
    ui->progressBar->setValue(pourcent);
}

/**
 * Slot appelé quand le téléchargement a échoué
 * @brief MainWindow::requestError
 * @param error
 */
void MainWindow::requestError(QNetworkReply::NetworkError error) {
    QMessageBox::warning(this, "Erreur de connexion", "Impossible d'accéder à l'URL demandée");
    ui->progressBar->setValue(0);
    QPropertyAnimation* animation3 = new QPropertyAnimation(ui->progressBar, "maximumSize");
    animation3->setDuration(1000);
    animation3->setEndValue(QSize(16777215, 0));
    animation3->setEasingCurve(QEasingCurve::OutExpo);
    animation3->start();
}

void MainWindow::on_urlLineEdit_returnPressed()
{
    on_submitButton_clicked();
}

void MainWindow::on_MainWindow_destroyed()
{

}
