#include "ResourceLabel.h"

//==========================constructors, destructor==============================
ResourceLabel::ResourceLabel(const QString & name)
{
    m_ResourceName = name;
}

ResourceLabel::~ResourceLabel()
{

}

//==========================methods==============================
/*virtual*/ void ResourceLabel::enterEvent(QEvent * event)
{
    Q_UNUSED(event);
    this->setPixmap(QPixmap(m_Deletion));
}
/*virtual*/ void ResourceLabel::leaveEvent(QEvent * event)
{
    Q_UNUSED(event);
    this->setPixmap(QPixmap(m_current));
}

/*virtual*/ void ResourceLabel::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    emit delete_me(m_ResourceName);
}

void ResourceLabel::init_state(ResourceLabel::state newstate, const QString & path_to_picture)
{
    switch (newstate)
    {
    case Idle:
        m_Idle = path_to_picture;
        break;
    case Processing:
        m_Processing = path_to_picture;
        break;
    case Corrupted:
        m_Corrupted = path_to_picture;
        break;
    case Deletion:
        m_Deletion = path_to_picture;
        break;
    case Finished:
        m_Finished = path_to_picture;
        break;
    }
}

void ResourceLabel::set_current_state(ResourceLabel::state newstate)
{
    switch (newstate)
    {
    case Idle:
        m_current = m_Idle;
        break;
    case Processing:
        m_current = m_Processing;
        break;
    case Corrupted:
        m_current = m_Corrupted;
        break;
    case Deletion:
        m_current = m_Deletion;
        break;
    case Finished:
        m_current = m_Finished;
        break;
    default:
        m_current = m_Corrupted;
        break;
    }

    this->setPixmap(QPixmap(m_current));
}








