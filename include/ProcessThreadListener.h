#pragma once

#include <QThread>
#include <Windows.h>

class ProcessThreadListener :
    public QThread
{
    DWORD pID;

public:
    ProcessThreadListener(DWORD pid, QObject* parent = Q_NULLPTR);

protected:
    void run() override;

private:
    DWORD waitForProcessStateChange();
    void startNewAppProcess();
};

