#pragma once

#include <QLabel>

class QResizeEvent;

namespace odw::ui::widgets {

class ElidedLabel final : public QLabel {
public:
    explicit ElidedLabel(QWidget* parent = nullptr);
    explicit ElidedLabel(const QString& text, QWidget* parent = nullptr);

    void setFullText(const QString& text);
    [[nodiscard]] const QString& fullText() const noexcept;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateVisibleText();

    QString fullText_;
};

} // namespace odw::ui::widgets
