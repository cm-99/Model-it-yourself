#include "Essentials/corecontroller.h"

#include <QApplication>
#include <QTableView>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Log>("Log");
    qRegisterMetaType<Log>("SignalInfo");
    qRegisterMetaType<Log>("DatasetSection");

    QApplication app_MIY(argc, argv);
    app_MIY.setApplicationName("MIY");
    CoreController *core_controller = new CoreController();
    core_controller->setWindowTitle(app_MIY.applicationName());
    core_controller->show();

    return app_MIY.exec();
}
