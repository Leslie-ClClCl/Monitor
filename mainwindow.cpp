#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "string"
#include <pwd.h>
#include <iostream>
#include "functional.h"
#include <unistd.h>
#include <pwd.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    running = 1;
    table1Init();
    moduleTableInit();
    ui->cpu_usage->setRange(0,100);
    ui->kill->setEnabled(false);
    ui->uninstall_module->setEnabled(false);
    row_focused_table1 = -1;
}

bool MainWindow::isRunning() {
    return running;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::table1Init() {
    ui->table1->setShowGrid(false);
    ui->table1->verticalHeader()->setVisible(false);
    ui->table1->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table1->setStyleSheet("selection-background-color:rgb(176,196,222);");
    ui->table1->verticalHeader()->setDefaultSectionSize(10);
    ui->table1->setAlternatingRowColors(true);
    ui->table1->setColumnWidth(0,140);
    ui->table1->setColumnWidth(1,55);
    ui->table1->setColumnWidth(2,85);
    ui->table1->setColumnWidth(3,100);
    ui->table1->setColumnWidth(4,40);
}

void MainWindow::addDataToTable1(QString name, QString pid, QString status, QString mem, QString priority) {
    int curRow = ui->table1->rowCount();//当前行号
    ui->table1->insertRow(curRow);//在表格尾部添加一行
    ui->table1->setItem(curRow, 0, new QTableWidgetItem(name));
    ui->table1->setItem(curRow, 1, new QTableWidgetItem(pid));
    ui->table1->setItem(curRow, 2, new QTableWidgetItem(status));
    ui->table1->setItem(curRow, 3, new QTableWidgetItem(mem));
    ui->table1->setItem(curRow, 4, new QTableWidgetItem(priority));
}

void MainWindow::setProcOverView(int total, int running, int sleeping) {
    ui->total->setNum(total);
    ui->running->setNum(running);
    ui->sleeping->setNum(sleeping);
}
void MainWindow::clearTable1() {
    ui->table1->clearContents();
    ui->table1->setRowCount(0);
}
void MainWindow::showCpuUsage(double cpu_usage) {
    ui->cpu_usage->setValue(cpu_usage);
}

void MainWindow::showCpuInfo(QString model_name, QString cpu_mhz, QString cache_size, QString cpu_cores, int p,int t) {
    ui->model_name->setText(model_name);
    ui->cpu_mhz->setText(cpu_mhz);
    ui->cache_size->setText(cache_size);
    ui->cpu_cores->setText(cpu_cores);
    ui->thread_count->setNum(t);
    ui->proc_count->setNum(p);
}

void MainWindow::showModuleInfo(QString module_name,QString module_size,QString ins_num) {
    int curRow = ui->module_table->rowCount();
    ui->module_table->insertRow(curRow);//在表格尾部添加一行
    ui->module_table->setItem(curRow, 0, new QTableWidgetItem(module_name));
    ui->module_table->setItem(curRow, 1, new QTableWidgetItem(module_size));
    ui->module_table->setItem(curRow, 2, new QTableWidgetItem(ins_num));
}

void MainWindow::clearModuleTable() {
    ui->module_table->clearContents();
    ui->module_table->setRowCount(0);
}

void MainWindow::moduleTableInit() {
    ui->module_table->setShowGrid(false);
    ui->module_table->verticalHeader()->setVisible(false);
    ui->module_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->module_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->module_table->setStyleSheet("selection-background-color:rgb(176,196,222);");
    ui->module_table->verticalHeader()->setDefaultSectionSize(10);
    ui->module_table->setAlternatingRowColors(true);
    ui->module_table->setColumnWidth(0,180);
    ui->module_table->setColumnWidth(1,100);
    ui->module_table->setColumnWidth(2,85);
    ui->module_table->setColumnWidth(3,150);
    ui->module_table->setColumnWidth(4,50);
}

void MainWindow::on_kill_clicked()
{
    QString cmd = "kill " + pid_showing;
    system(&cmd.toStdString()[0]);
    ui->pid->clear();   ui->proc_group->clear();
    ui->ppid->clear();  ui->user->clear();
    ui->proc_cpu->clear();
    row_focused_table1 = -1;
}

void MainWindow::on_module_table_itemClicked(QTableWidgetItem *item)
{
    ui->uninstall_module->setEnabled(true);
    int row = item->row();
    QString module_name,module_size,dependent,ins_num,module_state;
    FILE *fp = fopen("/proc/modules", "r");
    if (fp == NULL)
        exit(file_open_err);
    char buf[4096], *ptr;
    fread(buf,sizeof(char),sizeof(buf),fp);
    fclose(fp);
    module_name = ui->module_table->item(row,0)->text();
    ptr = strstr(buf,&module_name.toStdString()[0]);
    while(*ptr != ' ') ptr++;
    ptr++;
    while(*ptr != ' ') module_size.append(*ptr++);
    module_size = QString::number((double)module_size.toInt() / 1024, 'f', 2) + " KB (" + module_size + "bytes)";
    ptr++;
    while(*ptr != ' ') ins_num.append(*ptr++);
    ptr++;
    while(*ptr != ' ') dependent.append(*ptr++);
    ptr++;
    while(*ptr != ' ') module_state.append(*ptr++);
    ptr++;
    showModuleInfoRight(module_name,module_size,ins_num,dependent,module_state);
}

void MainWindow::showModuleInfoRight(QString module_name, QString module_size, QString ins_num, QString dependent, QString state) {
    ui->module_name->setText(module_name);
    module_name_showing = module_name;
    ui->module_size->setText(module_size);
    ui->ins_num->setText(ins_num);
    ui->dependent->setWordWrap(true);
    dependent.replace(",", "  ");
    ui->dependent->setText(dependent);
    ui->dependent->adjustSize();
    ui->module_state->setText(state);
}
void MainWindow::on_table1_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    QString pid = ui->table1->item(row,1)->text();
    pid_showing = pid; row_focused_table1 = row;
    QString name = ui->table1->item(row,0)->text();
    QString filepath = "/proc/" + pid + "/status";
    ui->kill->setEnabled(true);
    QString ppid,uid,ppid_name,user_name,tgid,tgid_name;
    get_spec_proc_info("PPid", pid, ppid);
    get_spec_proc_info("Name", ppid, ppid_name);
    get_spec_proc_info("Uid", pid, uid);
    get_spec_proc_info("Tgid", pid, tgid);
    get_spec_proc_info("Name",tgid,tgid_name);
    user_name = get_uid_name(uid.toInt());
    ui->proc_cpu->setText("-");
    ui->pid->setText(name + " (" + pid + ")");          // set pid
    ui->ppid->setText(ppid_name + " (" + ppid + ")");   // set ppid
    ui->proc_group->setText(tgid_name + " (" + tgid + ")");
    ui->user->setText(user_name + " (" + uid + ")");    // set user
    double cpu_ratio = cal_proc_cpu_ratio(pid);
    ui->proc_cpu->setNum(cpu_ratio);                    // set cpu ratio
}

