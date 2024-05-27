#ifndef DROP_AREA_H_
#define DROP_AREA_H_

#include <map>

#include <QtCore/QString>

#include <QtConcurrent/QtConcurrent>

#include <QtGui/QPaintEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>

#include "Resource.h"
#include "ResourceLabel.h"

class DropArea : public QWidget
{
    Q_OBJECT
private:
    static const int MAX_ELEMENTS = 40;
    static const long long MAX_UPLOAD_SIZE_LIMIT = 500'000'000;
    typedef QFuture<bool> FutureBool;

    std::map<QString, Resource> m_ResourceMap;
    std::map<QString, FutureBool> m_FutureCompressMap;
    std::map<QString, FutureBool> m_FutureSaveMap;
    QGridLayout * m_ResourceOrder;
private:
    void add_to_layout(const QString & resource);
    void redraw_resource(const QString & name, const ResourceLabel::state status);
    long long size() const;
protected:
    virtual void dragEnterEvent(QDragEnterEvent * pDragEvent) override;
    virtual void dropEvent(QDropEvent * pDropEvent) override;
    virtual void paintEvent(QPaintEvent * ) override;
public:
    DropArea(QWidget * pwgt = nullptr);
    DropArea(const DropArea & obj) = delete;
    ~DropArea();
public:
    DropArea & operator=(const DropArea & obj) = delete;
public:
    bool insert(const QString & name);
    void save(const QString & path);
    bool is_full() const
    {
        return (m_ResourceMap.size() == MAX_ELEMENTS);
    }
signals:
    void problem_occured(const QString & problem);
public slots:
    void clear_resources();
    void remove_resource(const QString & name);
    void get_status(const QString & resource, bool compressed_OK);
    void listen_problem(QString message)
    {
        emit problem_occured(message);
    }
};





#endif
