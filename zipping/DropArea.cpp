#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QFont>

#include <QtWidgets/QLabel>


#include "DropArea.h"


//==============constructors, destructor=======================

DropArea::DropArea(QWidget * pwgt) : QWidget(pwgt)
{
    QWidget::setAcceptDrops(true);
    m_ResourceOrder = new QGridLayout;
    m_ResourceOrder->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setLayout(m_ResourceOrder);
}

DropArea::~DropArea()
{
    //abort possible async compressions
    for (auto map_element = m_ResourceMap.begin(); map_element != m_ResourceMap.end(); ++map_element)
    {
        map_element->second.abort_compression();
    }

    for (auto future_element = m_FutureCompressMap.begin(); future_element != m_FutureCompressMap.end(); ++future_element)
    {
        future_element->second.waitForFinished();
    }

    for (auto future_element = m_FutureSaveMap.begin(); future_element != m_FutureSaveMap.end(); ++future_element)
    {
        future_element->second.waitForFinished();
    }
}





//==============events=======================

/*virtual*/ void DropArea::dragEnterEvent(QDragEnterEvent * pDragEvent)
{
    if (pDragEvent->mimeData()->hasUrls())
    {
        pDragEvent->acceptProposedAction();
    }
}

/*virtual*/ void DropArea::dropEvent(QDropEvent * pDropEvent)
{
    foreach(QUrl element, pDropEvent->mimeData()->urls())
    {
        QString result_path = element.toLocalFile();
        qDebug() << "Dropped:" << result_path;
        this->insert(result_path);
    }
}

/*virtual*/ void DropArea::paintEvent(QPaintEvent * )
{
    QPainter painter;

    painter.begin(this);

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(QColor(255, 255, 255, 128));
    pen.setWidth(5);
    pen.setDashPattern({2, 8});
    painter.setPen(pen);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    QFont font("Consolas", 15);
    font.setWeight(QFont::DemiBold);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, "drag your files or folders here");


    painter.end();
}



//==============methods=======================

/*private*/ void DropArea::add_to_layout(const QString & resource)
{
    int max_colummns = 5;
    int current_row = 0;

    for (int column = 0; column < max_colummns; column++)
    {
        qDebug() << "pos: " << current_row << " " << column;
        if (m_ResourceOrder->itemAtPosition(current_row, column))
        {
            qDebug() << "not free";
            if (column == max_colummns - 1)
            {
                qDebug() << "column overflow -> proceed with next row";
                current_row++;
                column = -1; //start again
                continue;
            }
            else
            {
                qDebug() << "proceed with next column";
                continue;
            }
        }
        else
        {
            qDebug() << "free";

            ResourceLabel * resource_pic = new ResourceLabel(resource);
            resource_pic->init_state(ResourceLabel::Idle, ":/folder_white.png");
            resource_pic->init_state(ResourceLabel::Processing, ":/folder_white.png");
            resource_pic->init_state(ResourceLabel::Corrupted, ":/warning.png");
            resource_pic->init_state(ResourceLabel::Deletion, ":/delete.png");
            resource_pic->init_state(ResourceLabel::Finished, ":/zip-file.png");
            resource_pic->set_current_state(ResourceLabel::Idle);
            QObject::connect(resource_pic, &ResourceLabel::delete_me,
                             this,         &DropArea::remove_resource);

            QLabel * resource_name = new QLabel(Resource(resource).short_name());

            QVBoxLayout * temp = new QVBoxLayout;
            temp->addWidget(resource_pic);
            temp->setAlignment(resource_pic, Qt::AlignTop | Qt::AlignCenter);
            temp->addWidget(resource_name);
            temp->setAlignment(resource_name, Qt::AlignTop | Qt::AlignCenter);
            m_ResourceOrder->addLayout(temp, current_row, column);

            m_ResourceOrder->setColumnStretch(column, 1);
            break;
        }
    }

}

