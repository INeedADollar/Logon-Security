#include "EventsWindow.h"

#include <wtsapi32.h>
#include <QTimer>
#include <QScreen>
#include <QApplication>
#include <QtWin>

EventsWindow::EventsWindow(HDESK main_desk, HDESK logon_desk, QWidget* parent)
	: QWidget(parent), current_window(nullptr), main_desktop(main_desk),
		logon_desktop(logon_desk)
{
	WTSRegisterSessionNotification(HWND(winId()), NOTIFY_FOR_ALL_SESSIONS);
	drawLogonDesktopBackground();
}


void EventsWindow::drawLogonDesktopBackground()
{
	HDC desktop_device_context = GetDC();
	Rectangle(desktop_device_context, 0, 0, 1920, 1080);
	HDC bitmap_device_context = CreateCompatibleDC(desktop_device_context);

	HBITMAP desktop_background = QtWin::toHBITMAP(QPixmap(":/images/background.jpg"));
	HGDIOBJ old_bitmap = SelectObject(bitmap_device_context, desktop_background);
	SelectObject(bitmap_device_context, old_bitmap);

	BITMAP gdip_bitmap;
	GetObject(desktop_background, sizeof(gdip_bitmap), &gdip_bitmap);

	BitBlt(desktop_device_context, 0, 0, gdip_bitmap.bmWidth, gdip_bitmap.bmHeight, bitmap_device_context, 0, 0, SRCCOPY);

	SelectObject(bitmap_device_context, old_bitmap);
	DeleteDC(bitmap_device_context);
}

void EventsWindow::switchToMainDesktop()
{
	SwitchDesktop(main_desktop);
}


bool EventsWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	MSG* msg = (MSG*)message;
	if (msg->message == WM_WTSSESSION_CHANGE)
	{
		if (msg->wParam == WTS_SESSION_LOGON || msg->wParam == WTS_SESSION_UNLOCK
			|| msg->wParam == WTS_CONSOLE_CONNECT || msg->wParam == WTS_REMOTE_CONNECT
			|| msg->wParam == WTS_SESSION_REMOTE_CONTROL)
		{

			if (current_window != nullptr)
			{
				delete current_window;
			}

			current_window = new LogonWindow;
			SwitchDesktop(logon_desktop);
			current_window->show();

			current_window->resize(QApplication::primaryScreen()->size());
			current_window->move(0, 0);
			QObject::connect(current_window, &LogonWindow::logonPassed, this, &EventsWindow::switchToMainDesktop);

			QTimer::singleShot(10, [=]() {

			});

		}
	}

	return false;
}