int MainWindow::isTable1Focused() {
    return row_focused_table1;
}

void MainWindow::refreshProcInfo() {
    QTableWidgetItem *item = ui->table1->item(row_focused_table1, 0);
    emit ui->table1->itemClicked(item);
}

void MainWindow::on_refresh_clicked()
{
    refresh_proc_page(*this);
}
void MainWindow::setRowFocus(int row) {
    ui->table1->selectRow(row);
}

void MainWindow::setMemoryInfo(long total, long free) {
    QString total_s = QString::number((double)total/1024,'f', 2) + " MB";
    QString free_s = QString::number((double)free/1024, 'f', 2) + " MB";
    ui->mem_total->setText(total_s);
    ui->mem_free->setText(free_s);
    ui->memory_usage->setRange(0,total);
    ui->memory_usage->setValue(total-free);
}

void MainWindow::setSystemInfo(QString kernel, QString gcc, QString build) {
    ui->kernel_version->setText(kernel);
    ui->gcc_version->setText(gcc);
    ui->build_date->setText(build);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    running = 0;
}

void MainWindow::on_uninstall_module_clicked()
{
    QString cmd = "rmmod " + module_name_showing;
    if(system(&cmd.toStdString()[0]) == -1)
        exit(111);
}

void MainWindow::on_pushButton_clicked()
{
    QString cmd = "insmod ~/Downloads/linux-4.15.2/drivers/misc/my_driver.ko";
    if(system(&cmd.toStdString()[0]) == -1)
        exit(111);
}
