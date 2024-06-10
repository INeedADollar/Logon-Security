#pragma once

#include "KeyboardHook.h"
#include "IconButton.h"
#include "PasswordLineEdit.h"

#include <QLabel>
#include <QLineEdit>
#include <QCamera>
#include <QCameraImageCapture>

class LogonWindow :
    public QWidget
{
    Q_OBJECT

    QLabel* window_background;
    QLabel* user_name;
    PasswordLineEdit* computer_password;
    IconButton* reveal_password_button;
    IconButton* continue_button;
    IconButton* shutdown_button;

    int enter_tries;

    QCamera* computer_camera;
    QCameraImageCapture* camera_capture;

public:
    LogonWindow(QWidget* parent = Q_NULLPTR);

    enum LogonWindowState
    {
        LockState,
        UnlockState
    };

    private: LogonWindowState session_state;

protected:
    void resizeEvent(QResizeEvent* ev) override;

private:
    void createUI();
    TCHAR* getCurrentUserName();
    void setupCamera();
    void setupRegistries();

signals:
    void logonPassed();

private slots:
    void showPassword();
    void askBeforeShutdown();
    void shutdownComputer();
    void checkPassword();
    void blockComputerAccess();
    void removeLineEditSelection();
    void readyForCapture(QCamera::State status);
    void imageSaved(int id, const QString& file_name);
};

