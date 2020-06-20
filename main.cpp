#include <functional.h>
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    while(w.isRunning()) {
        refresh_proc_page(w);
        w.showCpuUsage(calculate_cpu_usage());
        show_cpu_info(w);
        show_module_info(w);
        show_memory_info(w);
        show_system_info(w);
        QTime dieTime = QTime::currentTime().addMSecs(2000);
        while(QTime::currentTime() < dieTime && w.isRunning())
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    return 0;
}
