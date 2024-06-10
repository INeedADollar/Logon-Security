#include "ProcessListenerThread.h"
#include "HelperFunctions.h"

#include <QApplication>
#include <QProcess>
#include <QSharedMemory>
#include <QDebug>

ProcessListenerThread::ProcessListenerThread(DWORD pid, QObject* parent) :
	QThread(parent), process_id(pid)
{

}


int ProcessListenerThread::findProcessIndex(QRegularExpression argument_expression)
{
	QStringList app_arguments = QApplication::arguments();

	for (QString arg : app_arguments)
	{
		QRegularExpressionMatch expression_match = argument_expression.match(arg);

		if (expression_match.hasMatch())
		{
			return expression_match.captured(1).toInt();
		}
	}

	return -1;
}


void ProcessListenerThread::run()
{
	forever
	{
		DWORD return_value = waitForProcessStateChange();
		if (return_value == WAIT_OBJECT_0)
		{
			startNewAppProcess();
		}
	}
}


DWORD ProcessListenerThread::waitForProcessStateChange()
{
	HANDLE watched_process = OpenProcess(SYNCHRONIZE, FALSE, process_id);
	if (!watched_process) 
	{
		return FALSE;
	}

	DWORD return_value = WaitForSingleObject(watched_process, INFINITE);
	CloseHandle(watched_process);

	return return_value;
}


void ProcessListenerThread::startNewAppProcess()
{
	QRegularExpression argument_expression("--process_to_watch: \\d");
	QStringList app_arguments;

	int process_index = findProcessIndex(argument_expression);
	QString appPath = QApplication::applicationFilePath();

	qDebug() << process_index;
	if (process_index >= 0 && process_index <= 5)
	{
		if (process_index > 0)
		{
			app_arguments << QString("--process_to_watch: %1").arg(process_index - 1);
		}
		else
		{
			app_arguments << "--authorized_open";
		}
	}
	else
	{
		app_arguments << QString("--process_to_watch: %1").arg(5);
		process_index = 6;
	}

	QProcess::startDetached(appPath, app_arguments, QString(), (qint64*)&process_id);

	if (!app_shared_memory->isAttached() && !app_shared_memory->attach())
	{
		return;
	}

	app_shared_memory->lock();
	static_cast<qint64*>(app_shared_memory->data())[process_index] = process_id;
	app_shared_memory->unlock();
}