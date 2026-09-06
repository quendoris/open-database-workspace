#include "VisualTheme.hpp"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QString>

namespace odw::ui::theme {

VisualTheme defaultDarkCyan() {
    return VisualTheme{
        .canvas = QStringLiteral("#0B1014"),
        .panel = QStringLiteral("#10171D"),
        .raised = QStringLiteral("#151E25"),
        .hover = QStringLiteral("#19252D"),
        .textPrimary = QStringLiteral("#E7F1F5"),
        .textSecondary = QStringLiteral("#91A5AE"),
        .borderSubtle = QStringLiteral("#21313A"),
        .borderActive = QStringLiteral("#28D7E5"),
        .accentPrimary = QStringLiteral("#28D7E5"),
        .accentSoft = QStringLiteral("#123941"),
        .selection = QStringLiteral("#174A52"),
        .danger = QStringLiteral("#FF6B7A"),
        .warning = QStringLiteral("#E7B85C"),
        .success = QStringLiteral("#63D69A"),
    };
}

void apply(QApplication& app, const VisualTheme& theme) {
    app.setStyle(QStringLiteral("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(theme.canvas));
    palette.setColor(QPalette::WindowText, QColor(theme.textPrimary));
    palette.setColor(QPalette::Base, QColor(theme.panel));
    palette.setColor(QPalette::AlternateBase, QColor(theme.raised));
    palette.setColor(QPalette::ToolTipBase, QColor(theme.raised));
    palette.setColor(QPalette::ToolTipText, QColor(theme.textPrimary));
    palette.setColor(QPalette::Text, QColor(theme.textPrimary));
    palette.setColor(QPalette::Button, QColor(theme.raised));
    palette.setColor(QPalette::ButtonText, QColor(theme.textPrimary));
    palette.setColor(QPalette::BrightText, QColor(theme.danger));
    palette.setColor(QPalette::Highlight, QColor(theme.selection));
    palette.setColor(QPalette::HighlightedText, QColor(theme.textPrimary));
    palette.setColor(QPalette::PlaceholderText, QColor(theme.textSecondary));
    app.setPalette(palette);

    QString style = QStringLiteral(R"QSS(
* {
    color: @textPrimary@;
    selection-background-color: @selection@;
    selection-color: @textPrimary@;
}

QMainWindow, QDialog {
    background: @canvas@;
}

QWidget#odwCanvas {
    background: @canvas@;
}

QToolBar {
    background: @canvas@;
    border: 0;
    border-bottom: 1px solid @borderSubtle@;
    padding: 4px 5px;
    spacing: 5px;
}

QToolBar::separator {
    background: @borderSubtle@;
    width: 1px;
    margin: 6px 4px;
}

QStatusBar {
    background: @canvas@;
    color: @textSecondary@;
    border-top: 1px solid @borderSubtle@;
}

QLabel#odwEmptyTitle {
    color: @textPrimary@;
    font-size: 22px;
    font-weight: 600;
}

QLabel#odwEmptySubtitle,
QLabel#odwSecondaryText,
QLabel#odwInspectorKey {
    color: @textSecondary@;
}

QLabel#odwInspectorKey {
    font-size: 11px;
}

QLabel#odwInspectorValue {
    color: @textPrimary@;
    font-weight: 500;
}

QLabel#odwPipelineSummary {
    background: @raised@;
    color: @textPrimary@;
    border: 1px solid @borderSubtle@;
    border-radius: @radiusSmall@px;
    padding: 9px 10px;
}

QDockWidget {
    background: @panel@;
    border: 1px solid @borderSubtle@;
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
}

QDockWidget::title {
    background: @raised@;
    color: @textSecondary@;
    min-height: 30px;
    padding-left: 10px;
    border-bottom: 1px solid @borderSubtle@;
    text-align: left;
}

QDockWidget:focus {
    border: 1px solid @borderActive@;
}

QTreeView, QListView, QTableView, QTableWidget, QTreeWidget, QListWidget,
QTextEdit, QPlainTextEdit, QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
    background: @panel@;
    color: @textPrimary@;
    border: 1px solid @borderSubtle@;
    border-radius: @radiusSmall@px;
    outline: 0;
}

