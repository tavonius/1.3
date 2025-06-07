#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_getWeatherBtn_clicked();
    void handleNetworkReply(QNetworkReply* reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QSettings settings;

    QString getRangeCount(int index);
    bool isTargetHour(const QString &dt);
    void loadSettings();
    void saveSettings();
    void updateHistory(const QString &entry);
};

#endif // MAINWINDOW_H
