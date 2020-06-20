#include "functional.h"

struct module_info{
    QString module_name;
    QString module_size;
    QString cited;
    QString dependencies;
    QString state;
};

int show_module_info(MainWindow &w) {
    FILE *fp = fopen("/proc/modules", "r");
    QString str;
    struct module_info mi;
    char ch;
    if (fp == NULL)
        return -1;
    w.clearModuleTable();
    do{
        mi.module_name.clear();mi.module_size.clear();mi.cited.clear();
        mi.dependencies.clear();mi.state.clear();
        while((ch = getc(fp)) != ' ' && ch != EOF)
            mi.module_name.append(ch);
        if (ch == EOF)
            break;
        while((ch = getc(fp)) != ' ')
            mi.module_size.append(ch);

        while((ch = getc(fp)) != ' ')
            mi.cited.append(ch);

        while((ch = getc(fp)) != ' ')
            mi.dependencies.append(ch);

        while((ch = getc(fp)) != ' ')
            mi.state.append(ch);
        while((ch = getc(fp)) != '\n')
            ;
        w.showModuleInfo(mi.module_name,mi.module_size,mi.cited);
    }while(!feof(fp));
    fclose(fp);
    return 0;
}
