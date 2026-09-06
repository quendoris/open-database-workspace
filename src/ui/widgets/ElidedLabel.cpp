#include "ElidedLabel.hpp"

#include <QFontMetrics>
#include <QResizeEvent>
#include <QSizePolicy>

namespace odw::ui::widgets {

ElidedLabel::ElidedLabel(QWidget* parent)
    : QLabel(parent) {
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

ElidedLabel::ElidedLabel(const QString& text, QWidget* parent)
    : ElidedLabel(parent) {
    setFullText(text);
}

void ElidedLabel::setFullText(const QString& text) {
    fullText_ = text;
    updateVisibleText();
}

const QString& ElidedLabel::fullText() const noexcept {
    return fullText_;
}

void ElidedLabel::resizeEvent(QResizeEvent* event) {
    QLabel::resizeEvent(event);
    updateVisibleText();
}

void ElidedLabel::updateVisibleText() {
    const int availableWidth = qMax(0, contentsRect().width());
    const QFontMetrics metrics(font());
    const bool clipped = metrics.horizontalAdvance(fullText_) > availableWidth;

    QLabel::setText(metrics.elidedText(fullText_, Qt::ElideRight, availableWidth));
    setToolTip(clipped ? fullText_ : QString{});
}

} // namespace odw::ui::widgets
