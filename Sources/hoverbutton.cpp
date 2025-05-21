#include "../Headers/hoverbutton.h"
#include <QStyle>
#include <QPainter>

HoverButton::HoverButton(QWidget* parent)
    : QPushButton(parent), m_color(QColor("#F1A661")) {

    animation = new QPropertyAnimation(this, "bgColor", this);
    animation->setDuration(300);
    setStyleSheet("border: none;");
}


void HoverButton::setBgColor(const QColor& color) {
    m_color = color;

    QString style = QString("background-color: %1").arg(color.name());
    setStyleSheet(style);
}

void HoverButton::enterEvent(QEnterEvent* event) {
    animation->stop();
    animation->setStartValue(m_color);
    animation->setEndValue(QColor("#E38B29"));
    animation->start();
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent* event) {
    animation->stop();
    animation->setStartValue(m_color);
    animation->setEndValue(QColor("#F1A661"));
    animation->start();
    QPushButton::leaveEvent(event);
}
