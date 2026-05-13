#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QFont>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager *instance();

    void setTheme(const QString &themeName);
    QString getCurrentTheme() const;

    QColor getBackgroundColor() const;
    QColor getForegroundColor() const;
    QColor getAccentColor() const;
    QColor getButtonColor() const;
    QColor getButtonHoverColor() const;
    QColor getSliderColor() const;
    QColor getSliderHandleColor() const;
    QFont getFont() const;

signals:
    void themeChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    void loadTheme(const QString &themeName);

    QString currentTheme;
    QColor backgroundColor;
    QColor foregroundColor;
    QColor accentColor;
    QColor buttonColor;
    QColor buttonHoverColor;
    QColor sliderColor;
    QColor sliderHandleColor;
    QFont font;
};

#endif // THEMEMANAGER_H