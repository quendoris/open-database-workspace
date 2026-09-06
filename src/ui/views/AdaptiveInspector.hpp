#pragma once

#include <QWidget>

#include <array>

class QGridLayout;
class QLabel;
class QResizeEvent;

namespace odw::ui::widgets {
class ElidedLabel;
}

namespace odw::ui::views {

class AdaptiveInspector final : public QWidget {
public:
    enum class PresentationMode {
        Compact,
        Normal,
        Expanded,
    };

    explicit AdaptiveInspector(QWidget* parent = nullptr);

    [[nodiscard]] PresentationMode presentationMode() const noexcept;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updatePresentation();
    void applyPresentation(PresentationMode mode);

    static constexpr int kNormalWidth = 160;
    static constexpr int kExpandedWidth = 260;

    QGridLayout* layout_ = nullptr;
    std::array<QLabel*, 4> keys_{};
    std::array<widgets::ElidedLabel*, 4> values_{};
    PresentationMode mode_ = PresentationMode::Expanded;
};

} // namespace odw::ui::views
