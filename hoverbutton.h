#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QColor>

class HoverButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ bgColor   WRITE setBgColor)
public:

    HoverButton(QWidget* parent = nullptr);

    QColor bgColor() const {
        return m_color;
    }
    void setBgColor(const QColor& color);
protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
private:
    QColor m_color;
    QPropertyAnimation* animation;
};

#endif // HOVERBUTTON_H
