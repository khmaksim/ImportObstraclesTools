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

void DatabaseAccess::update(const QString &icaoCodeAirfield, const QString &nameAirfield, QVector<QVector<QString>> obstracles)
{
    QSqlQuery query(db);
    QVariant idAirfield = QVariant();

    query.exec("BEGIN TRANSACTION");

    if (!icaoCodeAirfield.isEmpty() && !nameAirfield.isEmpty()) {
        query.prepare("INSERT INTO airfield (name, code_icao) SELECT :name, :code_icao WHERE NOT EXISTS(SELECT 1 "
                      "FROM airfield WHERE name = :name AND code_icao = :code_icao)");
        query.bindValue(":name", nameAirfield);
        query.bindValue(":code_icao", icaoCodeAirfield);
        if (!query.exec()) {
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            return;
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
            idAirfield = query.value(0).toUInt();
        else
            return;
    }

    for (int i = 0; i < obstracles.size(); i++) {
        QVariant idCoordinationSystem = QVariant();
        QVariant idFragility = QVariant();
        QVariant idLocality = QVariant();
        QVariant idTypeConfigurationObstracle = QVariant();
        QVariant idTypeMaterial = QVariant();

        if (!obstracles.at(i).at(4).isEmpty()) {
            query.prepare("INSERT INTO coordinate_system (name) SELECT :name WHERE NOT EXISTS(SELECT 1 "
                          "FROM coordinate_system WHERE name = :name)");
            query.bindValue(":name", obstracles.at(i).at(4));
            if (!query.exec()) {
                qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            }
            else {
                // get id coordination system
                query.prepare("SELECT id FROM coordinate_system WHERE name = ?");
                query.addBindValue(obstracles.at(i).at(4));
                if (!query.exec()) {
                    qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
                }
                if (query.first())
                    idCoordinationSystem = query.value(0).toUInt();
            }
        }

        if (!obstracles.at(i).at(15).isEmpty()) {
            query.prepare("INSERT INTO fragility (name) SELECT :name WHERE NOT EXISTS(SELECT 1 \
                          FROM fragility WHERE name = :name)");
                    query.bindValue(":name", obstracles.at(i).at(15));
            if (!query.exec())
                qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            else {
                // get id fragility
                query.prepare("SELECT id FROM fragility WHERE name = ?");
                query.addBindValue(obstracles.at(i).at(15));
                if (!query.exec())
                    qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
                if (query.first())
                    idFragility = query.value(0).toUInt();
            }
        }

        if (!obstracles.at(i).at(3).isEmpty()) {
            query.prepare("INSERT INTO locality (name) SELECT :name WHERE NOT EXISTS(SELECT 1 \
                          FROM locality WHERE name = :name)");
                    query.bindValue(":name", obstracles.at(i).at(3));
            if (!query.exec())
                qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            else {
                // get id locality
                query.prepare("SELECT id FROM locality WHERE name = ?");
                query.addBindValue(obstracles.at(i).at(3));
                if (!query.exec()) {
                    qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
                }
                if (query.first())
                    idLocality = query.value(0).toUInt();
            }
        }

        if (!obstracles.at(i).at(2).isEmpty()) {
            query.prepare("INSERT INTO type_configuration_obstracle (name) SELECT :name WHERE NOT EXISTS(SELECT 1 \
                          FROM type_configuration_obstracle WHERE name = :name)");
                    query.bindValue(":name",  obstracles.at(i).at(2));
            if (!query.exec())
                qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            else {
                // get id type configuration obstracle
                query.prepare("SELECT id FROM type_configuration_obstracle WHERE name = ?");
                query.addBindValue(obstracles.at(i).at(2));
                if (!query.exec())
                    qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
                if (query.first())
                    idTypeConfigurationObstracle = query.value(0).toUInt();
            }
        }

        if (!obstracles.at(i).at(14).isEmpty()) {
            query.prepare("INSERT INTO type_material (name) SELECT :name WHERE NOT EXISTS(SELECT 1 \
                          FROM type_material WHERE name = :name)");
                    query.bindValue(":name", obstracles.at(i).at(14));
            if (!query.exec())
                qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
            else {
                // get id type material
                query.prepare("SELECT id FROM type_material WHERE name = ?");
                query.addBindValue(obstracles.at(i).at(14));
                if (!query.exec())
                    qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
                if (query.first())
                    idTypeMaterial = query.value(0).toUInt();
            }
        }

        query.prepare("INSERT OR REPLACE INTO obstracle (id, name, type_configuration, locality, coordinate_system, "
                        "latitude, longitude, latitude_center, longitude_center, radius, horizontal_accuracy, "
                        "orthometric_height, relative_height, vertical_precision, type_material, fragility, "
                        "marking_daytime, marking_daytime_template, marking_daytime_color, night_marking, "
                        "night_marking_color, night_marking_type_light, night_marking_intensity, "
                        "night_marking_work_time, accordance_icao, source_data, date_updated, airfield, last_updated) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                        "datetime('now','localtime'))");
        query.addBindValue(obstracles.at(i).at(0));
        query.addBindValue(obstracles.at(i).at(1));
        query.addBindValue(idTypeConfigurationObstracle);
        query.addBindValue(idLocality);
        query.addBindValue(idCoordinationSystem);
        query.addBindValue(obstracles.at(i).at(5));
        query.addBindValue(obstracles.at(i).at(6));
        query.addBindValue(obstracles.at(i).at(7));
        query.addBindValue(obstracles.at(i).at(8));
        query.addBindValue(obstracles.at(i).at(9));
        query.addBindValue(obstracles.at(i).at(10));
        query.addBindValue(obstracles.at(i).at(11));
        query.addBindValue(obstracles.at(i).at(12));
        query.addBindValue(obstracles.at(i).at(13));
        query.addBindValue(idTypeMaterial);
        query.addBindValue(idFragility);
        query.addBindValue(obstracles.at(i).at(16));
        query.addBindValue(obstracles.at(i).at(17));
        query.addBindValue(obstracles.at(i).at(18));
        query.addBindValue(obstracles.at(i).at(19));
        query.addBindValue(obstracles.at(i).at(20));
        query.addBindValue(obstracles.at(i).at(21));
        query.addBindValue(obstracles.at(i).at(22));
        query.addBindValue(obstracles.at(i).at(23));
        query.addBindValue(obstracles.at(i).at(24));
        query.addBindValue(obstracles.at(i).at(25));
        query.addBindValue(obstracles.at(i).at(26));
        query.addBindValue(idAirfield);
        if (!query.exec())
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
    }
    query.exec("COMMIT");
//    if (!QSqlDatabase::database().commit()) {
//        QSqlDatabase::database().rollback();
//        qDebug() << "Rollback transaction";
//    }
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
        query.prepare("INSERT OR REPLACE INTO obstracle (id, name, latitude, longitude, "
                      "orthometric_height, airfield, last_updated) "
                      "VALUES (:id, :name, :latitude, :longitude, "
                      ":orthometric_height, :airfield, :last_updated, "
                      "datetime('now','localtime'))");
        query.bindValue(":id", obstracle.value());
        query.bindValue(":name", obstracle.value());
        query.bindValue(":latitude", obstracle.value());
        query.bindValue(":longitude", obstracle.value());
        query.bindValue(":orthometric_height", obstracle.value());
        query.bindValue(":airfield", idAirfield);
        query.bindValue(":last_updated", obstracle.value());
        if (!query.exec())
            qDebug() << query.lastError().text() << query.lastQuery() << query.boundValues();
    }
    query.exec("COMMIT");
//    if (!QSqlDatabase::database().commit()) {
//        QSqlDatabase::database().rollback();
//        qDebug() << "Rollback transaction";
//    }
}
