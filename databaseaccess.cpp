#include "databaseaccess.h"
#include <QSqlDatabase>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QSqlRecord>
#include <QDateTime>
#include <QApplication>

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "obstracle");

    if (!QFile("ImportObstraclesTools.db").exists())
        qDebug() << "File database is not found.";

    db.setDatabaseName("ImportObstraclesTools.db");

    if (!db.open())
        qDebug() << "Can not connected to database.";

    db.exec("PRAGMA FOREIGN_KEYS=ON");       //set support for foreign keys
}

DatabaseAccess* DatabaseAccess::getInstance()
{
    static DatabaseAccess instance;
    return &instance;
}

//DatabaseAccess* DatabaseAccess::getInstance()
//{
//    if(databaseAccess == 0)
//       databaseAccess = new DatabaseAccess;
//    return databaseAccess;
//}

QVector<QString> DatabaseAccess::getTags()
{
    QSqlQuery query(db);
    QVector<QString> tags = QVector<QString>();

    query.exec("SELECT name FROM tag ORDER BY name");
    while (query.next())
        tags.append(query.value(0).toString());

    return tags;
}

QVector<QString> DatabaseAccess::getTypeObstracle()
{
    QSqlQuery query(db);
    QVector<QString> types = QVector<QString>();

    query.exec("SELECT name FROM obstracle GROUP BY name ORDER BY name");
    while (query.next())
        types.append(query.value(0).toString());

    return types;
}

void DatabaseAccess::setTag(const QString &nameTag, const QVariantList &idObstrales)
{
    QSqlQuery query(db);
    uint idTag = 0;

    query.prepare("SELECT id FROM tag WHERE name = :name");
    query.bindValue(":name", nameTag);
    if (!query.exec())
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
    if (query.first()) {
        idTag = query.value(0).toUInt();
    }
    else
        return;

    query.prepare("INSERT INTO obstracle_tag (id_obstracle, id_tag) SELECT :id_obstracle, :id_tag WHERE NOT EXISTS(SELECT 1 "
                  "FROM obstracle_tag WHERE id_obstracle = :id_obstracle AND id_tag = :id_tag)");

    query.bindValue(":id_obstracle", idObstrales);

    QVariantList tags;
    for (int i = 0; i < idObstrales.size(); i++)
        tags << idTag;
    query.bindValue(":id_tag", tags);
    if (!query.execBatch())
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();

    emit updatedTags();
}

bool DatabaseAccess::createTag(const QString &nameTag)
{
    QSqlQuery query(db);

    query.prepare("INSERT INTO tag (name) SELECT :name WHERE NOT EXISTS(SELECT 1 "
                  "FROM tag WHERE name = :name)");
    query.bindValue(":name", nameTag);
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
        return false;
    }
    if (query.numRowsAffected())
        return true;

    return false;
}

bool DatabaseAccess::removeTag(const QString &nameTag)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM tag WHERE name = ?");
    query.addBindValue(nameTag);
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
        return false;
    }
    return true;
}

QVector<QVariantList> DatabaseAccess::getAirfields()
{
    QSqlQuery query(db);
    QVector<QVariantList> airfields = QVector<QVariantList>();

    query.exec("SELECT name, code_icao, id FROM airfield ORDER BY name");
    while (query.next()) {
        QSqlRecord record = query.record();
        QVariantList list = QVariantList();
        for (int i = 0; i < record.count(); i++)
            list.append(record.value(i));

        airfields.append(list);
    }
    return airfields;
}

