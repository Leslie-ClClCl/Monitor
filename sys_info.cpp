#include "functional.h"

void get_system_info (QString &kernel_version,QString &gcc_version,QString &build_time) {
    const char filePath[] = "/proc/version";
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) exit(file_open_err);
    char buf[256], *ptr;
    fread(buf,sizeof(char),sizeof(buf),fp);
    fclose(fp);
    ptr = buf + 14;
    int count = 0;
    while (count++ < 3) {
        while(*ptr != ' ')
            kernel_version.append(*ptr++);
        kernel_version.append(' ');
    }
    ptr = strstr(buf, "gcc version") + 12;
    count = strstr(buf, " #") - ptr;
    while (count--)
        gcc_version.append(*ptr++);
    while (*ptr != 0) { ptr++;}
    count = 0; ptr -= 3;
    while (count++ < 6) {
        while(*ptr != ' ')
            build_time = *ptr-- + build_time;
        build_time = *ptr-- + build_time;
    }
    build_time.remove(0,1);
}

void show_system_info(MainWindow &w) {
    QString kernel_version, gcc_version, build_time;
    get_system_info(kernel_version, gcc_version, build_time);
    w.setSystemInfo(kernel_version, gcc_version, build_time);
}
