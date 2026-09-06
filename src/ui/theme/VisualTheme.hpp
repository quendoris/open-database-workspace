#pragma once

#include <QString>

class QApplication;

namespace odw::ui::theme {

struct VisualTheme final {
    QString canvas;
    QString panel;
    QString raised;
    QString hover;
    QString textPrimary;
    QString textSecondary;
    QString borderSubtle;
    QString borderActive;
    QString accentPrimary;
    QString accentSoft;
    QString selection;
    QString danger;
    QString warning;
    QString success;

    int radiusSmall = 5;
    int radiusMedium = 8;
    int spacing = 8;
    int compactRowHeight = 30;
};

[[nodiscard]] VisualTheme defaultDarkCyan();
void apply(QApplication& app, const VisualTheme& theme);

} // namespace odw::ui::theme
