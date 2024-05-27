#ifndef FILE_SYSTEM_MODEL_H_
#define FILE_SYSTEM_MODEL_H_

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>

#include <QtGui/QIcon>

#include <QtWidgets/QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
public:
    FileSystemModel(QObject * parent = nullptr) : QFileSystemModel(parent)
    {

    }
public:
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DecorationRole)
        {
            if (this->isDir(index))
            {
                return QIcon(":/FS_folder.svg");
            }
            else
            {
                return QIcon(":/FS_file.svg");
            }
        }
        return QFileSystemModel::data(index, role);
    }
};



#endif
