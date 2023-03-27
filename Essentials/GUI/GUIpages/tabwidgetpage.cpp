#include "Essentials/GUI/GUIpages/tabwidgetpage.h"

TabWidgetPage::TabWidgetPage(QWidget *parent)
    : QWidget{parent}
{
    connect(this, &TabWidgetPage::signal_relay_log_message, &log_relay, &LogConnectedObject::signal_log_message);
}

LogRelay *TabWidgetPage::get_log_relay()
{
    return &log_relay;
}
