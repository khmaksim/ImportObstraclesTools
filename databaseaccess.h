#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseAccess : public QObject
{
        Q_OBJECT
    public:
        static DatabaseAccess* getInstance();

        void setTag(const QString&, const QVariantList&);
        QVector<QString> getTags();
        QVector<QString> getTypeObstracle();
        QVector<QVariantList> getAirfields();
        QVector<QVariantList> getObstracles(int id = -1);
        bool createTag(const QString&);
        bool removeTag(const QString&);
        int insertAirfield(const QString&, const QString&);
        void insertObstracle(int idAirfield, QMap<QString, QString> obstracle);
        bool removeAirfield(int idAirfield);

    private:
        DatabaseAccess(QObject *parent = nullptr);
        DatabaseAccess(const DatabaseAccess&);
        DatabaseAccess& operator =(const DatabaseAccess);
//        static DatabaseAccess *databaseAccess;

        QSqlDatabase db;

    signals:
        void updatedTags();
};

#endif // DATABASEACCESS_H
