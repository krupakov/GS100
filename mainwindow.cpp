#include "mainwindow.h"
#include "ui_mainwindow.h"

enum Shards {
    THREAD_OF_FATE     = 1,
    STAR_OF_FORTUNE    = 2201,
    GODS_LEGACY        = 101,
    ETERNAL_CALL       = 501,
    YOUNG_GUARD        = 601
};

enum Classes {
    BARD       = 274401281,
    WARRIOR    = 61117,
    MAGE       = 64104,
    HEALER     = 61119,
    ENGINEER   = 739833443,
    PSIONICIST = 64105,
    SUMMONER   = 61121,
    SCOUT      = 61123,
    PALADIN    = 61120,
    WARDEN     = 64103,
    WARLOCK    = 740021330
};

QMap<Shards, QString> ShardName {
    {THREAD_OF_FATE, "Нить судьбы"},
    {STAR_OF_FORTUNE, "Звезда Удачи"},
    {GODS_LEGACY, "Наследие Богов"},
    {ETERNAL_CALL, "Вечный Зов"},
    {YOUNG_GUARD, "Молодая Гвардия"}
};
Classes ClassesVector[] = {BARD, WARRIOR, MAGE, HEALER, ENGINEER, PSIONICIST, SUMMONER, SCOUT, PALADIN, WARDEN, WARLOCK};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    apiLinkRu = "https://allods.mail.ru/api/rating/gearscore/";

    QSettings m("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);
    QString path = m.value("Аллоды Онлайн/InstallLocation").toString();
    if (path.isEmpty()) {
        path = m.value("gcgame_0.359/InstallLocation").toString();
    }
    ui->browseLineEdit->setText(path.isEmpty() ? "" : path + "data\\Mods\\Addons\\GS100");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fetchData(int shardId, QString *result, QString *apiLink)
{
    QList<QUrl> urls;

    for (Classes classId : ClassesVector) {
        urls.append(*apiLink + QString::number(shardId) + "/" + QString::number(classId));
    }

    QNetworkAccessManager manager;
    QList<QNetworkReply*> replies;
    QEventLoop loop;

    int total = urls.length();
    foreach (const QUrl &url, urls) {
        QNetworkReply *reply = manager.get(QNetworkRequest(url));
        QObject::connect(reply, &QNetworkReply::finished, this, [&total, &loop]() {
            total--;
            if (total < 1) {
                loop.quit();
            }
        });
        replies << reply;
    }
    loop.exec();

    *result += "GS100[\"" + ShardName.value(static_cast<Shards>(shardId)) + "\"] = {}\n";

    foreach (QNetworkReply *reply, replies) {
        QByteArray r(reply->readAll());
        QJsonDocument jsonDocument = QJsonDocument::fromJson(r);
        QJsonArray jsonArray = jsonDocument.array();

        foreach (QJsonValue val, jsonArray) {
            QJsonObject obj = val.toObject();

            *result += "GS100[\"" + ShardName.value(static_cast<Shards>(shardId)) +
                    "\"][\"" + obj.value("name").toString() + "\"] = {\"" +
                    obj.value("guild").toString() + "\", " +
                    obj.value("gearscore").toString().mid(0, obj.value("gearscore").toString().indexOf('.')) + "}\n";
        }
    }

    qDeleteAll(replies.begin(), replies.end());
    replies.clear();
}

void MainWindow::writeFile(int shardId, QFile *file, bool *isError)
{
    QString result;

    fetchData(shardId, &result, &apiLinkRu);

    if (result.isEmpty()) {
        *isError = true;
        return;
    }

    QTextStream writeStream(file);
    writeStream.setCodec("Windows-1251");
    writeStream << result;
    writeStream.flush();
}


void MainWindow::on_browseButton_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this, "Выбор папки", ui->browseLineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (folderName != "") {
        ui->browseLineEdit->setText(folderName);
    }
}

void MainWindow::on_updateButton_clicked()
{
    if (ui->browseLineEdit->text().isEmpty() || !QDir(ui->browseLineEdit->text()).exists()) {
        ui->resultLabel->setStyleSheet("QLabel { color : red; }");
        ui->resultLabel->setText("Директория не найдена");

        return;
    }

    if (!ui->threadOfFateCheckBox->isChecked() &&
        !ui->starOfFortuneCheckBox->isChecked() &&
        !ui->godsLegacyCheckBox->isChecked() &&
        !ui->eternalCallCheckBox->isChecked() &&
        !ui->youngGuardCheckBox->isChecked())
    {
        ui->resultLabel->setStyleSheet("QLabel { color : red; }");
        ui->resultLabel->setText("Выберите сервер");

        return;
    }

    ui->resultLabel->setStyleSheet("QLabel { color : black; }");
    ui->resultLabel->setText("Загрузка...");
    ui->updateButton->setEnabled(false);

    bool isError = false;

    QFile threadOfFateFile(ui->browseLineEdit->text() + "/threadOfFate.txt");
    QFile starOfFortuneFile(ui->browseLineEdit->text() + "/starOfFortune.txt");
    QFile godsLegacyFile(ui->browseLineEdit->text() + "/godsLegacy.txt");
    QFile eternalCallFile(ui->browseLineEdit->text() + "/eternalCall.txt");
    QFile youngGuardFile(ui->browseLineEdit->text() + "/youngGuard.txt");
    QFile configFile(ui->browseLineEdit->text() + "/config.txt");

    if (threadOfFateFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        starOfFortuneFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        godsLegacyFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        eternalCallFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        youngGuardFile.open(QIODevice::WriteOnly | QIODevice::Text) &&
        configFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (ui->threadOfFateCheckBox->isChecked()) {
            writeFile(THREAD_OF_FATE, &threadOfFateFile, &isError);
        }
        if (ui->starOfFortuneCheckBox->isChecked()) {
            writeFile(STAR_OF_FORTUNE, &starOfFortuneFile, &isError);
        }
        if (ui->godsLegacyCheckBox->isChecked()) {
            writeFile(GODS_LEGACY, &godsLegacyFile, &isError);
        }
        if (ui->eternalCallCheckBox->isChecked()) {
            writeFile(ETERNAL_CALL, &eternalCallFile, &isError);
        }
        if (ui->youngGuardCheckBox->isChecked()) {
            writeFile(YOUNG_GUARD, &youngGuardFile, &isError);
        }

        QTextStream writeStream(&configFile);
        writeStream.setCodec("Windows-1251");
        writeStream << ("Global(\"GS100\", {})\nGlobal(\"UpdatedAt\", \"" + QDateTime::currentDateTime().toString("hh:mm:ss dd.MM.yyyy") + "\")");
        writeStream.flush();

        threadOfFateFile.close();
        starOfFortuneFile.close();
        godsLegacyFile.close();
        eternalCallFile.close();
        youngGuardFile.close();
        configFile.close();
    } else {
        isError = true;
    }

    ui->updateButton->setEnabled(true);

    if (isError) {
        ui->resultLabel->setStyleSheet("QLabel { color : red; }");
        ui->resultLabel->setText("Ошибка при записи файла");

        return;
    }

    ui->resultLabel->setStyleSheet("QLabel { color : green; }");
    ui->resultLabel->setText("Успешно обновлено");
}
