#ifndef LOGRELAY_H
#define LOGRELAY_H

#include "Interfaces/logconnectedobject.h"

/**
 * Basic implementation of LogConnectedObject.
 * Used primarily as a way to solve QObject multi inheritance problem in TabWidgetPage.
 * Instead of inheriting LogConnectedObject, LogRelay is used to send logs.
 */
class LogRelay : public LogConnectedObject
{
    Q_OBJECT

public:
    LogRelay();
};

#endif // LOGRELAY_H
