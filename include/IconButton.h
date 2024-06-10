#pragma once

#include <QPushButton>

class IconButton :
    public QPushButton
{
    Q_OBJECT

    QCursor hover_cursor;

    QIcon normal_icon;
    QIcon hover_icon;

    QColor normal_text_color;
    QColor hover_text_color;

    bool is_hover_active;

public:
    IconButton(QWidget* parent = Q_NULLPTR, QString text = QString());

    void setNormalIcon(const QIcon& icon);
    void setHoverIcon(const QIcon& icon);
    void setHoverCursor(const QCursor& cursor);
    void setNormalTextColor(const QColor& color);
    void setHoverTextColor(const QColor& color);

    QIcon& getNormalIcon();
    QIcon& getHoverIcon();

    QCursor& getHoverCursor();
    QColor& getNormalTextColor();
    QColor& getHoverTextColor();


protected:
    void enterEvent(QEvent* ev) override;
    void leaveEvent(QEvent* ev) override;
};

