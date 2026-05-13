#include "SubtitleSettingsDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>

SubtitleSettingsDialog::SubtitleSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , currentColor(Qt::white)
{
    setWindowTitle("Ajustes de subtítulos");
    setModal(true);
    setupUI();
}

SubtitleSettingsDialog::~SubtitleSettingsDialog()
{
}

void SubtitleSettingsDialog::setSubtitleTrack(int track)
{
    trackComboBox->setCurrentIndex(track);
}

void SubtitleSettingsDialog::setSubtitleDelay(double delay)
{
    delaySlider->setValue(static_cast<int>(delay * 10)); // Convert to slider units
}

void SubtitleSettingsDialog::setSubtitleSize(int size)
{
    sizeSpinBox->setValue(size);
}

void SubtitleSettingsDialog::setSubtitleColor(const QColor &color)
{
    currentColor = color;
    updateColorButton();
}

void SubtitleSettingsDialog::setSubtitlePosition(int position)
{
    positionSlider->setValue(position);
}

int SubtitleSettingsDialog::getSubtitleTrack() const
{
    return trackComboBox->currentIndex();
}

double SubtitleSettingsDialog::getSubtitleDelay() const
{
    return delaySlider->value() / 10.0; // Convert from slider units
}

int SubtitleSettingsDialog::getSubtitleSize() const
{
    return sizeSpinBox->value();
}

QColor SubtitleSettingsDialog::getSubtitleColor() const
{
    return currentColor;
}

int SubtitleSettingsDialog::getSubtitlePosition() const
{
    return positionSlider->value();
}

void SubtitleSettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Track selection
    QGroupBox *trackGroup = new QGroupBox("Pista de subtítulos");
    QVBoxLayout *trackLayout = new QVBoxLayout(trackGroup);
    trackComboBox = new QComboBox();
    trackComboBox->addItem("Ninguna", 0);
    trackComboBox->addItem("Pista 1", 1);
    trackComboBox->addItem("Pista 2", 2);
    trackComboBox->addItem("Pista 3", 3);
    trackLayout->addWidget(trackComboBox);
    mainLayout->addWidget(trackGroup);

    // Delay adjustment
    QGroupBox *delayGroup = new QGroupBox("Retraso de subtítulos");
    QVBoxLayout *delayLayout = new QVBoxLayout(delayGroup);
    delaySlider = new QSlider(Qt::Horizontal);
    delaySlider->setRange(-500, 500); // -50 to +50 seconds in 0.1s units
    delaySlider->setValue(0);
    delayLabel = new QLabel("0.0 s");
    delayLabel->setAlignment(Qt::AlignCenter);
    delayLayout->addWidget(delaySlider);
    delayLayout->addWidget(delayLabel);
    mainLayout->addWidget(delayGroup);

    // Size adjustment
    QGroupBox *sizeGroup = new QGroupBox("Tamaño de fuente");
    QVBoxLayout *sizeLayout = new QVBoxLayout(sizeGroup);
    sizeSpinBox = new QSpinBox();
    sizeSpinBox->setRange(10, 200);
    sizeSpinBox->setValue(100);
    sizeSpinBox->setSuffix(" %");
    sizeLayout->addWidget(sizeSpinBox);
    mainLayout->addWidget(sizeGroup);

    // Color selection
    QGroupBox *colorGroup = new QGroupBox("Color de fuente");
    QHBoxLayout *colorLayout = new QHBoxLayout(colorGroup);
    colorButton = new QPushButton();
    colorButton->setFixedSize(50, 30);
    updateColorButton();
    colorLayout->addWidget(colorButton);
    colorLayout->addStretch();
    mainLayout->addWidget(colorGroup);

    // Position adjustment
    QGroupBox *positionGroup = new QGroupBox("Posición");
    QVBoxLayout *positionLayout = new QVBoxLayout(positionGroup);
    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 100);
    positionSlider->setValue(100);
    positionLabel = new QLabel("100%");
    positionLabel->setAlignment(Qt::AlignCenter);
    positionLayout->addWidget(positionSlider);
    positionLayout->addWidget(positionLabel);
    mainLayout->addWidget(positionGroup);

    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(trackComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SubtitleSettingsDialog::onTrackChanged);
    connect(delaySlider, &QSlider::valueChanged, this, &SubtitleSettingsDialog::onDelayChanged);
    connect(sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SubtitleSettingsDialog::onSizeChanged);
    connect(colorButton, &QPushButton::clicked, this, &SubtitleSettingsDialog::onColorClicked);
    connect(positionSlider, &QSlider::valueChanged, this, &SubtitleSettingsDialog::onPositionChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SubtitleSettingsDialog::onApplyClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SubtitleSettingsDialog::onCancelClicked);

    // Update labels initially
    onDelayChanged(delaySlider->value());
    onPositionChanged(positionSlider->value());
}

void SubtitleSettingsDialog::onTrackChanged(int index)
{
    Q_UNUSED(index)
    // Track selection changed
}

void SubtitleSettingsDialog::onDelayChanged(int value)
{
    double delay = value / 10.0;
    delayLabel->setText(QString("%1 s").arg(delay, 0, 'f', 1));
}

void SubtitleSettingsDialog::onSizeChanged(int value)
{
    Q_UNUSED(value)
    // Font size changed
}

void SubtitleSettingsDialog::onColorClicked()
{
    QColor color = QColorDialog::getColor(currentColor, this, "Seleccionar color de subtítulos");
    if (color.isValid()) {
        currentColor = color;
        updateColorButton();
    }
}

void SubtitleSettingsDialog::onPositionChanged(int value)
{
    positionLabel->setText(QString("%1%").arg(value));
}

void SubtitleSettingsDialog::onApplyClicked()
{
    emit subtitleSettingsChanged();
    accept();
}

void SubtitleSettingsDialog::onCancelClicked()
{
    reject();
}

void SubtitleSettingsDialog::updateColorButton()
{
    QString style = QString("background-color: %1; border: 1px solid black;").arg(currentColor.name());
    colorButton->setStyleSheet(style);
}