#include "IconButton.h"


IconButton::IconButton(QWidget* parent, QString text)
	: QPushButton(text, parent)
{
	is_hover_active = false;
}


void IconButton::setNormalIcon(const QIcon& icon)
{
	normal_icon = icon;

	if (!is_hover_active)
		setIcon(icon);
}


void IconButton::setHoverIcon(const QIcon& icon)
{
	hover_icon = icon;

	if (is_hover_active)
		setIcon(icon);
}


void IconButton::setHoverCursor(const QCursor& cursor)
{
	hover_cursor = cursor;
	setCursor(cursor);
}


void IconButton::setNormalTextColor(const QColor& color)
{
	normal_text_color = color;
	
	setStyleSheet(styleSheet() + "QPushButton {color: " + color.name() + ";}"
		"QPushButton:hover {color: " + hover_text_color.name() + ";}"
	);
}


void IconButton::setHoverTextColor(const QColor& color)
{
	setStyleSheet(styleSheet() + "QPushButton {color: " + normal_text_color.name() + ";}"
		"QPushButton:hover {color: " + color.name() + ";}"
	);
}


QIcon& IconButton::getNormalIcon()
{
	return normal_icon;
}


QIcon& IconButton::getHoverIcon()
{
	return hover_icon;
}


QCursor& IconButton::getHoverCursor()
{
	return hover_cursor;
}


QColor& IconButton::getNormalTextColor()
{
	return normal_text_color;
}


QColor& IconButton::getHoverTextColor()
{
	return hover_text_color;
}


void IconButton::enterEvent(QEvent* ev)
{
	is_hover_active = true;

	setIcon(hover_icon);
	setCursor(hover_cursor);
}


void IconButton::leaveEvent(QEvent* ev)
{
	is_hover_active = false;

	setIcon(normal_icon);
}