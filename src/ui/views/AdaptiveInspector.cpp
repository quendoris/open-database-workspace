#include "AdaptiveInspector.hpp"

#include "../widgets/ElidedLabel.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QStringList>

namespace odw::ui::views {
namespace {

const QStringList kCompactKeys{
    QStringLiteral("Type"),
    QStringLiteral("Rows"),
    QStringLiteral("PK"),
    QStringLiteral("Source"),
};

const QStringList kExpandedKeys{
    QStringLiteral("Object type"),
    QStringLiteral("Estimated rows"),
    QStringLiteral("Primary key"),
    QStringLiteral("Connector source"),
};

const QStringList kBaseValues{
    QStringLiteral("table"),
    QStringLiteral("unknown"),
    QStringLiteral("id"),
    QStringLiteral("prototype connector"),
};

} // namespace

AdaptiveInspector::AdaptiveInspector(QWidget* parent)
    : QWidget(parent),
      layout_(new QGridLayout(this)) {
    setObjectName(QStringLiteral("odwAdaptiveInspector"));
    setMinimumWidth(96);

    layout_->setContentsMargins(10, 10, 10, 10);
    layout_->setHorizontalSpacing(12);
    layout_->setVerticalSpacing(8);
    layout_->setColumnStretch(1, 1);

    for (std::size_t index = 0; index < keys_.size(); ++index) {
        auto* key = new QLabel(kExpandedKeys.at(static_cast<qsizetype>(index)), this);
        key->setObjectName(QStringLiteral("odwInspectorKey"));

        auto* value = new widgets::ElidedLabel(
            kBaseValues.at(static_cast<qsizetype>(index)), this);
        value->setObjectName(QStringLiteral("odwInspectorValue"));

        keys_[index] = key;
        values_[index] = value;
    }

    applyPresentation(PresentationMode::Expanded);
}

AdaptiveInspector::PresentationMode AdaptiveInspector::presentationMode() const noexcept {
    return mode_;
}

QSize AdaptiveInspector::sizeHint() const {
    return QSize(270, 180);
}

QSize AdaptiveInspector::minimumSizeHint() const {
    return QSize(96, 140);
}

void AdaptiveInspector::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updatePresentation();
}

void AdaptiveInspector::updatePresentation() {
    PresentationMode nextMode = PresentationMode::Compact;
    if (width() >= kExpandedWidth) {
        nextMode = PresentationMode::Expanded;
    } else if (width() >= kNormalWidth) {
        nextMode = PresentationMode::Normal;
    }

    if (nextMode != mode_) {
        applyPresentation(nextMode);
    }
}

void AdaptiveInspector::applyPresentation(PresentationMode mode) {
    for (std::size_t index = 0; index < keys_.size(); ++index) {
        layout_->removeWidget(keys_[index]);
        layout_->removeWidget(values_[index]);
    }

    const bool expanded = mode == PresentationMode::Expanded;
    for (std::size_t index = 0; index < keys_.size(); ++index) {
        keys_[index]->setText((expanded ? kExpandedKeys : kCompactKeys)
                                  .at(static_cast<qsizetype>(index)));
        values_[index]->setFullText(kBaseValues.at(static_cast<qsizetype>(index)));

        if (mode == PresentationMode::Compact) {
            const int row = static_cast<int>(index) * 2;
            layout_->addWidget(keys_[index], row, 0, 1, 2);
            layout_->addWidget(values_[index], row + 1, 0, 1, 2);
        } else {
            const int row = static_cast<int>(index);
            layout_->addWidget(keys_[index], row, 0);
            layout_->addWidget(values_[index], row, 1);
        }
    }

    layout_->setRowStretch(mode == PresentationMode::Compact ? 8 : 4, 1);
    mode_ = mode;
}

} // namespace odw::ui::views
