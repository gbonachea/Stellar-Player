#ifndef SUBTITLESETTINGSDIALOG_H
#define SUBTITLESETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QSpinBox>

class SubtitleSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubtitleSettingsDialog(QWidget *parent = nullptr);
    ~SubtitleSettingsDialog();

    void setSubtitleTrack(int track);
    void setSubtitleDelay(double delay);
    void setSubtitleSize(int size);
    void setSubtitleColor(const QColor &color);
    void setSubtitlePosition(int position);

    int getSubtitleTrack() const;
    double getSubtitleDelay() const;
    int getSubtitleSize() const;
    QColor getSubtitleColor() const;
    int getSubtitlePosition() const;

signals:
    void subtitleSettingsChanged();

private slots:
    void onTrackChanged(int index);
    void onDelayChanged(int value);
    void onSizeChanged(int value);
    void onColorClicked();
    void onPositionChanged(int value);
    void onApplyClicked();
    void onCancelClicked();

private:
    void setupUI();
    void updateColorButton();

    QComboBox *trackComboBox;
    QSlider *delaySlider;
    QLabel *delayLabel;
    QSpinBox *sizeSpinBox;
    QPushButton *colorButton;
    QSlider *positionSlider;
    QLabel *positionLabel;
    QPushButton *applyButton;
    QPushButton *cancelButton;

    QColor currentColor;
};

#endif // SUBTITLESETTINGSDIALOG_H