#ifndef STARVOORSGUI_H
#define STARVOORSGUI_H

#include <QWidget>

namespace Ui {
class StarvoorsGUI;
}

class StarvoorsGUI : public QWidget
{
    Q_OBJECT

public:
    explicit StarvoorsGUI(QWidget *parent = 0);
    ~StarvoorsGUI();

private slots:
    QString browseDir(QString s);
    QString browseFile(QString s);

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_button_run_clicked();

private:
    Ui::StarvoorsGUI *ui;
    QString makeStarvoorsCall(QString address);
    bool checkArguments();
    QString exitMessage(int n);
    int argumentsEmpty();
};

#endif // STARVOORSGUI_H
