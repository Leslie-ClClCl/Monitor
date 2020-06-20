#include "functional.h"
#include "pwd.h"
#include "unistd.h"

int total,running,sleeping;

int get_proc_info(const char *filename, struct proc_info &info) {
    FILE *fp = fopen(filename, "r");
    QString str;
    char ch;
    if (fp == NULL) return -1;
    // get process's pid
    info.pid.clear();
    while((ch = fgetc(fp)) != ' ')
        info.pid.append(ch);

    // get process's name
    info.name.clear();
    while((ch = getc(fp)) != ' ')
        info.name.append(ch);
    info.name.remove(0,1);
    info.name.remove(info.name.size()-1,1);
    // get process's status
    info.status.clear();
    ch = getc(fp); getc(fp);
    total++;
    switch (ch) {
        case 'R': info.status = "Running"; running++; break;
        case 'I': info.status = "IDLE"; break;
        case 'S': info.status = "Sleeping"; sleeping++; break;
        case 'D': info.status = "Waiting"; break;
        case 'Z': info.status = "Zombie"; break;
        case 'T': info.status = "Stopped"; break;
        case 't': info.status = "Tracing stop"; break;
        case 'X': info.status = "Dead"; break;
        default: info.status = "Unknown";printf("ch = %c\n", ch);
    }

    // get process's ppid
    info.ppid.clear();
    while ((ch = getc(fp)) != ' ')
        info.ppid.append(ch);

    // get process's pgrp
    info.pgrp.clear();
    while ((ch = getc(fp)) != ' ')
        info.pgrp.append(ch);

    int count = 0;
    while(count < 12) {
        while((ch = getc(fp)) != ' ') ;
        count++;
    }

    // get process's priority
    info.priority.clear();
    while((ch = getc(fp)) != ' ')
        info.priority.append(ch);

    count = 0;
    while(count < 5) {
        while((ch = getc(fp)) != ' ') ;
        count++;
    }

    // get process's memory poccession
    while((ch = getc(fp)) != ' ')
        str.append(ch);
    long page_size = sysconf(_SC_PAGE_SIZE);
    double memory = (double)str.toULong() * page_size; // Bytes
    if (memory > 1000) {
        memory /= 1024;
        if (memory > 1000) {
            memory /= 1024;
            info.mem.setNum(memory,'f', 2);
            info.mem += " MB";
        }
        else {
            info.mem.setNum(memory, 'f', 2);
            info.mem += " KB";
        }
    }
    else {
        info.mem.setNum(memory, 'f', 2);
        info.mem += " B";
    }
    fclose(fp);
    return  0;
}

int set_proc_info(MainWindow &w) {
    struct dirent *ptr;
    struct proc_info proc_info;
    DIR *dir = opendir("/proc");
    if (dir == NULL)
        exit(bad_dir);
    char filePath [128];
    w.clearTable1();
    while((ptr = readdir(dir)) != NULL) {
        if ((ptr->d_name[0] >= '0') && (ptr->d_name[0] <= '9')) {
            sprintf(filePath, "/proc/%s/stat", ptr->d_name);
            if (get_proc_info(filePath, proc_info) != 0)
                continue;
            w.addDataToTable1(proc_info.name, proc_info.pid, proc_info.status, proc_info.mem, proc_info.priority);
        }
    }
    closedir(dir);
    if (w.isTable1Focused() != -1) {
        w.setRowFocus(w.isTable1Focused());
    }
    return 0;
}

void get_spec_proc_info(const char *item, QString pid, QString &content) {
    QString filePath = "/proc/" + pid +"/status";
    FILE *fp = fopen(&filePath.toStdString()[0], "r");
    if (fp == NULL) return ;
    char buf[1024], *ptr;
    fread(buf, sizeof(char), sizeof(buf), fp);
    fclose(fp);
    ptr = strstr(buf, item);
    if (ptr == NULL) return;
    while(*ptr++ != ':') ;
    ptr++;
    while(*ptr != '\n' && *ptr != '\t') content.append(*ptr++);
}

void refresh_proc_page(MainWindow &w) {
    total = 0; running = 0; sleeping = 0;
    set_proc_info(w);
    w.setProcOverView(total,running,sleeping);
}

long get_proc_cpu_time(QString pid) {
    QString filePath = "/proc/" + pid + "/stat";
    FILE *fp = fopen(&filePath.toStdString()[0],"r");
    if (fp == NULL) return -1;
    UL stime, utime; QString str;
    char ch; int count = 0;
    while (count++ < 13)
        while((ch = getc(fp)) != ' ') ;

    while ((ch = getc(fp)) != ' ')
        str.append(ch);
    stime = str.toULong();
    str.clear();
    while ((ch = getc(fp)) != ' ')
        str.append(ch);
    utime = str.toULong();
    fclose(fp);
    return stime + utime;
}

double cal_proc_cpu_ratio(QString pid) {
    long hz = sysconf(_SC_CLK_TCK);
    long t1 = get_proc_cpu_time(pid);
    QTime durTime = QTime::currentTime().addMSecs(100);
    while( QTime::currentTime() < durTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    long t2 = get_proc_cpu_time(pid);
    double ratio = (double)(t2 - t1) / hz * 1000;
    return ratio;
}
const char *get_uid_name(int uid){
    struct passwd *info = getpwuid(uid);
    return info->pw_name;
}
