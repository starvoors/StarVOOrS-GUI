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
    Q_ENUMS(StarvoorsExec)

public:
    explicit StarvoorsGUI(QWidget *parent = 0);
    ~StarvoorsGUI();
    QProcess *process;

    enum class StarvoorsExec
      {
        NoneExec = 0,
        KeyExec = 1,
        KeyExecErr = 2,
        LarvaExec = 3,
        LarvaExecErr = 4,
        FinishedExec = 5,
        OnlyParsing = 6
      };

private slots:
    QString browseDir(QString s);
    QString browseFile(QString s);
    void readFromConsole();
    void resetComponents(int exit,QProcess::ExitStatus status);

    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();
    void on_button_run_clicked();
    void on_pushButton_2_clicked();
    void on_only_parse_clicked();

    void on_source_returnPressed();
    void on_ppdate_returnPressed();
    void on_output_returnPressed();

    void showAbout();

private:
    Ui::StarvoorsGUI *ui;
    QStringList makeStarvoorsCall(QStringList address);
    bool checkArguments();
    QString exitMessage(int n);
    int argumentsEmpty();
    StarvoorsExec starvoorsExec;
};

#endif // STARVOORSGUI_H
