#pragma once

#include "WindowsSemaphore.h"

#include <QSharedMemory>
#include <QProcess>


HDESK createLogonDesktop(
	_In_ QString name
);


HDESK getMainDesktop();
qint64 launchProcesses();
bool areAllProcessesRunning();


QString createUniqueKey(
	_In_ int dec,
	_In_ QString name,
	_In_ int key_id_1,
	_In_ int key_id_2,
	_In_ int key_id_3,
	_In_ int key_id_4,
	_In_ int key_id_5
);


bool findArg(
	_In_ QString argToFind
);


extern WindowsSemaphore* app_semaphore;
extern QSharedMemory* app_shared_memory;