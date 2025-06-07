#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      manager(new QNetworkAccessManager(this)),
      settings("MyCompany", "WeatherApp")
{
    ui->setupUi(this);
    connect(ui->getWeatherBtn, &QPushButton::clicked,
            this, &MainWindow::on_getWeatherBtn_clicked);
    connect(manager, &QNetworkAccessManager::finished,
            this, &MainWindow::handleNetworkReply);
    loadSettings();
}

MainWindow::~MainWindow() {
    saveSettings();
    delete ui;
}

void MainWindow::on_getWeatherBtn_clicked() {
    QString city = ui->cityLineEdit->text().trimmed();
    QString range = getRangeCount(ui->rangeComboBox->currentIndex());
    QString apiKey = "YOUR_API_KEY"; // Замени на свой ключ

    QString url = QString(
      "https://api.openweathermap.org/data/2.5/forecast"
      "?q=%1&units=metric&cnt=%2&appid=%3")
      .arg(city, range, apiKey);

    manager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::handleNetworkReply(QNetworkReply* reply) {
    if (reply->error()) {
        ui->resultTextEdit->setText("Ошибка: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();
    if (!root.contains("list")) {
        QString msg = root.value("message").toString("Нет данных");
        ui->resultTextEdit->setText("Ошибка: " + msg);
        reply->deleteLater();
        return;
    }

    QJsonArray list = root["list"].toArray();
    QString result;
    for (auto val : list) {
        auto obj = val.toObject();
        QString dt = obj["dt_txt"].toString();
        if (!isTargetHour(dt)) continue;

        double temp = obj["main"].toObject()["temp"].toDouble();
        QString weather = obj["weather"].toArray()
                          .at(0).toObject()["description"].toString();

        result += QString("%1; Темп: %2°C; %3\n")
                    .arg(dt).arg(temp).arg(weather);
    }

    ui->resultTextEdit->setPlainText(result);
    updateHistory(result);
    reply->deleteLater();
}

QString MainWindow::getRangeCount(int index) {
    switch (index) {
        case 0: return "1";
        case 1: return "8";
        case 2: return "24";
        default: return "1";
    }
}

bool MainWindow::isTargetHour(const QString &dt) {
    return dt.endsWith("09:00:00")
        || dt.endsWith("15:00:00")
        || dt.endsWith("21:00:00");
}

void MainWindow::loadSettings() {
    ui->cityLineEdit->setText(settings.value("city").toString());
    ui->rangeComboBox->setCurrentIndex(
        settings.value("rangeIndex").toInt());
    QStringList hist = settings.value("history").toStringList();
    ui->historyTextEdit->setPlainText(
        hist.join("\n-----------------\n"));
}

void MainWindow::saveSettings() {
    settings.setValue("city", ui->cityLineEdit->text());
    settings.setValue("rangeIndex",
                      ui->rangeComboBox->currentIndex());
}

void MainWindow::updateHistory(const QString &entry) {
    QStringList hist = settings.value("history").toStringList();
    hist.append(entry);
    if (hist.size() > 10) hist.removeFirst();
    settings.setValue("history", hist);
    ui->historyTextEdit->setPlainText(
        hist.join("\n-----------------\n"));
}
