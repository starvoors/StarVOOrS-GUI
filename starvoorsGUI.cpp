#include "starvoorsGUI.h"
#include "ui_starvoorsGUI.h"

#include <QWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>


StarvoorsGUI::StarvoorsGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StarvoorsGUI)
{
    ui->setupUi(this);
    ui->console->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    ui->source->setFocus();
    ui->pushButton_2->setEnabled(false);
    starvoorsExec = StarvoorsExec::NoneExec;

    //setting up the menu bar
    QVBoxLayout *boxLayout = new QVBoxLayout(this); // Main layout of widget
    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = new QMenu("&File");
    QMenu *helpMenu = new QMenu("&Help");

    QAction* exitAction = new QAction(tr("&Exit"), this);
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction* aboutAction = new QAction(tr("&About"), this);
    exitAction->setStatusTip(tr("Information about the GUI"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    fileMenu->addAction(exitAction);
    helpMenu->addAction(aboutAction);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);

    this->setLayout(boxLayout);
    boxLayout->setMenuBar(menuBar);

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

    ui->pushButton_2->setEnabled(true);
    ui->button_run->setEnabled(false);
    ui->tabWidget->setCurrentIndex(2);
    QString currentDir = QDir::currentPath();
    QString starvoors = currentDir + tr("/StaRVOOrS");
    QStringList arguments;

    process = new QProcess(this);

    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readFromConsole()));
    connect(process,SIGNAL(readyReadStandardError()),this,SLOT(readFromConsole()));
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(resetComponents(int,QProcess::ExitStatus)));

    process->start(starvoors, makeStarvoorsCall(arguments));
}

QStringList StarvoorsGUI::makeStarvoorsCall(QStringList args){

    QStringList flags ;

    if (ui->only_parse->isChecked())
        return args << tr("-p") << ui->ppdate->text();
    if (ui->only_rv->isChecked())
        flags << tr("-r");
    if (ui->none_verbose->isChecked())
        flags << tr("-n");
    if (ui->xml->isChecked())
        flags << tr("-x");

    return args << ui->source->text()
                << flags
                << ui->ppdate->text()
                << ui->output->text();
}

