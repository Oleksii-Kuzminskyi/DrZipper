#ifndef ZIPPER_H_
#define ZIPPER_H_

#include <QtCore/QString>

#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

#include "FileSystemModel.h"
#include "Notification.h"
#include "DropArea.h"


class Zipper : public QWidget
{
    Q_OBJECT
private:
    FileSystemModel m_FS_model;
    QString m_FS_upload_item;
    QString m_FS_download_item;
    int m_mouseClicked_X;
    int m_mouseClicked_Y;

    QPushButton * m_CloseApp;
    QPushButton * m_FullScreen;
    QPushButton * m_SmallScreen;
    QPushButton * m_HiddenScreen;

    QLabel * m_Watermark;
    Notification * m_Notification;
    QLabel * m_Help;

    QTreeView * m_FS_uploader;
    DropArea * m_drop_area;
    QTreeView * m_FS_saver;

    QPushButton * m_download_button;
    QPushButton * m_clear_button;
    QPushButton * m_upload_button;
private:
    void setup_level_0(QHBoxLayout* & Level_0);
    void setup_level_1(QHBoxLayout* & Level_1);
    void setup_level_2(QHBoxLayout* & Level_2);
    void setup_level_3(QHBoxLayout* & Level_3);
    void setup_connections();
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
public:
    Zipper();
    Zipper(const Zipper & obj) = delete;
    ~Zipper();
public slots:
    void upload_select(const QModelIndex &index);
    void upload_select_and_push_back(const QModelIndex &index);
    void upload_push_back();

    void download_select(const QModelIndex &index);
    void download_select_and_push_back(const QModelIndex &index);
    void download_push_back();
public:
    Zipper & operator=(const Zipper & obj) = delete;
};














#endif
