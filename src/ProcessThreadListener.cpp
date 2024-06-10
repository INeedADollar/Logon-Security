#include "ProcessThreadListener.h"

#include <QApplication>
#include <QProcess>

ProcessThreadListener::ProcessThreadListener(DWORD pid, QObject* parent) :
	QThread(parent), pID(pid)
{

}

void ProcessThreadListener::run()
{
	forever
	{
		DWORD res = waitForProcessStateChange();
		if (res == WAIT_OBJECT_0)
		{
			startNewAppProcess();
		}
	}
}

DWORD ProcessThreadListener::waitForProcessStateChange()
{
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pID);
	if (!process) 
	{
		return FALSE;
	}

	DWORD ret = WaitForSingleObject(process, INFINITE);
	CloseHandle(process);

	return ret;
}

void ProcessThreadListener::startNewAppProcess()
{
	QString appPath = QApplication::applicationFilePath();
	QProcess::startDetached(appPath, QStringList(), "", (qint64*)&pID);
}