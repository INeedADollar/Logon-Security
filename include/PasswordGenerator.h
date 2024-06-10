#pragma once

#include <QString>

class PasswordGenerator
{

public:
	static QString generateUserPassword();
	static QString generateShutdownPassword();
	static QString generateUnlockPassword();
};