QTreeView::item, QListView::item {
    min-height: 28px;
    padding: 2px 6px;
    border-radius: @radiusSmall@px;
}

QTreeView::item:hover, QListView::item:hover,
QTableView::item:hover, QTableWidget::item:hover {
    background: @hover@;
}

QTreeView::item:selected, QListView::item:selected {
    background: @selection@;
    color: @textPrimary@;
}

QHeaderView::section {
    background: @raised@;
    color: @textSecondary@;
    border: 0;
    border-right: 1px solid @borderSubtle@;
    border-bottom: 1px solid @borderSubtle@;
    padding: 7px 9px;
    font-weight: 600;
}

QTableView, QTableWidget {
    gridline-color: @borderSubtle@;
    alternate-background-color: @raised@;
}

QTableView::item, QTableWidget::item {
    padding: 5px 8px;
    border: 0;
}

QTabWidget::pane {
    border: 1px solid @borderSubtle@;
    background: @panel@;
}

QTabBar::tab {
    background: transparent;
    color: @textSecondary@;
    border: 0;
    border-bottom: 2px solid transparent;
    padding: 7px 12px;
    margin-right: 2px;
}

QTabBar::tab:hover {
    color: @textPrimary@;
    background: @hover@;
}

QTabBar::tab:selected {
    color: @textPrimary@;
    border-bottom: 2px solid @accentPrimary@;
    background: @accentSoft@;
}

QPushButton, QToolButton {
    background: @raised@;
    color: @textPrimary@;
    border: 1px solid @borderSubtle@;
    border-radius: @radiusSmall@px;
    padding: 6px 10px;
}

QPushButton:hover, QToolButton:hover {
    background: @hover@;
    border-color: @accentPrimary@;
}

QPushButton:pressed, QToolButton:pressed {
    background: @accentSoft@;
}

QPushButton:disabled, QToolButton:disabled {
    color: @textSecondary@;
    background: @panel@;
}

QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus,
QTreeView:focus, QListView:focus, QTableView:focus, QTableWidget:focus {
    border: 1px solid @borderActive@;
}

QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 2px;
}

QScrollBar::handle:vertical {
    background: @borderSubtle@;
    min-height: 28px;
    border-radius: 4px;
}

QScrollBar::handle:vertical:hover {
    background: @textSecondary@;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
    background: transparent;
    height: 0;
}

QScrollBar:horizontal {
    background: transparent;
    height: 10px;
    margin: 2px;
}

QScrollBar::handle:horizontal {
    background: @borderSubtle@;
    min-width: 28px;
    border-radius: 4px;
}

QScrollBar::handle:horizontal:hover {
    background: @textSecondary@;
}

QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal,
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
    background: transparent;
    width: 0;
}

QSplitter::handle {
    background: @canvas@;
}

QSplitter::handle:hover {
    background: @accentSoft@;
}

QToolTip {
    background: @raised@;
    color: @textPrimary@;
    border: 1px solid @borderSubtle@;
    padding: 5px;
}
)QSS");

    style.replace(QStringLiteral("@canvas@"), theme.canvas);
    style.replace(QStringLiteral("@panel@"), theme.panel);
    style.replace(QStringLiteral("@raised@"), theme.raised);
    style.replace(QStringLiteral("@hover@"), theme.hover);
    style.replace(QStringLiteral("@textPrimary@"), theme.textPrimary);
    style.replace(QStringLiteral("@textSecondary@"), theme.textSecondary);
    style.replace(QStringLiteral("@borderSubtle@"), theme.borderSubtle);
    style.replace(QStringLiteral("@borderActive@"), theme.borderActive);
    style.replace(QStringLiteral("@accentPrimary@"), theme.accentPrimary);
    style.replace(QStringLiteral("@accentSoft@"), theme.accentSoft);
    style.replace(QStringLiteral("@selection@"), theme.selection);
    style.replace(QStringLiteral("@radiusSmall@"), QString::number(theme.radiusSmall));

    app.setStyleSheet(style);
}

} // namespace odw::ui::theme