QVector<QVariantList> DatabaseAccess::getObstracles(int id)
{
    QSqlQuery query(db);
    QVector<QVariantList> obstracles = QVector<QVariantList>();

    if (id != -1) {
        query.prepare("SELECT ob.id, ob.name, tc.name, lo.name, cs.name, "
                      "ob.latitude, ob.longitude, ob.latitude_center, ob.longitude_center, ob.radius, ob.horizontal_accuracy, "
                      "ob.orthometric_height, ob.relative_height, ob.vertical_precision, tm.name, fg.name, "
                      "ob.marking_daytime, ob.marking_daytime_template, ob.marking_daytime_color, ob.night_marking, "
                      "ob.night_marking_color, ob.night_marking_type_light, ob.night_marking_intensity, "
                      "ob.night_marking_work_time, ob.accordance_icao, ob.source_data, ob.date_updated, ob.last_updated, "
                      "GROUP_CONCAT(t.name) "
                      "FROM obstracle ob "
                      "LEFT OUTER JOIN type_configuration_obstracle tc ON tc.id = ob.type_configuration "
                      "LEFT OUTER JOIN locality lo ON lo.id = ob.locality "
                      "LEFT OUTER JOIN coordinate_system cs ON cs.id = ob.coordinate_system "
                      "LEFT OUTER JOIN type_material tm ON tm.id = ob.type_material "
                      "LEFT OUTER JOIN fragility fg ON fg.id = ob.fragility "
                      "LEFT OUTER JOIN obstracle_tag obt ON ob.id = obt.id_obstracle "
                      "LEFT OUTER JOIN tag t ON obt.id_tag = t.id "
                      "WHERE ob.airfield = ? OR ob.airfield IS NULL GROUP BY ob.id");
        query.addBindValue(id);
    }
    else {
        query.prepare("SELECT ob.id, ob.name, tc.name, lo.name, cs.name, "
                      "ob.latitude, ob.longitude, ob.latitude_center, ob.longitude_center, ob.radius, ob.horizontal_accuracy, "
                      "ob.orthometric_height, ob.relative_height, ob.vertical_precision, tm.name, fg.name, "
                      "ob.marking_daytime, ob.marking_daytime_template, ob.marking_daytime_color, ob.night_marking, "
                      "ob.night_marking_color, ob.night_marking_type_light, ob.night_marking_intensity, "
                      "ob.night_marking_work_time, ob.accordance_icao, ob.source_data, ob.date_updated, ob.last_updated, "
                      "GROUP_CONCAT(t.name) "
                      "FROM obstracle ob "
                      "LEFT OUTER JOIN type_configuration_obstracle tc ON tc.id = ob.type_configuration "
                      "LEFT OUTER JOIN locality lo ON lo.id = ob.locality "
                      "LEFT OUTER JOIN coordinate_system cs ON cs.id = ob.coordinate_system "
                      "LEFT OUTER JOIN type_material tm ON tm.id = ob.type_material "
                      "LEFT OUTER JOIN fragility fg ON fg.id = ob.fragility "
                      "LEFT OUTER JOIN obstracle_tag obt ON ob.id = obt.id_obstracle "
                      "LEFT OUTER JOIN tag t ON obt.id_tag = t.id "
                      "GROUP BY ob.id");
    }
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
    }

    while (query.next()) {
        QSqlRecord record = query.record();
        QVariantList list = QVariantList();
        for (int i = 0; i < record.count(); i++)
            list.append(record.value(i));

        obstracles.append(list);
    }

    return obstracles;
}

int DatabaseAccess::insertAirfield(const QString &icaoCodeAirfield, const QString &nameAirfield)
{
    QSqlQuery query(db);

    query.exec("BEGIN TRANSACTION");

    if (!nameAirfield.isEmpty()) {
        query.prepare("INSERT INTO airfield (name, code_icao) SELECT :name, :code_icao WHERE NOT EXISTS(SELECT 1 "
                      "FROM airfield WHERE name = :name AND code_icao = :code_icao)");
        query.bindValue(":name", nameAirfield);
        query.bindValue(":code_icao", icaoCodeAirfield);
        if (!query.exec()) {
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            return -1;
        }

        // get id airfield
        query.prepare("SELECT id FROM airfield WHERE name = ? AND code_icao = ?");
        query.addBindValue(nameAirfield);
        query.addBindValue(icaoCodeAirfield);
        if (!query.exec()) {
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            QSqlDatabase::database().rollback();
        }

        if (query.first())
            return query.value(0).toInt();
    }
    return -1;
}

void DatabaseAccess::insertObstracle(int idAirfield, QMap<QString, QString> obstracle)
{
    QSqlQuery query(db);

    for (int i = 0; i < obstracle.size(); i++) {
        query.prepare("INSERT OR REPLACE INTO obstracle (id, name, latitude, longitude, orthometric_height, night_marking, airfield, last_updated) "
                      "VALUES (:id, :name, :latitude, :longitude, :orthometric_height, :night_marking, :airfield, datetime('now','localtime'))");
        query.bindValue(":id", obstracle.value("id"));
        query.bindValue(":name", obstracle.value("name"));
        query.bindValue(":latitude", obstracle.value("latitude"));
        query.bindValue(":longitude", obstracle.value("longitude"));
        query.bindValue(":orthometric_height", obstracle.value("orthometric_height").toInt());
        query.bindValue(":night_marking", obstracle.value("night_marking"));
        query.bindValue(":airfield", idAirfield);
        if (!query.exec())
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
    }
    query.exec("COMMIT");
//    if (!QSqlDatabase::database().commit()) {
//        QSqlDatabase::database().rollback();
//        qDebug() << "Rollback transaction";
//    }
}

bool DatabaseAccess::removeAirfield(int idAirfield)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM airfield WHERE id = ?");
    query.addBindValue(idAirfield);
    if (!query.exec()) {
        qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
        return false;
    }
    return true;
}
