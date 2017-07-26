#ifndef STARVOORSGUI_H
#define STARVOORSGUI_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class StarvoorsGUI;
}

class StarvoorsGUI : public QWidget
{
    Q_OBJECT

public:
    explicit StarvoorsGUI(QWidget *parent = 0);
    ~StarvoorsGUI();
    QProcess *process;

private slots:
    QString browseDir(QString s);
    QString browseFile(QString s);
    void readFromConsole();
    void disableComponents(int exit,QProcess::ExitStatus status);

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_button_run_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StarvoorsGUI *ui;
    QStringList makeStarvoorsCall(QStringList address);
    bool checkArguments();
    QString exitMessage(int n);
    int argumentsEmpty();
};

#endif // STARVOORSGUI_H
