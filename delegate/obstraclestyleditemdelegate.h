#ifndef OBSTRACLESTYLEDITEMDELEGATE_H
#define OBSTRACLESTYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>

class ObstracleStyledItemDelegate : public QStyledItemDelegate
{
        Q_OBJECT

    public:
        explicit ObstracleStyledItemDelegate(QObject *parent = nullptr) { Q_UNUSED(parent) }
        ~ObstracleStyledItemDelegate() {}

    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // OBSTRACLESTYLEDITEMDELEGATE_H
