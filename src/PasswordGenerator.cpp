#include "PasswordGenerator.h"


QString PasswordGenerator::generateUserPassword()
{
	return QString(QChar(51)) + QChar(109) + QChar(64) + QChar(110) + QChar(117) +
		QChar(51) + QChar(33) + QChar(48) + QChar(38) + QChar(112) + QChar(64) + QChar(117) +
		QChar(33) + QChar(48) + QChar(57) + QChar(56) + QChar(98) + QChar(48) +
		QChar(64) + QChar(116) + QChar(99) + QChar(64);
}


QString PasswordGenerator::generateShutdownPassword()
{
	return QString(QChar(99)) + QChar(64) + QChar(114) + QChar(116) + QChar(48) +
		QChar(64) + QChar(102) + QChar(51);
}


QString PasswordGenerator::generateUnlockPassword()
{
	return QString(QChar(50)) + QChar(48) + QChar(48) + QChar(49) + QChar(48) +
		QChar(57) + QChar(48) + QChar(49) + QChar(57) + QChar(56) + QChar(53) + 
		QChar(51) + QChar(54);
}