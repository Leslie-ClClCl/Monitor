#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool isRunning();
    void addDataToTable1(QString name, QString pid, QString status, QString mem, QString priority);
    void setProcOverView(int total, int running, int sleeping);
    void showCpuUsage(double cpu_usage);
    void showCpuInfo(QString, QString, QString, QString,int,int);
    void showModuleInfo(QString module_name,QString module_size,QString ins_num);
    void showModuleInfoRight(QString module_name,QString module_size,QString ins_num,QString dependent,QString state);
    void clearTable1();
    void clearModuleTable();
    int isTable1Focused();
    int isModuleTableFocused();
    void refreshProcInfo();
    void setRowFocus(int row);
    void setMemoryInfo(long total, long free);
    void setSystemInfo(QString kernel, QString gcc, QString build);
    ~MainWindow();

protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void on_kill_clicked();
    void on_table1_itemClicked(QTableWidgetItem *item);
    void on_refresh_clicked();

    void on_module_table_itemClicked(QTableWidgetItem *item);

    void on_uninstall_module_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString pid_showing;
    bool running;
    int row_focused_table1;
    int row_focused_module;
    QString module_name_showing;
    void table1Init();
    void moduleTableInit();
};
#endif // MAINWINDOW_H
