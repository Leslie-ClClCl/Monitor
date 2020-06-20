#include "functional.h"
#include <unistd.h>

struct cpu_usage {
    UL user;    UL nice;    UL system;
    UL idle;    UL iowait;  UL irq;
    UL softirq; UL steal;   UL guest;
    UL guest_nice;
};

struct cpu_info {
    QString model_name;
    QString cpu_mhz;
    QString cache_size;
    QString cpu_cores;
};

int get_cpu_usage(struct cpu_usage &cu) {
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL)
        return -1;
    fscanf(fp, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           &cu.user,&cu.nice,&cu.system,&cu.idle,&cu.iowait,
           &cu.irq,&cu.softirq,&cu.steal,&cu.guest,&cu.guest_nice);
    fclose(fp);
    return 0;
}

double calculate_cpu_usage(){
    long totalCpuTime,idleTime;
    double ratio;
    struct cpu_usage cu;
    if (get_cpu_usage(cu) != 0)
        return -1;
    totalCpuTime = cu.user+cu.nice+cu.system+cu.idle+
            cu.iowait+cu.irq+cu.softirq+cu.steal+cu.guest+cu.guest_nice;
    idleTime = cu.idle;

    QTime dieTime = QTime::currentTime().addMSecs(1000);
    while( QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (get_cpu_usage(cu) != 0)
        return -1;
    totalCpuTime = cu.user+cu.nice+cu.system+cu.idle+
            cu.iowait+cu.irq+cu.softirq+cu.steal+cu.guest+cu.guest_nice - totalCpuTime;
    idleTime = cu.idle - idleTime;
    ratio = 100 * ((double)totalCpuTime - idleTime)/totalCpuTime;
    return ratio;
}

int get_cpu_info(struct cpu_info &ci) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL)
        return -1;
    char buf[2048], *ptr = NULL;
    fread(buf, sizeof(char), sizeof(buf), fp);
    fclose(fp);
    ci.model_name.clear();
    ci.cpu_mhz.clear();
    ci.cache_size.clear();
    ci.cpu_cores.clear();
    if((ptr = strstr(buf, "model name")) == NULL) return -1;
    while(*ptr != ':') {ptr++;}
    while(*++ptr != '\n') ci.model_name.append(*ptr);

    if((ptr = strstr(buf, "cpu MHz")) == NULL) return -1;
    while(*ptr != ':') ptr++;
    while(*++ptr != '\n') ci.cpu_mhz.append(*ptr);

    if((ptr = strstr(buf, "cache size")) == NULL) return -1;
    while(*ptr != ':') ptr++;
    while(*++ptr != '\n') ci.cache_size.append(*ptr);

    if((ptr = strstr(buf, "cpu cores")) == NULL) return -1;
    while(*ptr != ':') ptr++;
    while(*++ptr != '\n') ci.cpu_cores.append(*ptr);
    return 0;
}

long get_proc_thread_num(const char* filePath) {
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        int err = errno;
        std::cout << "err code:" << err << std::endl;
    }
    QString str;
    char ch; int count = 0;
    while (count++ < 19)
        while ((ch = getc(fp)) != ' ');
    while ((ch = getc(fp)) != ' ')
        str.append(ch);
    fclose(fp);
    return str.toLong();
}
void get_total_thread_num(long &proc_num, long &thread_num) {
    DIR *dir = opendir("/proc");
    struct dirent *ptr;
    if (dir == NULL)
        exit(bad_dir);
    char filePath [128];
    proc_num = 0; thread_num = 0;
    while((ptr = readdir(dir)) != NULL) {
        if ((ptr->d_name[0] >= '0') && (ptr->d_name[0] <= '9')) {
            sprintf(filePath, "/proc/%s/stat", ptr->d_name);
            thread_num += get_proc_thread_num(filePath);
            proc_num++;
        }
    }
    closedir(dir);
}

int show_cpu_info(MainWindow &w){
    struct cpu_info ci;
    if(get_cpu_info(ci) != 0)
        return -1;
    long proc_num, thread_num;
    get_total_thread_num(proc_num,thread_num);
    w.showCpuInfo(ci.model_name, ci.cpu_mhz, ci.cache_size, ci.cpu_cores,proc_num,thread_num);
    return 0;
}
