#include "PasswordLineEdit.h"

#include <QKeyEvent>



static QMap<QChar, QChar> char_mapping = {
	{'t', '@'}, {'7', '!'}, {'q', '0'},
	{'p', '@'}, {'g', '#'}, {'0', '$'},
	{'1', '%'}, {'l', '^'}, {'9', '&'},
	{'e', '*'}, {'k', '('}, {'r', ')'},
	{'v', '!'}
};



PasswordLineEdit::PasswordLineEdit(QWidget* parent)
	: QLineEdit(parent), characters_number(0)
{

}


void PasswordLineEdit::keyReleaseEvent(QKeyEvent* ev)
{
	QChar event_character = ev->text()[0];
	if (isAlphanumericCharacter(event_character))
	{
		if (text() == "" || characters_number == 0)
		{
			current_character = event_character;
			characters_number = 1;
		}
		else
		{
			if (current_character == event_character)
			{
				characters_number++;
			}
			else
			{
				current_character = event_character;
				characters_number = 1;
			}

			if (characters_number == 3)
			{
				QString current_text = text();
				int cursor_position = cursorPosition() - 3;

				setText(current_text.replace(cursor_position, 3, char_mapping[current_character]));
				characters_number = 0;
			}
		}
	}
	
	QLineEdit::keyReleaseEvent(ev);
}


bool PasswordLineEdit::isAlphanumericCharacter(QChar character)
{
	return (character >= 'a' && character <= 'z') ||
		(character >= '0' && character <= '9');
}