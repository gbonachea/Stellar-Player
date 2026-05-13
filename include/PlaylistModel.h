#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        FilePathRole = Qt::UserRole + 1,
        TitleRole,
        DurationRole
    };

    explicit PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addItem(const QString &filePath);
    void removeItem(int index);
    void clear();
    void moveItem(int from, int to);
    QString getItem(int index) const;
    int getCount() const;

    void saveToFile(const QString &filePath);
    void loadFromFile(const QString &filePath);

private:
    QStringList playlist;
    QList<double> durations;
};

#endif // PLAYLISTMODEL_H