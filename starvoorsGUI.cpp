#include "starvoorsGUI.h"
#include "ui_starvoorsGUI.h"
#include <QtGui>
#include <iostream>
#include <string>

StarvoorsGUI::StarvoorsGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StarvoorsGUI)
{
    ui->setupUi(this);
    ui->console->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    ui->source->setFocus();
    //avoids changing the size of the app, e.g. no maximize
    setFixedSize(width(), height());
    //Centers main window
    this->move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
}

StarvoorsGUI::~StarvoorsGUI()
{
    delete ui;
}

void StarvoorsGUI::on_toolButton_clicked()
{
    QString path;
    path = browseDir(tr("Choose directory"));
    ui->source->setText( path );
}

void StarvoorsGUI::on_toolButton_2_clicked()
{
    QString path;
    path = browseFile(tr("Choose file"));
    ui->ppdate->setText( path );
}

void StarvoorsGUI::on_toolButton_3_clicked()
{
    QString path;
    path = browseDir(tr("Choose directory"));
    ui->output->setText( path );
}

QString StarvoorsGUI::browseDir(QString s)
{
    return QFileDialog::getExistingDirectory(this, s,
                        QString::null,
                        QFileDialog::ShowDirsOnly
                        | QFileDialog::DontResolveSymlinks);
}


QString StarvoorsGUI::browseFile(QString s)
{
    return QFileDialog::getOpenFileName(this, s, QString::null, QString::null);
}


void StarvoorsGUI::on_button_run_clicked()
{
    if (!checkArguments()) { return; }
    if (!(ui->console->toPlainText() == ""))
        ui->console->clear();

    ui->tabWidget->setCurrentIndex(2);
    QString currentDir = QDir::currentPath();

    QString program = "/home/chimento/tool StaRVOOrS/StaRVOOrS";

    QStringList arguments;

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, makeStarvoorsCall(arguments));
    myProcess->waitForFinished();
    QString result_all = myProcess->readAllStandardOutput();
    ui->console->setText(result_all);
}

QStringList StarvoorsGUI::makeStarvoorsCall(QStringList args){
  return args << ui->source->toPlainText()
              << ui->ppdate->toPlainText()
              << ui->output->toPlainText();
}

bool StarvoorsGUI::checkArguments(){

    switch (ui->only_parse->isChecked()) {
    case true:
        if (ui->ppdate->toPlainText() == "") {
            QMessageBox::warning(this, tr("Error"),
                                exitMessage(10),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
            ui->ppdate->setFocus();
            return false;
        }
        else {
            return true;
        }
        break;
    default:
        int res = argumentsEmpty();
        switch (res) {
        case 0:
            return true;
            break;
        default:
            QMessageBox::warning(this, tr("Error"),
                                exitMessage(res),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
            return false;
            break;
        }
        break;
    }
}

QString StarvoorsGUI::exitMessage(int n){
    switch (n) {
    case 1:
        return tr("Missing source code address.\n");
        break;
    case 10:
        return tr("Missing ppDATE specification file.\n");
        break;
    case 11:
          return "Missing source code address.\n"
                 "Missing ppDATE specification file.\n";
        break;
    case 101:
        return tr("Missing source code address.\n")
                + tr("Missing output address.\n");
        break;
    case 110:
        return tr("Missing ppDATE specification file.\n")
               + tr("Missing output address.\n");
        break;
    case 100:
        return tr("Missing output address.\n");
        break;
    case 111:
        return tr("Missing source code address.\n")
               + tr("Missing ppDATE specification file.\n")
               + tr("Missing output address.\n");
        break;
    default:
        return tr("Missing information.\n");
        break;
    }
}

int StarvoorsGUI::argumentsEmpty(){
    int res = 0;
    if (ui->source->toPlainText() == "")
        res = res + 1;
    if (ui->ppdate->toPlainText() == "")
        res = res + 10;
    if (ui->output->toPlainText() == "")
        res = res + 100;
    return res;
}

void StarvoorsGUI::on_pushButton_2_clicked()
{
    ui->source->clear();
}
