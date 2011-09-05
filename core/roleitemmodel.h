#ifndef ROLEITEMMODEL_H
#define ROLEITEMMODEL_H

#include <QStandardItemModel>
#include <QByteArray>

/*!
 * Wrapper for QStandardItemModel.
 * RoleItemModel is used as the model for the search results. It's only function is to enable setting
 * model rolenames. See also http://gitorious.org/qmlreddit/qmlreddit/blobs/master/roleitemmodel.h
 */
class RoleItemModel : public QStandardItemModel{
    Q_OBJECT
public:
    RoleItemModel(QObject *parent = 0) : QStandardItemModel(parent){

    }

    int count()
    {
        return rowCount();
    }

    RoleItemModel(const QHash<int, QByteArray> &roleNames, QObject *parent = 0) : QStandardItemModel(parent){
        setRoleNames(roleNames);
    }



};
#endif
