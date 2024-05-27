#ifndef RESOURCE_LABEL_H_
#define RESOURCE_LABEL_H_

#include <QtCore/QEvent>
#include <QtCore/QString>

#include <QtGui/QMouseEvent>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

class ResourceLabel : public QLabel
{
    Q_OBJECT
private:
    QString m_ResourceName;
    QString m_current;
    QString m_Idle;
    QString m_Processing;
    QString m_Corrupted;
    QString m_Deletion;
    QString m_Finished;
signals:
    void delete_me(const QString &);
protected:
    virtual void enterEvent(QEvent * event) override;
    virtual void leaveEvent(QEvent * event) override;
    virtual void mousePressEvent(QMouseEvent * event) override;
public:
    enum state{Idle, Processing, Corrupted, Deletion, Finished};
public:
    ResourceLabel() = delete;
    explicit ResourceLabel(const QString & name);
    ResourceLabel(const ResourceLabel & obj) = delete;
    ~ResourceLabel();
public:
    ResourceLabel & operator=(const ResourceLabel & obj) = delete;
public:
    void init_state(ResourceLabel::state newstate, const QString & path_to_picture);
    void set_current_state(ResourceLabel::state newstate);
    void set_name(const QString & newname)
    {
        m_ResourceName = newname;
    }
    const QString & name() const
    {
        return m_ResourceName;
    }
};



#endif
