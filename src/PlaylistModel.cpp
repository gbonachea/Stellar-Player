#include "PlaylistModel.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PlaylistModel::~PlaylistModel()
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return playlist.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= playlist.size()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return QFileInfo(playlist.at(index.row())).baseName();
    case FilePathRole:
        return playlist.at(index.row());
    case DurationRole:
        return durations.at(index.row());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilePathRole] = "filePath";
    roles[TitleRole] = "title";
    roles[DurationRole] = "duration";
    return roles;
}

void PlaylistModel::addItem(const QString &filePath)
{
    beginInsertRows(QModelIndex(), playlist.size(), playlist.size());
    playlist.append(filePath);
    durations.append(0.0); // Duration will be updated later
    endInsertRows();
}

void PlaylistModel::removeItem(int index)
{
    if (index < 0 || index >= playlist.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    playlist.removeAt(index);
    durations.removeAt(index);
    endRemoveRows();
}

void PlaylistModel::clear()
{
    beginResetModel();
    playlist.clear();
    durations.clear();
    endResetModel();
}

void PlaylistModel::moveItem(int from, int to)
{
    if (from < 0 || from >= playlist.size() || to < 0 || to >= playlist.size() || from == to) {
        return;
    }

    beginMoveRows(QModelIndex(), from, from, QModelIndex(), to > from ? to + 1 : to);
    playlist.move(from, to);
    durations.move(from, to);
    endMoveRows();
}

QString PlaylistModel::getItem(int index) const
{
    if (index < 0 || index >= playlist.size()) {
        return QString();
    }
    return playlist.at(index);
}

int PlaylistModel::getCount() const
{
    return playlist.size();
}

void PlaylistModel::saveToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    out << "#EXTM3U\n";
    for (int i = 0; i < playlist.size(); ++i) {
        out << "#EXTINF:" << durations.at(i) << "," << QFileInfo(playlist.at(i)).baseName() << "\n";
        out << playlist.at(i) << "\n";
    }
    file.close();
}

void PlaylistModel::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return;
    }

    clear();

    QTextStream in(&file);
    QString line;
    bool isM3U = false;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("#EXTM3U")) {
            isM3U = true;
        } else if (line.startsWith("#EXTINF:")) {
            // Parse duration from M3U format
            QStringList parts = line.split(",");
            if (parts.size() >= 2) {
                QString durationStr = parts.first().mid(8); // Remove "#EXTINF:"
                double duration = durationStr.toDouble();
                durations.append(duration);
            }
        } else if (!line.isEmpty() && !line.startsWith("#")) {
            // This is a file path
            if (QFile::exists(line)) {
                playlist.append(line);
                if (!isM3U || durations.size() < playlist.size()) {
                    durations.append(0.0);
                }
            }
        }
    }

    file.close();

    if (playlist.size() > 0) {
        beginInsertRows(QModelIndex(), 0, playlist.size() - 1);
        endInsertRows();
    }
}