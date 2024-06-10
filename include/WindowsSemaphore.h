#pragma once

#include <Windows.h>

#include <QString>

class WindowsSemaphore
{
	HANDLE semaphore;

public:
	WindowsSemaphore(QString sem_name, int sem_count);
	~WindowsSemaphore();

	bool isValid();
	bool acquire();
	bool release();
};