/*private*/ void DropArea::redraw_resource(const QString & name, const ResourceLabel::state status)
{
    qDebug() << "redrawing resource: " << name;
    for (int row = 0; row < m_ResourceOrder->rowCount(); row++)
    {
        for (int col = 0; col < m_ResourceOrder->columnCount(); col++)
        {
            qDebug() << "cell:" << row << " " << col;
            if (!m_ResourceOrder->itemAtPosition(row, col))
            {
                qDebug() << "empty";
                continue;
            }

            QVBoxLayout * vbox = qobject_cast<QVBoxLayout*>(m_ResourceOrder->itemAtPosition(row, col)->layout());
            if (!vbox)
            {
                qDebug() << "NO vbox";
                vbox = nullptr;
                continue;
            }

            ResourceLabel * resource_pic = qobject_cast<ResourceLabel*>(vbox->itemAt(0)->widget());

            //to think: change text as well?
            if (!resource_pic)
            {
                qDebug() << "NO picture in vbox";
                vbox = nullptr;
                resource_pic = nullptr;
                continue;
            }

            if (resource_pic->name() == name)
            {
                qDebug() << "GOTCHA";
                resource_pic->set_current_state(status);
                return; //resource changed
            }
            else
            {
                qDebug() << "Not that name";
                vbox = nullptr;
                resource_pic = nullptr;
                continue;
            }

        }
    }

}

/*private*/ long long DropArea::size() const
{
    long long total_resource_size = 0;
    for (auto resource = m_ResourceMap.begin(); resource != m_ResourceMap.end(); ++resource)
    {
        // qDebug() << "resource->second.short_name(): " << resource->second.short_name();
        // qDebug() << "resource->second.size(): " << resource->second.size();
        total_resource_size += resource->second.size();
    }
    return total_resource_size;
}


bool DropArea::insert(const QString & name)
{
    //to do: check resource size
    qDebug() << "current size: " << this->size();
    qDebug() << "input resource size: " << Resource(name).size();

    if (this->size() + Resource(name).size() >= MAX_UPLOAD_SIZE_LIMIT)
    {
        qDebug() << "upload limit exceeded";
        emit problem_occured("Upload size limit reached. Please remove some resources\n");
        return  false;
    }

    if (m_ResourceMap.size() >= MAX_ELEMENTS)
    {
        qDebug() << "Overflow";
        emit problem_occured("Upload resource number limit reached. Please remove some resources\n");
        return false;
    }

    if (m_ResourceMap.find(name) != m_ResourceMap.end())
    {
        qDebug() << "duplicate";
        emit problem_occured(name + " already uploaded\n");
        return false;
    }

    m_ResourceMap.emplace(name, name);
    this->add_to_layout(name);

    QObject::connect(&m_ResourceMap[name], &Resource::compressed, this, &DropArea::get_status);
    QObject::connect(&m_ResourceMap[name], &Resource::something_wrong, this, &DropArea::problem_occured);

    m_FutureCompressMap[name] = QtConcurrent::run(&m_ResourceMap[name], &Resource::compress);

    return true;
}

void DropArea::save(const QString & path)
{
    for (auto resource = m_ResourceMap.begin(); resource != m_ResourceMap.end(); ++resource)
    {
        m_FutureSaveMap[resource->second.name()] = \
            QtConcurrent::run(&(resource->second), &Resource::save, path);
    }
}




//==============slots=======================