bool StarvoorsGUI::checkArguments(){

    switch (ui->only_parse->isChecked()) {
    case true:
        if (ui->ppdate->text() == "") {
            QMessageBox msgb (tr("Error"),
                        exitMessage(10),
                        QMessageBox::Warning,
                        QMessageBox::Ok,
                        QMessageBox::NoButton,
                        QMessageBox::NoButton);
            msgb.exec();
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
            QMessageBox msgb (tr("Error"),
                        exitMessage(res),
                        QMessageBox::Warning,
                        QMessageBox::Ok,
                        QMessageBox::NoButton,
                        QMessageBox::NoButton);
            msgb.exec();
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
    case 100:
        return tr("Missing output address.\n");
        break;
    case 110:
        return tr("Missing ppDATE specification file.\n")
               + tr("Missing output address.\n");
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
    if (ui->source->text() == "")
        res = res + 1;

    if (ui->ppdate->text() == "")
        res = res + 10;
    if (ui->output->text() == "")
        res = res + 100;
    return res;
}


void StarvoorsGUI::readFromConsole()
{
    QByteArray arr = process->readAllStandardOutput();
    QByteArray err = process->readAllStandardError();
    QByteArray line = arr.simplified();

    if (line == "")
        return;

    if (ui->only_parse->isChecked())
        starvoorsExec = StarvoorsExec::OnlyParsing;

    switch (starvoorsExec) {
    case StarvoorsExec::NoneExec:
        if (line.contains("Setting the taclet")) {
            ui->console->append(tr("Initiating static verification of Hoare triples with KeY."));
            ui->console->append(line);
            starvoorsExec = StarvoorsExec::KeyExec;
            return;
        }
        if (line.contains("DATE Compiled Successfully")) {
            starvoorsExec = StarvoorsExec::FinishedExec;
            if (ui->only_rv->isChecked()) {
                ui->console->append("StaRVOOrS is ran in only runtime verification mode.");
            }
            else {
                ui->console->append("There are no Hoare triples to analyse.");
            }
            ui->console->append("Initiating monitor files generation.");
            return;
        }
        if (line.contains("Welcome to StaRVOOrS")){
            starvoorsExec = StarvoorsExec::LarvaExecErr;
            if (ui->only_rv->isChecked()) {
                ui->console->append("StaRVOOrS is ran in only runtime verification mode.");
            }
            else {
                ui->console->append("There are no Hoare triples to analyse.");
            }
            ui->console->append("Initiating monitor files generation.");
            ui->console->append("Translating ppDATE to DATE.");
            ui->console->append("Translation completed.");
            ui->console->append("Running LARVA...");
            ui->console->append("There was an error when running LARVA.");
            return;
        }
        break;
    case StarvoorsExec::KeyExec:
        if (line.contains("Analysing the Hoare triple")) {
            ui->console->append(line + "\n");
            return;
        }
        if (line.contains("Static verification completed.")) {
            ui->console->append(tr("Static verification completed. See the generated report file for more details.\n"));
            ui->console->append(tr("Generating Java files to control the Hoare triple(s)."));
            starvoorsExec = StarvoorsExec::LarvaExec;
            return;
        }
        if (line.contains("An error has occurred")) {
            ui->console->append(line + "\n");
            starvoorsExec = StarvoorsExec::KeyExecErr;
            return;
        }
        if (line == "Done.") {
            ui->console->append(line + "\n");
            return;
        }
        ui->console->append(line);
        break;
    case StarvoorsExec::KeyExecErr:
        ui->console->append(line + "\n");
        ui->console->append(tr("Generating Java files to control the Hoare triple(s)."));
        starvoorsExec = StarvoorsExec::LarvaExec;
        break;
    case StarvoorsExec::LarvaExec:
        if (line.contains("DATE Compiled Successfully")) {
            starvoorsExec = StarvoorsExec::FinishedExec;
            ui->console->append("Java files generation completed.");
            ui->console->append("Initiating monitor files generation.");
            return;
        }
        if (line.contains("Welcome to StaRVOOrS")){
            starvoorsExec = StarvoorsExec::LarvaExecErr;
            ui->console->append("Java files generation completed.");
            ui->console->append("Initiating monitor files generation.");
            ui->console->append("Translating ppDATE to DATE.");
            ui->console->append("Translation completed.");
            ui->console->append("Running LARVA...");
            ui->console->append("There was an error when running LARVA.");
            return;
        }
        break;
    case StarvoorsExec::LarvaExecErr:
        if (line.contains("Welcome to StaRVOOrS")){
           ui->console->append("Monitor files generation was not successful.");
           return;
        }
        ui->console->append(line);
        break;
    case StarvoorsExec::FinishedExec:
        if (line.contains("Welcome to StaRVOOrS")){
            ui->console->append("Translating ppDATE to DATE.");
            ui->console->append("Translation completed.");
            ui->console->append("Running LARVA...");
            ui->console->append("Monitor files generation completed.");
            return;
        }
        break;
    case StarvoorsExec::OnlyParsing:
        if (line.contains("Welcome to StaRVOOrS")) {
           ui->console->append(arr.remove(0,23));
        }
        break;
    default:
        ui->console->append(line + "\n");
        break;
    }
}


void StarvoorsGUI::on_pushButton_2_clicked()
{
    process->kill();
    ui->pushButton_2->setEnabled(false);
    ui->button_run->setEnabled(true);
    starvoorsExec = StarvoorsExec::NoneExec;
}


void StarvoorsGUI::resetComponents(int exit,QProcess::ExitStatus status)
{
    if (exit == 0 && status == QProcess::NormalExit){
        ui->pushButton_2->setEnabled(false);
        ui->button_run->setEnabled(true);

        switch (starvoorsExec) {
        case StarvoorsExec::FinishedExec:
            ui->console->append("StaRVOOrS has finished successfully.");
            break;
        case StarvoorsExec::OnlyParsing:
            break;
        default:
            ui->console->append("StaRVOOrS has finished abruptly.");
            break;
        }

        starvoorsExec = StarvoorsExec::NoneExec;
    } else {
        ui->pushButton_2->setEnabled(false);
        ui->button_run->setEnabled(true);
        starvoorsExec = StarvoorsExec::NoneExec;
        ui->console->append("StaRVOOrS has finished abruptly.");
    }
}

void StarvoorsGUI::on_only_parse_clicked()
{
    if (ui->only_parse->isChecked()) {
        ui->only_rv->setEnabled(false);
        ui->xml->setEnabled(false);
        ui->none_verbose->setEnabled(false);
    } else {
        ui->only_rv->setEnabled(true);
        ui->xml->setEnabled(true);
        ui->none_verbose->setEnabled(true);
    }
}

void StarvoorsGUI::on_source_returnPressed()
{
    ui->button_run->click();
}

void StarvoorsGUI::on_ppdate_returnPressed()
{
    ui->button_run->click();
}

void StarvoorsGUI::on_output_returnPressed()
{
    ui->button_run->click();
}

void StarvoorsGUI::showAbout()
{
    QPixmap myPixmap(15,15);
    myPixmap.load(QDir::currentPath() + tr("/starvoors_logo.png"));

    QString info = tr("For more information about StaRVOOrS, visit our website :\n\n") + tr("http://cse-212294.cse.chalmers.se/starvoors/");

    QMessageBox msgb (tr("About StaRVOOrS"),
                info,
                QMessageBox::Information,
                QMessageBox::NoButton,
                QMessageBox::NoButton,
                QMessageBox::Close);
    msgb.setIconPixmap(myPixmap.scaled(250,180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgb.exec();
}
