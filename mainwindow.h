#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTime>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fetchData(int shardId, QString *result, QString *apiLink);

    void writeFile(int shardId, QFile *file, bool *isError);

private slots:
    void on_browseButton_clicked();

    void on_updateButton_clicked();

private:
    Ui::MainWindow *ui;

    QString apiLinkRu;
};
#endif // MAINWINDOW_H