/*slot*/ void DropArea::clear_resources()
{
    qDebug() << "Try to remove all";


    for (auto map_element = m_ResourceMap.begin(); map_element != m_ResourceMap.end(); ++map_element)
    {
        map_element->second.abort_compression();
    }

    for (auto future_element = m_FutureCompressMap.begin(); future_element != m_FutureCompressMap.end(); ++future_element)
    {
        future_element->second.waitForFinished();
    }

    for (auto future_element = m_FutureSaveMap.begin(); future_element != m_FutureSaveMap.end(); ++future_element)
    {
        future_element->second.waitForFinished();
    }

    m_ResourceMap.clear();
    m_FutureCompressMap.clear();
    m_FutureSaveMap.clear();

    //clear UI
    for (int row = 0; row < m_ResourceOrder->rowCount(); row++)
    {
        for (int col = 0; col < m_ResourceOrder->columnCount(); col++)
        {
            qDebug() << "cell:" << row << " " << col;
            if (!m_ResourceOrder->itemAtPosition(row, col))
            {
                qDebug() << "empty";
                continue;
            }

            QVBoxLayout * vbox = qobject_cast<QVBoxLayout*>(m_ResourceOrder->itemAtPosition(row, col)->layout());
            if (!vbox)
            {
                qDebug() << "NO vbox";
                vbox = nullptr;
                continue;
            }

            ResourceLabel * resource_pic = qobject_cast<ResourceLabel*>(vbox->itemAt(0)->widget());

            QLabel * resource_text = qobject_cast<QLabel*>(vbox->itemAt(1)->widget());

            if (!resource_pic || !resource_text)
            {
                qDebug() << "NO label in vbox";
                vbox = nullptr;
                resource_pic = nullptr;
                resource_text = nullptr;
                continue;
            }

            qDebug() << "GOTCHA";
            delete resource_pic;
            delete resource_text;
            delete vbox;
            delete m_ResourceOrder->itemAtPosition(row, col);
        }
    }

}

/*slot*/ void DropArea::remove_resource(const QString & name)
{
    qDebug() << "Try to remove: " << name;
    qDebug() << "Resources: " << m_ResourceMap.size();
    qDebug() << "Compress threads:" << m_FutureCompressMap.size();
    qDebug() << "Save threads: " << m_FutureSaveMap.size();


    if (m_ResourceMap.find(name) == m_ResourceMap.end())
    {
        qDebug() << "could not find: " << name;
        emit problem_occured(name + " not found");
        return;
    }

    m_ResourceMap[name].abort_saving();
    m_ResourceMap[name].abort_compression();

    m_FutureCompressMap[name].waitForFinished();
    m_FutureSaveMap[name].waitForFinished();

    m_ResourceMap.erase(name);
    m_FutureCompressMap.erase(name);
    m_FutureSaveMap.erase(name);

    //clear UI
    for (int row = 0; row < m_ResourceOrder->rowCount(); row++)
    {
        for (int col = 0; col < m_ResourceOrder->columnCount(); col++)
        {
            qDebug() << "cell:" << row << " " << col;
            if (!m_ResourceOrder->itemAtPosition(row, col))
            {
                qDebug() << "empty";
                continue;
            }

            QVBoxLayout * vbox = qobject_cast<QVBoxLayout*>(m_ResourceOrder->itemAtPosition(row, col)->layout());
            if (!vbox)
            {
                qDebug() << "NO vbox";
                vbox = nullptr;
                continue;
            }

            ResourceLabel * resource_pic = qobject_cast<ResourceLabel*>(vbox->itemAt(0)->widget());

            QLabel * resource_text = qobject_cast<QLabel*>(vbox->itemAt(1)->widget());

            if (!resource_pic || !resource_text)
            {
                qDebug() << "NO label in vbox";
                vbox = nullptr;
                resource_pic = nullptr;
                resource_text = nullptr;
                continue;
            }

            if (resource_pic->name() == name)
            {
                qDebug() << "GOTCHA";
                delete resource_pic;
                delete resource_text;
                delete vbox;
                delete m_ResourceOrder->itemAtPosition(row, col);
                return; //success
            }
        }
    }

}

/*slot*/ void DropArea::get_status(const QString & resource, bool compressed_OK)
{
    qDebug() << "REDRAWING RESULT";
    if (compressed_OK == true)
    {
        this->redraw_resource(resource, ResourceLabel::Finished);
    }
    else
    {
        this->redraw_resource(resource, ResourceLabel::Corrupted);
    }
}





