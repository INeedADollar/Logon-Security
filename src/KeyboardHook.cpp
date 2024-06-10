#include "KeyboardHook.h"
#include <QDebug>


KeyboardHook keyboard_hook;


KeyboardHook::KeyboardHook()
{
	hook = nullptr;
}


bool KeyboardHook::isHookDisabled()
{
	return hook == nullptr;
}


void KeyboardHook::enableHook()
{
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardHook::keyboardHookCallback, 
		GetModuleHandle(0), 0);
}


void KeyboardHook::disableHook()
{
	UnhookWindowsHookEx(hook);
	hook = nullptr;
}


bool KeyboardHook::isKeyAllowed(BYTE key)
{
	return (key >= 0x30 && key <= 0x39) ||
		(key >= 0x41 && key <= 0x5A) ||
		(key == VK_BACK) || (key == VK_RETURN);
}


LRESULT KeyboardHook::keyboardHookCallback(int nCode, WPARAM wp, LPARAM lp)
{
	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lp;
		if (!keyboard_hook.isKeyAllowed(p->vkCode))
		{
			MessageBeep(MB_OK);
			return 1;
		}
	}

	return CallNextHookEx(keyboard_hook.hook, nCode, wp, lp);
}