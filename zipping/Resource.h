#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>

#include "zip.h"

class Resource : public QObject
{
    Q_OBJECT
private:
    QString m_OriginalName;
    QString m_PureName;
    long long m_ResourceSize;
    QByteArray m_FileBuf;
    bool m_AbortCompression; //for async running
    bool m_AbortSaving;      //for async running
    bool m_Compressed;
    QString m_ZipWarningError;
    QString m_SaveWarningError;
private:
    QString compress_dir();
    QString process_empty_dir(const char * zipname, zip_t* & zip_handler);
    QString compress_file();
    bool check_save_all(const QString & location);
    void finalize_save_path(QString & downloadPath);
signals:
    void compressed(const QString & resource, bool compress_status);
    void something_wrong(QString message);
public:
    Resource(); //carefully
    explicit Resource(const QString & name);
    Resource(const Resource & obj) = delete;
    ~Resource();
public:
    Resource & operator=(const Resource & obj) = delete;
    friend bool operator==(const Resource & obj1, const Resource & obj2);
public:
    bool compress();
    bool save(const QString & location);
    void abort_compression()
    {
        m_AbortCompression = true;
    }
    void abort_saving()
    {
        m_AbortSaving = true;
    }
public:
    bool isEmpty() const
    {
        return m_FileBuf.isEmpty();
    }
    const QString & name() const
    {
        return m_OriginalName;
    }
    const QString & short_name() const
    {
        return m_PureName;
    }
    bool isReady() const
    {
        return m_Compressed;
    }
    long long size() const
    {
        return m_ResourceSize;
    }
};




#endif
