#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include "Zipper.h"

QString read_content(const QString & filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        return QString("");
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}


//==========================constructors, destructor==============================
Zipper::Zipper()
{
    //===================non-GUI setup================
    m_FS_model.setRootPath(QDir::rootPath());
    m_FS_model.setReadOnly(true);
    m_FS_model.setFilter(QDir::Dirs | QDir::Files | QDir::NoDot| QDir::NoDotDot| QDir::NoDotAndDotDot);


    //===================GUI setup================
    QHBoxLayout * Level_0 = new QHBoxLayout;
    this->setup_level_0(Level_0);

    QHBoxLayout * Level_1 = new QHBoxLayout;
    this->setup_level_1(Level_1);

    QHBoxLayout * Level_2 = new QHBoxLayout;
    this->setup_level_2(Level_2);

    QHBoxLayout * Level_3 = new QHBoxLayout;
    this->setup_level_3(Level_3);

    QVBoxLayout * layout_main = new QVBoxLayout;
    layout_main->addLayout(Level_0);
    layout_main->addLayout(Level_1);
    layout_main->addLayout(Level_2);
    layout_main->addLayout(Level_3);
    this->setLayout(layout_main);

    this->setup_connections();
}


Zipper::~Zipper()
{

}


//==========================slots==============================

void Zipper::upload_select(const QModelIndex &index)
{
    m_FS_upload_item = m_FS_model.filePath(index);
    qDebug() << "selected path: " << m_FS_upload_item;
}

void Zipper::upload_select_and_push_back(const QModelIndex &index)
{
    m_FS_upload_item = m_FS_model.filePath(index);
    qDebug() << "selected path: " << m_FS_upload_item;
    qDebug() << "start pushing: " << m_FS_upload_item;
    m_drop_area->insert(m_FS_upload_item);
}

void Zipper::upload_push_back()
{
    qDebug() << "start pushing: " << m_FS_upload_item;
    m_drop_area->insert(m_FS_upload_item);
}

void Zipper::download_select(const QModelIndex &index)
{
    m_FS_download_item = m_FS_model.filePath(index);
    qDebug() << "selected download path: " << m_FS_upload_item;
}

void Zipper::download_select_and_push_back(const QModelIndex &index)
{
    m_FS_download_item = m_FS_model.filePath(index);
    qDebug() << "selected download path: " << m_FS_download_item;
    qDebug() << "start downloading to: " << m_FS_download_item;
    m_drop_area->save(m_FS_download_item);
}

void Zipper::download_push_back()
{
    qDebug() << "start downloading to: " << m_FS_download_item;
    m_drop_area->save(m_FS_download_item);
}

//==========================methods==============================

/*virtual*/ void Zipper::mousePressEvent(QMouseEvent *event)
{
    m_mouseClicked_X = event->x();
    m_mouseClicked_Y = event->y();
}

/*virtual*/ void Zipper::mouseMoveEvent(QMouseEvent *event)
{
    //https://forum.qt.io/topic/119008/framelesswindowhint-removes-the-abbility-to-drag-move-the-window
    this->move(event->globalX() - m_mouseClicked_X, event->globalY() - m_mouseClicked_Y);
}

/*virtual*/ void Zipper::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(255, 255, 255, 64));
    pen.setWidth(10);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.end();
}


/*private*/ void Zipper::setup_connections()
{
    QObject::connect(m_CloseApp, &QPushButton::clicked, this, &Zipper::close);
    QObject::connect(m_FullScreen, &QPushButton::clicked, this, &Zipper::showFullScreen);
    QObject::connect(m_SmallScreen, &QPushButton::clicked, this, &Zipper::showNormal);
    QObject::connect(m_HiddenScreen, &QPushButton::clicked, this, &Zipper::showMinimized);

    QObject::connect(m_FS_uploader, &QTreeView::clicked, this, &Zipper::upload_select);
    QObject::connect(m_FS_uploader, &QTreeView::doubleClicked, this, &Zipper::upload_select_and_push_back);
    QObject::connect(m_drop_area, &DropArea::problem_occured, m_Notification, &Notification::display_problem);
    QObject::connect(m_FS_saver, &QTreeView::doubleClicked, this, &Zipper::download_select_and_push_back);
    QObject::connect(m_FS_saver, &QTreeView::clicked, this, &Zipper::download_select);

    QObject::connect(m_download_button, &QPushButton::clicked, this, &Zipper::upload_push_back);

    QObject::connect(m_clear_button, &QPushButton::clicked, m_drop_area, &DropArea::clear_resources);
    QObject::connect(m_clear_button, &QPushButton::clicked, m_Notification, &Notification::clear_error_logs);

    QObject::connect(m_upload_button, &QPushButton::clicked, this, &Zipper::download_push_back);
}

