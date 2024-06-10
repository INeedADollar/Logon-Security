#pragma once

#include "LogonWindow.h"

#include <QWidget>

class EventsWindow :
    public QWidget
{
    Q_OBJECT

    LogonWindow* current_window;
    HDESK main_desktop;
    HDESK logon_desktop;

public:
    EventsWindow(HDESK main_desk, HDESK logon_desk, QWidget* parent = Q_NULLPTR);

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
    void drawLogonDesktopBackground();

private slots:
    void switchToMainDesktop();
};

