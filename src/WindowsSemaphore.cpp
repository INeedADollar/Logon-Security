#include "WindowsSemaphore.h"



WindowsSemaphore::WindowsSemaphore(QString sem_name, int sem_count)
{
	wchar_t* semaphore_name = new wchar_t[sem_name.size()];
	sem_name.toWCharArray(semaphore_name);

	semaphore = CreateSemaphore(NULL, sem_count, sem_count, semaphore_name);

	if (!semaphore)
	{
		semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, semaphore_name);
	}

	delete[] semaphore_name;
}


WindowsSemaphore::~WindowsSemaphore()
{
	release();
	CloseHandle(semaphore);
}


bool WindowsSemaphore::isValid()
{
	return (semaphore != NULL);
}


bool WindowsSemaphore::acquire()
{
	if (!isValid())
	{
		return false;
	}

	DWORD return_value = WaitForSingleObject(semaphore, 0L);
	return (return_value != WAIT_TIMEOUT);
}


bool WindowsSemaphore::release()
{
	if (!isValid())
	{
		return false;
	}

	return (ReleaseSemaphore(semaphore, 1, NULL) != 0);
}