/*private*/ void Zipper::setup_level_0(QHBoxLayout* & Level_0)
{
    //Watermark
    m_Watermark = new QLabel("by Alex Kuzminskyi");
    m_Watermark->setStyleSheet("QLabel \
                               { color: rgba(255, 255, 255, 128); \
                                           font-size: 40 px; \
                                           font-weight: bold; \
                                           font-family: Consolas, monospace; \
                               }");

    //Help
    m_Help = new QLabel;
    m_Help->setPixmap(QPixmap(":/info.png"));
    m_Help->setToolTip(read_content(":/greetings.txt")); //to do: add full description
    m_Help->setStyleSheet("Qlabel {font-family: Consolas, monospace;}");

    m_CloseApp = new QPushButton;
    m_CloseApp->setIcon(QIcon(":/close.svg"));
    m_CloseApp->setIconSize(QSize(38, 38));
    m_CloseApp->setMinimumWidth(38);
    m_CloseApp->setStyleSheet("QPushButton { border: none;}");

    m_FullScreen = new QPushButton;
    m_FullScreen->setIcon(QIcon(":/maximize.svg"));
    m_FullScreen->setIconSize(QSize(32, 32));
    m_FullScreen->setMinimumWidth(32);
    m_FullScreen->setStyleSheet("QPushButton { border: none;}");

    m_SmallScreen = new QPushButton;
    m_SmallScreen->setIcon(QIcon(":/minimize.svg"));
    m_SmallScreen->setIconSize(QSize(32, 32));
    m_SmallScreen->setMinimumWidth(32);
    m_SmallScreen->setStyleSheet("QPushButton { border: none;}");


    m_HiddenScreen = new QPushButton;
    m_HiddenScreen->setIcon(QIcon(":/shrink.svg"));
    m_HiddenScreen->setIconSize(QSize(32, 32));
    m_HiddenScreen->setMinimumWidth(32);
    m_HiddenScreen->setStyleSheet("QPushButton { border: none;}");


    QHBoxLayout * temp = new QHBoxLayout;
    temp->setAlignment(Qt::AlignCenter);
    temp->addWidget(m_Help);
    temp->addWidget(m_Watermark);

    QHBoxLayout * temp2 = new QHBoxLayout;
    temp2->setAlignment(Qt::AlignCenter);
    temp2->addWidget(m_HiddenScreen);
    temp2->addWidget(m_SmallScreen);
    temp2->addWidget(m_FullScreen);
    temp2->addWidget(m_CloseApp);


    Level_0->addLayout(temp);
    Level_0->setAlignment(temp, Qt::AlignTop | Qt::AlignLeft);
    Level_0->addLayout(temp2);
    Level_0->setAlignment(temp2, Qt::AlignTop | Qt::AlignRight);
}

/*private*/ void Zipper::setup_level_1(QHBoxLayout* & Level_1)
{
    //Notification
    m_Notification = new Notification(":/notification.gif");

    Level_1->addWidget(m_Notification);
    Level_1->setAlignment(m_Notification, Qt::AlignTop | Qt::AlignCenter);
}

/*private*/ void Zipper::setup_level_2(QHBoxLayout* & Level_2)
{
    //FS observer-uploader
    m_FS_uploader = new QTreeView;
    m_FS_uploader->setModel(&m_FS_model);
    for (int i = 1; i < m_FS_model.columnCount(); i++)
    {
        m_FS_uploader->setColumnHidden(i, true); //hide 'size', 'permissions', 'date', etc.
    }
    m_FS_uploader->setMinimumWidth(200);
    m_FS_uploader->setMaximumWidth(400);

    //Drop Area
    m_drop_area = new DropArea;
    m_drop_area->setMinimumWidth(m_FS_uploader->width());

    //FS observer-downloader
    m_FS_saver = new QTreeView;
    m_FS_saver->setModel(&m_FS_model);
    for (int i = 1; i < m_FS_model.columnCount(); i++)
    {
        m_FS_saver->setColumnHidden(i, true);
    }
    m_FS_saver->setMinimumWidth(m_FS_uploader->minimumWidth());
    m_FS_saver->setMaximumWidth(m_FS_uploader->maximumWidth());

    Level_2->addWidget(m_FS_uploader);
    Level_2->addWidget(m_drop_area);
    Level_2->addWidget(m_FS_saver);
}

/*private*/ void Zipper::setup_level_3(QHBoxLayout* & Level_3)
{
    //'Download' buttton
    m_download_button = new QPushButton;
    m_download_button->setIcon(QIcon(":/download.png"));
    m_download_button->setIconSize(QSize(32, 32));
    m_download_button->setMinimumWidth(m_FS_uploader->minimumWidth());
    m_download_button->setMaximumWidth(m_FS_uploader->maximumWidth());

    //'Clear' button
    m_clear_button = new QPushButton;
    m_clear_button->setIcon(QIcon(":/delete.png"));
    m_clear_button->setIconSize(QSize(32, 32));
    m_clear_button->setMinimumWidth(m_drop_area->minimumWidth());
    m_clear_button->setMaximumWidth(m_drop_area->maximumWidth());

    //'Upload' button
    m_upload_button = new QPushButton;
    m_upload_button->setIcon(QIcon(":/upload.png"));
    m_upload_button->setIconSize(QSize(32, 32));
    m_upload_button->setMinimumWidth(m_FS_saver->minimumWidth());
    m_upload_button->setMaximumWidth(m_FS_saver->maximumWidth());

    Level_3->addWidget(m_download_button);
    Level_3->addWidget(m_clear_button);
    Level_3->addWidget(m_upload_button);
}
