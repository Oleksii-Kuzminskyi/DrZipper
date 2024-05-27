#include "Notification.h"

//==========================constructors, destructor==============================

Notification::Notification(const QString & movie_file, QWidget * pwgt) : QLabel(pwgt)
{
    m_movie.setFileName(movie_file);
    m_movie.setSpeed(300);
    this->setMovie(&m_movie);

    m_movie.start();
    this->resize(m_movie.frameRect().size());
    m_movie.stop();

    //this->setPixmap(QPixmap(movie_file));
    QObject::connect(&m_movie, &QMovie::frameChanged, this, &Notification::OnFrameChanged);
}

Notification::~Notification()
{

}

//==========================slots==============================

/*private slot*/ void Notification::OnFrameChanged(int frame)
{
    if (frame == m_movie.frameCount() - 1)
    {
        m_movie.stop();
    }
}

/*slot*/ void Notification::display_problem(const QString & problem)
{
    m_ErrorMessage += problem;
    this->setToolTip(m_ErrorMessage);

    m_movie.start();
    this->resize(m_movie.frameRect().size());
}

/*slot*/ void Notification::clear_error_logs()
{
    m_ErrorMessage.clear();
    this->setToolTip("");
}










