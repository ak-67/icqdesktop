#include "stdafx.h"
#include "PushButton_t.h"
#include "NameAndStatusWidget.h"

Ui::PushButton_t::PushButton_t(QWidget* parent/* = NULL*/) 
    : QPushButton(parent)
    , fromIconToText_(0)
    , iconW_(-1)
    , alignment_(Qt::AlignVCenter | Qt::AlignLeft)
    , iconH_(-1)
    , postfixColor_(0, 0, 0, 255)
    , currentState_(normal) 
{
    for (size_t ix = 0; ix < sizeof(colorsForStates_) / sizeof(colorsForStates_[0]); ++ix) {
        QColor& color = colorsForStates_[ix];
        color.setAlpha(0);
    }
}

void Ui::PushButton_t::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
}

void Ui::PushButton_t::setColorForState(const eButtonState state, const QColor& color)
{
    colorsForStates_[state] = color;
}

void Ui::PushButton_t::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QRect rc = rect();
    const QPixmap& icon   = !bitmapsForStates_[currentState_].isNull() ? bitmapsForStates_[currentState_] : bitmapsForStates_[normal];
    const QColor& color   = colorsForStates_[currentState_];

    const QString& prefix  = prefix_;
    const QString& postfix = postfix_; 

    QFontMetrics fm(font());
    const int offsetPrefix  = ((!icon.isNull()) & (!prefix.isEmpty()))  * fromIconToText_;
    const int offsetPostfix = ((!icon.isNull()) & (!postfix.isEmpty())) * fromIconToText_;

    const int iconW = (iconW_ >= 0 ? iconW_ : icon.width())  * !icon.isNull();
    const int iconH = (iconH_ >= 0 ? iconH_ : icon.height()) * !icon.isNull();

    const int prefixW  = !prefix.isEmpty() ? fm.width(prefix) : 0;

    int desiredWidth = precalculateWidth();

    int desiredHeight = 0;
    desiredHeight = std::max(iconH, desiredHeight);
    desiredHeight = std::max(!prefix.isEmpty() ? fm.height()   : 0, desiredHeight);
    desiredHeight = std::max(!postfix.isEmpty() ? fm.height()   : 0, desiredHeight);

    const int actualWidth  = std::min(rc.width(),  desiredWidth);
    const int actualHeight = std::min(rc.height(), desiredHeight);

    QRect rcDraw(rc.left(), rc.top(), actualWidth, actualHeight);
    if (Qt::AlignHCenter & alignment_) {
        rcDraw.moveTo(rc.center().x() - actualWidth*0.5f, rcDraw.top());
    } else if (Qt::AlignRight & alignment_) {
        rcDraw.moveTo(rc.right() - actualWidth, rcDraw.top());
    }

    if (Qt::AlignVCenter & alignment_) {
        rcDraw.moveTo(rcDraw.left(), rc.center().y() - actualHeight*0.5f);
    } else if (Qt::AlignBottom & alignment_) {
        rcDraw.moveTo(rcDraw.left(), rc.bottom() - actualHeight);
    }

    painter.fillRect(rc, color);

    if (!prefix.isEmpty()) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        painter.setFont(font());
        painter.drawText(rcDraw, Qt::AlignVCenter | Qt::AlignLeft, prefix);

        rcDraw.setLeft(rcDraw.left() + prefixW + offsetPrefix);
    }

    if (!icon.isNull()) {
        const int w = std::min(iconW, rcDraw.width());
        const int h = std::min(iconH, rcDraw.height());
        const QRect iconRect(rcDraw.left(), (rcDraw.top() + rcDraw.bottom() - h) * 0.5f, w, h);

        painter.drawPixmap(iconRect, icon);
        rcDraw.setLeft(rcDraw.left() + w + offsetPostfix);
    }

    if (!postfix.isEmpty()) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QPen wasPen = painter.pen();

        QPen penHText(postfixColor_);
        painter.setPen(penHText);

        painter.setFont(font());
        painter.drawText(rcDraw, Qt::AlignVCenter | Qt::AlignLeft, postfix);
        painter.setPen(wasPen);
    }

    painter.end();
}

void Ui::PushButton_t::setPostfixColor(const QColor& color)
{
    postfixColor_ = color;
}

void Ui::PushButton_t::setText(const QString& prefix, const QString& postfix)
{
    prefix_  = prefix;
    postfix_ = postfix;
    repaint();
}

void Ui::PushButton_t::setIconSize(const int w, const int h) {
    iconW_ = w;
    iconH_ = h;
}

void Ui::PushButton_t::setImageForState(const eButtonState state, const std::string& image) {
    assert(state >= 0);
    assert(state < total);

    if (image.empty()) {
        bitmapsForStates_[state] = QPixmap();
    } else {
        bitmapsForStates_[state] = QPixmap(Utils::parse_image_name(image.c_str()));
    }
}

void Ui::PushButton_t::setOffsets(int fromIconToText) {
    fromIconToText_ = fromIconToText;
}

bool Ui::PushButton_t::event(QEvent *event) {
    if (event->type() == QEvent::Enter)
        (currentState_ = hovered), update();
    else if (event->type() == QEvent::Leave)
        (currentState_ = normal), update();
    if (event->type() == QEvent::MouseButtonPress)
        (currentState_ = pressed), update();

    return QPushButton::event(event);
}

void Ui::PushButton_t::setPrefix(const QString& prefix)
{
    prefix_ = prefix;
    repaint();
}

void Ui::PushButton_t::setPostfix(const QString& postfix)
{
    postfix_ = postfix;
    repaint();
}

int Ui::PushButton_t::precalculateWidth()
{
    const QPixmap& icon   = !bitmapsForStates_[currentState_].isNull() ? bitmapsForStates_[currentState_] : bitmapsForStates_[normal];

    const QString& prefix  = prefix_;
    const QString& postfix = postfix_; 

    QFontMetrics fm(font());
    const int offsetPrefix  = ((!icon.isNull()) & (!prefix.isEmpty()))  * fromIconToText_;
    const int offsetPostfix = ((!icon.isNull()) & (!postfix.isEmpty())) * fromIconToText_;

    const int iconW = (iconW_ >= 0 ? iconW_ : icon.width())  * !icon.isNull();

    const int prefixW  = !prefix.isEmpty() ? fm.width(prefix) : 0;
    const int postfixW = !postfix.isEmpty() ? fm.width(postfix) : 0;

    int desiredWidth = 0;
    desiredWidth += iconW;
    desiredWidth += prefixW;
    desiredWidth += postfixW;
    desiredWidth += offsetPrefix + offsetPostfix;

    return desiredWidth;
}