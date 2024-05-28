#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <QtCore/QString>

#include <QtGui/QMovie>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>

class Notification : public QLabel
{
    Q_OBJECT
private:
    //to think: size limit string?
    QMovie m_movie;
    QString m_ErrorMessage;
public:
    Notification(const QString & movie_file, QWidget * pwgt = nullptr);
    ~Notification();
private slots:
    void OnFrameChanged(int frame);
public slots:
    void display_problem(const QString & problem);
    void clear_error_logs();
};



#endif
