#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <QString>
#include <QTime>
#include "mainwindow.h"
#include "iostream"

typedef unsigned long UL;
#define file_open_err   1
#define bad_dir         2

enum proc_header {proc_name = 100, proc_pid, proc_status, proc_mem, proc_priority};
struct proc_info {
    QString name;   // The filename of the executable
    QString pid;    // The process ID.
    QString status; // process state
    QString ppid;   // The PID of the parent of this process
    QString pgrp;   // The process group ID of the process
    QString mem;    //
    QString priority;
};

void refresh_proc_page(MainWindow &w);
void get_spec_proc_info(const char *item, QString pid, QString &content);
const char *get_uid_name(int uid);
double cal_proc_cpu_ratio(QString pid);

int get_cpu_usage( struct cpu_usage &cu);
double calculate_cpu_usage();
void show_proc_thread_num();

int show_cpu_info(MainWindow &w);
void show_memory_info(MainWindow &w);

int show_module_info(MainWindow &w);
void show_system_info(MainWindow &w);

#endif // FUNCTIONAL_H
