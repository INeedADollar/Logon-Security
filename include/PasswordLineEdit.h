#pragma once

#include <QLineEdit>
#include <QMap>

class PasswordLineEdit : 
	public QLineEdit
{
	Q_OBJECT

	int characters_number;	
	QChar current_character;

public:
	PasswordLineEdit(QWidget* parent = Q_NULLPTR);

protected:
	void keyReleaseEvent(QKeyEvent* ev) override;

private:
	bool isAlphanumericCharacter(QChar ch);
};

