#include "obstraclestyleditemdelegate.h"
#include <QDebug>
#include <QDateTime>
#include <QPainter>
#include <QSettings>

void ObstracleStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPainter *p(painter);
    QStyleOptionViewItem opt(option);
    QSettings settings;

    settings.beginGroup("database");
    QDateTime datetimeLastUpdated = settings.value("datetime_updated").toDateTime();
    settings.endGroup();

    // date update for entries in database
    QDateTime dateTime = QDateTime::fromString(index.model()->data(index.model()->index(index.row(), 0), Qt::UserRole + 2).toString(), "yyyy-MM-dd HH:mm:ss");
    // data update AIP
    QString dateUpdatedSource = index.model()->data(index.model()->index(index.row(), index.model()->columnCount() - 1)).toString();

    // new data 15 days
    if (QDate(dateUpdatedSource.left(2).prepend("20").toInt(), dateUpdatedSource.right(2).toInt(), 1).addDays(15) > dateTime.date()) {
        p->fillRect(opt.rect, QBrush(QColor(148, 243, 159, 64), Qt::SolidPattern));
    }
    if (datetimeLastUpdated.date() != dateTime.date()) {
        p->fillRect(opt.rect, QBrush(QColor(255, 70, 94, 64), Qt::SolidPattern));
    }
    QStyledItemDelegate::paint(painter, opt, index);
}
