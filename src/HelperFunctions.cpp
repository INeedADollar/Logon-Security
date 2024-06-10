#include "HelperFunctions.h"

#include <QApplication>

WindowsSemaphore* createAppSemaphore();
QSharedMemory* createAppSharedMemory();



WindowsSemaphore*
app_semaphore = createAppSemaphore();

QSharedMemory* 
app_shared_memory = createAppSharedMemory();




HDESK createLogonDesktop(
	_In_ QString name)
{
	wchar_t* desk_name = new wchar_t[name.size()];
	name.toWCharArray(desk_name);

	HDESK logon_desk = CreateDesktop(desk_name, 0, 0, 0, GENERIC_ALL, 0);
	delete[] desk_name;

	if (!logon_desk)
	{
		return Q_NULLPTR;
	}

	if (!SetThreadDesktop(logon_desk))
	{
		CloseDesktop(logon_desk);
		return Q_NULLPTR;
	}

	return logon_desk;
}


WindowsSemaphore* createAppSemaphore()
{
	QString semaphore_name = createUniqueKey(36, "Logon_Security", 4243, 2373, 8462, 3900, 1215);
	WindowsSemaphore* app_semaphore = new WindowsSemaphore(semaphore_name, 7);

	return app_semaphore;
}


QSharedMemory* createAppSharedMemory()
{
	QString shared_memory_name = createUniqueKey(64, "Logon_Security", 2343, 3243, 5442, 3132, 1245);
	QSharedMemory* app_shared_memory = new QSharedMemory(shared_memory_name);

	if (!app_shared_memory->create(7))
	{
		return Q_NULLPTR;
	}

	return app_shared_memory;
}


QString createUniqueKey(
	_In_ int dec,
	_In_ QString name,
	_In_ int key_id_1,
	_In_ int key_id_2,
	_In_ int key_id_3,
	_In_ int key_id_4,
	_In_ int key_id_5)
{
	QString unique_key;
	unique_key += dec;
	unique_key += dec;
	unique_key += name;
	unique_key += dec;
	unique_key += dec;

	unique_key += 123;
	unique_key += QString::number(key_id_1);
	unique_key += 45;
	unique_key += QString::number(key_id_2);
	unique_key += 45;
	unique_key += QString::number(key_id_3);
	unique_key += 45;
	unique_key += QString::number(key_id_4);
	unique_key += 45;
	unique_key += QString::number(key_id_5);
	unique_key += 123;

	return unique_key;
}


HDESK getMainDesktop()
{
	return OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP);
}


qint64 launchProcesses()
{
	if (!app_shared_memory || 
		(!app_shared_memory->isAttached() 
			&& !app_shared_memory->attach()))
	{
		return 0;
	}

	app_shared_memory->lock();
	qint64* shared_memory_array = static_cast<qint64*>(app_shared_memory->data());
	shared_memory_array[0] = QApplication::applicationPid();

	qint64 current_pid;
	for (int i = 1; i < 7; i++)
	{
		QStringList app_arguments;
		app_arguments << QString("--process_to_watch: %1").arg(i - 1);

		QProcess::startDetached(QApplication::applicationFilePath(), app_arguments, QString(), &current_pid);
		shared_memory_array[i] = current_pid;
	}

	app_shared_memory->unlock();
	return current_pid;
}


bool areAllProcessesRunning()
{
	return (!app_semaphore->acquire());
}


bool findArg(
	_In_ QString argToFind)
{
	QStringList app_arguments = QApplication::arguments();

	for (QString arg : app_arguments)
	{
		if (arg == argToFind)
		{
			return true;
		}
	}

	return false;
}