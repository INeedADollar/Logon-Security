#include "EventsWindow.h"
#include "ProcessListenerThread.h"
#include "HelperFunctions.h"

#include <QApplication>
#include <QRegularExpression>
#include <QFile>


int main(int argc, char* argv[])
{
	HDESK main_desktop = getMainDesktop();
	HDESK logon_desktop = createLogonDesktop("LOGON_SECURITY_DESKTOP");
	qDebug() << logon_desktop;

	QApplication main_application(argc, argv);
	QRegularExpression argument_expression("--process_to_watch: \\d");
	int process_id = ProcessListenerThread::findProcessIndex(argument_expression);

	if (process_id >= 0 && process_id <= 5)
	{
		if (app_shared_memory->isAttached() || app_shared_memory->attach())
		{
			app_shared_memory->lock();
			
			qint64* shared_memory_array = static_cast<qint64*>(app_shared_memory->data());
			ProcessListenerThread* thread = new ProcessListenerThread(shared_memory_array[process_id]);
			thread->start();
			
			app_shared_memory->unlock();
		}
	}
	else
	{
		if (!areAllProcessesRunning())
		{
			qint64 process_to_watch = 0;
			
			if (findArg("--authorized_open"))
			{
				if (app_shared_memory->isAttached() || app_shared_memory->attach())
				{
					app_shared_memory->lock();

					qint64* shared_memory_array = static_cast<qint64*>(app_shared_memory->data());
					process_to_watch = shared_memory_array[6];
					
					app_shared_memory->unlock();
				}
			}
			else
			{
				process_to_watch = 0;//launchProcesses();
			}
			
			//ProcessListenerThread* thread = new ProcessListenerThread(process_to_watch);
			//thread->start();

			EventsWindow* events_window = new EventsWindow(main_desktop, logon_desktop);
		}
		else
		{
			return 0;
		}
	}

	return main_application.exec();
}