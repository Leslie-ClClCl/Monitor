#include "functional.h"

void get_memory_info(const char *item, QString &str) {
    const char filePath[] = "/proc/meminfo";
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) exit(file_open_err);
    char buf[512], *ptr;
    fread(buf, sizeof(char), sizeof(buf), fp);
    fclose(fp);
    ptr = strstr(buf, item);
    while(*ptr != ':') ptr++; ptr++;
    while(*ptr == ' ') ptr++;
    while(*ptr != ' ')
        str.append(*ptr++);
}

void show_memory_info(MainWindow &w) {
    QString total,free;
    get_memory_info("MemTotal", total);
    get_memory_info("MemFree", free);
    w.setMemoryInfo(total.toLong(), free.toLong());
}
