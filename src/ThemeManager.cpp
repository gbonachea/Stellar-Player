#include "ThemeManager.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>

ThemeManager *ThemeManager::instance()
{
    static ThemeManager instance;
    return &instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , currentTheme("dark")
{
    loadTheme(currentTheme);
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::setTheme(const QString &themeName)
{
    if (currentTheme != themeName) {
        currentTheme = themeName;
        loadTheme(themeName);
        emit themeChanged();
    }
}

QString ThemeManager::getCurrentTheme() const
{
    return currentTheme;
}

QColor ThemeManager::getBackgroundColor() const
{
    return backgroundColor;
}

QColor ThemeManager::getForegroundColor() const
{
    return foregroundColor;
}

QColor ThemeManager::getAccentColor() const
{
    return accentColor;
}

QColor ThemeManager::getButtonColor() const
{
    return buttonColor;
}

QColor ThemeManager::getButtonHoverColor() const
{
    return buttonHoverColor;
}

QColor ThemeManager::getSliderColor() const
{
    return sliderColor;
}

QColor ThemeManager::getSliderHandleColor() const
{
    return sliderHandleColor;
}

QFont ThemeManager::getFont() const
{
    return font;
}

void ThemeManager::loadTheme(const QString &themeName)
{
    if (themeName == "dark") {
        backgroundColor = QColor(30, 30, 30);
        foregroundColor = QColor(220, 220, 220);
        accentColor = QColor(70, 130, 180);
        buttonColor = QColor(60, 60, 60);
        buttonHoverColor = QColor(80, 80, 80);
        sliderColor = QColor(100, 100, 100);
        sliderHandleColor = QColor(150, 150, 150);
        font = QFont("Segoe UI", 10);
    } else if (themeName == "light") {
        backgroundColor = QColor(240, 240, 240);
        foregroundColor = QColor(30, 30, 30);
        accentColor = QColor(70, 130, 180);
        buttonColor = QColor(220, 220, 220);
        buttonHoverColor = QColor(200, 200, 200);
        sliderColor = QColor(180, 180, 180);
        sliderHandleColor = QColor(120, 120, 120);
        font = QFont("Segoe UI", 10);
    } else {
        // Default to dark theme
        loadTheme("dark");
        return;
    }

    // Apply theme to application
    QPalette palette;
    palette.setColor(QPalette::Window, backgroundColor);
    palette.setColor(QPalette::WindowText, foregroundColor);
    palette.setColor(QPalette::Base, backgroundColor.lighter(110));
    palette.setColor(QPalette::AlternateBase, backgroundColor.lighter(120));
    palette.setColor(QPalette::Text, foregroundColor);
    palette.setColor(QPalette::BrightText, accentColor);
    palette.setColor(QPalette::Button, buttonColor);
    palette.setColor(QPalette::ButtonText, foregroundColor);
    palette.setColor(QPalette::Highlight, accentColor);
    palette.setColor(QPalette::HighlightedText, backgroundColor);

    QApplication::setPalette(palette);
    QApplication::setFont(font);
}