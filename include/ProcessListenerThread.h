#pragma once

#include <Windows.h>

#include <QThread>
#include <QRegularExpression>

class ProcessListenerThread 
    : public QThread
{
    Q_OBJECT

    DWORD process_id;

public:
    ProcessListenerThread(DWORD pid, QObject* parent = Q_NULLPTR);
    static int findProcessIndex(QRegularExpression exp);

protected:
    void run() override;

private:
    DWORD waitForProcessStateChange();
    void startNewAppProcess();
};

