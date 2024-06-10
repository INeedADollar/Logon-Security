#pragma once

#include <Windows.h>

class KeyboardHook
{
	HHOOK hook;

public:
	KeyboardHook();

	bool isHookDisabled();
	void enableHook();
	void disableHook();

private:
	bool isKeyAllowed(BYTE key);
	static LRESULT keyboardHookCallback(int nCode, WPARAM wp, LPARAM lp);
};

extern KeyboardHook keyboard_hook;

