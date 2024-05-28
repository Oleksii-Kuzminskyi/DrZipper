#include <filesystem>
#include <string>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QChar>

#include "Resource.h"

#define qdebug() qDebug() << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__
namespace fs = std::filesystem;

//==========================constructors, destructor==============================
Resource::Resource()
{
    m_AbortCompression = false;
    m_AbortSaving = false;
    m_Compressed = false;
}


Resource::Resource(const QString & name)
{
    m_AbortCompression = false;
    m_AbortSaving = false;
    m_Compressed = false;
    m_ResourceSize = 0;

    if (!name.isEmpty() && fs::exists(qPrintable(name)))
    {
        m_OriginalName = name;
        QFileInfo info(name);
        m_PureName = info.fileName();

        if (info.isDir())
        {
            const fs::path targetdir{qPrintable(m_OriginalName)};
            for (auto const & dir_entry : fs::recursive_directory_iterator{targetdir})
            {
                if (dir_entry.is_directory() || dir_entry.is_symlink())
                {
                    continue;
                }
                QString temp = QString::fromStdString(dir_entry.path().string());
                m_ResourceSize += QFileInfo(temp).size();
            }
        }
        else
        {
            m_ResourceSize = info.size();
        }
    }
    else
    {
        if (name.isEmpty())
        {
            qdebug() << "Warning: empty path..";
        }
        if (!fs::exists(qPrintable(name)))
        {
            qdebug() << "Warning, path: " << name << " not exists";
        }
    }
}


Resource::~Resource()
{

}
//==========================operators==============================

/*friend*/ bool operator==(const Resource & obj1, const Resource & obj2)
{
    return (obj1.m_OriginalName == obj2.m_OriginalName);
}


//==========================methods=====================================

/*private*/ QString Resource::process_empty_dir(const char * zipname, zip_t* & zip_handler)
{
    zip_int64_t add_res = zip_dir_add(zip_handler, qPrintable(m_PureName), ZIP_FL_ENC_UTF_8);
    if (-1 == add_res)
    {
        qdebug() << "error placing: " << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error placing - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_discard(zip_handler);
        return QString("");
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            zip_discard(zip_handler);
            return QString("");
        }
    }

    //saving to disk
    if (-1 == zip_close(zip_handler))
    {
        qdebug() << "error saving" << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error saving - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_discard(zip_handler);
        return QString("");
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            fs::remove(fs::path(zipname));
            return QString("");
        }
    }
    return zipname;
}

/*private*/ QString Resource::compress_dir()
{
    //initialize archive handler
    QString zipname = m_OriginalName;
    zipname.remove('/');
    zipname.remove('\\');
    zipname += ".zip";

    zip_t * zip_handler = zip_open(qPrintable(zipname), ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (zip_handler)
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            zip_discard(zip_handler);
            return QString("");
        }
    }
    else
    {
        qdebug() << "error creating zip-handler: " << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error creating zip-handler - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        return QString("");
    }

    //check for empty dir
    if (QDir(m_OriginalName).isEmpty())
    {
        return this->process_empty_dir(qPrintable(zipname), zip_handler);
    }

    //compress all files/subdirs recursively
    const fs::path targetdir{qPrintable(m_OriginalName)};

    for (auto const & dir_entry : fs::recursive_directory_iterator{targetdir})
    {
        std::string current_entry = dir_entry.path().string();
        std::string relative_entry;

        if (dir_entry.is_symlink())
        {
            qDebug() << "warning: ignoring symlink" << QString::fromStdString(current_entry);

            m_ZipWarningError += m_OriginalName + ": warning - ignored symlink ";
            m_ZipWarningError += current_entry.c_str();
            m_ZipWarningError += "\n";

            continue;
        }
        else
        {

            relative_entry = (fs::relative(current_entry, m_OriginalName.toStdString())).string();
        }

        //qdebug() << "Original parent dir: " << m_OriginalName;
        //qdebug() << "Current entry: " << QString::fromStdString(current_entry);
        //qdebug() << "Relative entry to parent dir: " << QString::fromStdString(relative_entry);

        if (dir_entry.is_directory())
        {
            if (fs::is_empty(dir_entry))
            {
                zip_int64_t add_res = zip_dir_add(zip_handler, relative_entry.c_str(), ZIP_FL_ENC_UTF_8);
                if (-1 == add_res)
                {
                    qdebug() << "error placing: " << zip_strerror(zip_handler);

                    m_ZipWarningError += m_OriginalName;
                    m_ZipWarningError += ": error placing - ";
                    m_ZipWarningError += zip_strerror(zip_handler);
                    m_ZipWarningError +="\n";

                    zip_discard(zip_handler);
                    return QString("");
                }
                else
                {
                    if (m_AbortCompression == true)
                    {
                        qdebug() << "Abort compression of: " << m_OriginalName;

                        m_ZipWarningError += m_OriginalName;
                        m_ZipWarningError += ": abort compression\n";

                        zip_discard(zip_handler);
                        return QString("");
                    }
                }
            }
            continue;
        }

        //create zip-source
        zip_source_t * source = zip_source_file(zip_handler, current_entry.c_str(), 0, ZIP_LENGTH_TO_END);

        if (source)
        {
            if (m_AbortCompression == true)
            {
                qdebug() << "Abort compression of: " << m_OriginalName;

                m_ZipWarningError += m_OriginalName;
                m_ZipWarningError += ": abort compression\n";

                zip_source_free(source); //very carefully!
                zip_discard(zip_handler);
                return QString("");
            }
        }
        else
        {
            qdebug() << "error creating zip-source" << zip_strerror(zip_handler);

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": error creating zip-source - ";
            m_ZipWarningError += zip_strerror(zip_handler);
            m_ZipWarningError +="\n";

            zip_discard(zip_handler);
            return QString("");
        }

        //place zip-source to archive
        zip_int64_t add_res = zip_file_add(zip_handler, relative_entry.c_str(),
                                           source, ZIP_FL_ENC_UTF_8);
        if (-1 == add_res)
        {
            qdebug() << "error placing: " << zip_strerror(zip_handler);

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": error placing - ";
            m_ZipWarningError += zip_strerror(zip_handler);
            m_ZipWarningError +="\n";

            zip_source_free(source); //carefully !!!!
            zip_discard(zip_handler);
            return QString("");
        }
        else
        {
            if (m_AbortCompression == true)
            {
                qdebug() << "Abort compression of: " << m_OriginalName;

                m_ZipWarningError += m_OriginalName;
                m_ZipWarningError += ": abort compression\n";

                //zip_source_free(source); //looks like no need, see: https://libzip.org/documentation/zip_source_free.html
                zip_discard(zip_handler);
                return QString("");
            }
        }
    }

    //saving to disk
    if (-1 == zip_close(zip_handler))
    {
        qdebug() << "error saving to disk" << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error saving - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_discard(zip_handler);
        //zip_source_free(source); //looks like no need, see: https://libzip.org/documentation/zip_source_free.html
        return QString("");
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            fs::remove(fs::path(qPrintable(zipname)));
            return QString("");
        }
    }

    return zipname;
}

/*private*/ QString Resource::compress_file()
{
    //initialize archive handler
    QString zipname = m_OriginalName;
    zipname.remove('/');
    zipname.remove('\\');
    zipname += ".zip";

    zip_t * zip_handler = zip_open(qPrintable(zipname), ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (zip_handler)
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            zip_discard(zip_handler);
            return QString("");
        }
    }
    else
    {
        qdebug() << "error creating zip-handler: " << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error creating zip-handler - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        return QString("");
    }

    //create zip-source
    zip_source_t *source = zip_source_file(zip_handler, qPrintable(m_OriginalName), 0, ZIP_LENGTH_TO_END);
    if (source)
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            zip_source_free(source); //very carefully!
            zip_discard(zip_handler);
            return QString("");
        }
    }
    else
    {
        qdebug() << "   error creating zip-source" << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error creating zip-source - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_discard(zip_handler);
        return QString("");
    }

    //place zip-source to archive
    zip_int64_t add_res = zip_file_add(zip_handler, qPrintable(m_PureName), source, ZIP_FL_ENC_UTF_8);
    if (-1 == add_res)
    {
        qdebug() << "error placing: " << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error placing - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_source_free(source); //carefully!!!!
        zip_discard(zip_handler);
        return QString("");
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            //zip_source_free(source); //looks like no need, see: https://libzip.org/documentation/zip_source_free.html
            zip_discard(zip_handler);
            return QString("");
        }
    }

    //saving to disk
    if (-1 == zip_close(zip_handler))
    {
        qdebug() << "error saving to disk" << zip_strerror(zip_handler);

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error saving - ";
        m_ZipWarningError += zip_strerror(zip_handler);
        m_ZipWarningError +="\n";

        zip_discard(zip_handler);
        return QString("");
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            fs::remove(fs::path(qPrintable(zipname)));
            return QString("");
        }
    }

    return zipname;
}

bool Resource::compress()
{
    //qdebug() << "Entered here";

    QFileInfo info(m_OriginalName);
    QString zipname;

    //trivial check
    if (m_OriginalName.isEmpty())
    {
        qdebug() << "empty path. cannot perform zipping";
        m_ZipWarningError += "empty path. cannot perform zipping\n";
        emit something_wrong(m_ZipWarningError);
        emit compressed(m_OriginalName, false);
        m_ZipWarningError.clear();
        return false;
    }

    //compress whatever resource is
    if (info.isDir())
    {
        zipname = this->compress_dir();
    }
    else
    {
        zipname = this->compress_file();
    }

    if (zipname.isEmpty())
    {
        qdebug() << "Error compressing resource";

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": error compressing resource\n";

        emit something_wrong(m_ZipWarningError);
        emit compressed(m_OriginalName, false);
        m_ZipWarningError.clear();
        return false;
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            fs::remove(fs::path(qPrintable(zipname)));
            emit something_wrong(m_ZipWarningError);
            emit compressed(m_OriginalName, false);
            m_ZipWarningError.clear();
            return false;
        }
    }

    QFile file_handler(zipname);
    if (!file_handler.open(QIODevice::ReadOnly))
    {
        qdebug() << "Could not open file";

        m_ZipWarningError += m_OriginalName;
        m_ZipWarningError += ": could not open file\n";

        fs::remove(fs::path(qPrintable(zipname)));
        emit compressed(m_OriginalName, false);
        emit something_wrong(m_ZipWarningError);
        m_ZipWarningError.clear();

        return false;
    }
    else
    {
        if (m_AbortCompression == true)
        {
            qdebug() << "Abort compression of: " << m_OriginalName;

            m_ZipWarningError += m_OriginalName;
            m_ZipWarningError += ": abort compression\n";

            file_handler.remove();
            file_handler.close();
            emit compressed(m_OriginalName, false);
            emit something_wrong(m_ZipWarningError);
            m_ZipWarningError.clear();

            return false;
        }
    }

    m_FileBuf = file_handler.readAll();
    m_Compressed = true;

    file_handler.remove();
    file_handler.close();

    qdebug() << "FINISHED COMPRESSING";
    emit compressed(m_OriginalName, true);

    if (!m_ZipWarningError.isEmpty())
    {
        //only some warning appeared
        emit something_wrong(m_ZipWarningError);
        m_ZipWarningError.clear();
    }

    return true;
}

/*private*/ bool Resource::check_save_all(const QString & location)
{
    if (m_FileBuf.isEmpty())
    {
        qdebug() << "nothing to save.exit";
        m_SaveWarningError += m_OriginalName + ": nothing to save yet\n";
        return false;
    }
    if (m_Compressed == false)
    {
        qDebug() << "Compression not finished yet";
        m_SaveWarningError += m_OriginalName + ": compression not finished yet\n";
        return false;
    }
    if (location.isEmpty())
    {
        qdebug() << "Empty save path";
        m_SaveWarningError += m_OriginalName + ": empty save path\n";
        return false;
    }
    if (!fs::exists(qPrintable(location)))
    {
        qdebug() << "unexisting path";
        m_SaveWarningError += m_OriginalName + ": unexisting path\n";
        return false;
    }
    if (!QFileInfo(location).isDir())
    {
        qdebug() << "Not a directory to save file";
        m_SaveWarningError += m_OriginalName + ": not a directory\n";
        return false;
    }

    return true;
}

/*private*/ void Resource::finalize_save_path(QString & downloadPath)
{
    QChar slash = QDir::separator();
    if (!downloadPath.endsWith(slash))
    {
        downloadPath.append(slash);
    }
    downloadPath.append(m_PureName + ".zip");
}

bool Resource::save(const QString & location)
{
    if (!this->check_save_all(location))
    {
        m_SaveWarningError += m_OriginalName + ": error saving\n";
        emit something_wrong(m_SaveWarningError);
        m_SaveWarningError.clear();
        return false;
    }
    QString downloadPath = location;
    this->finalize_save_path(downloadPath);
    qdebug() << "Final save path: " << downloadPath;

    QFile file_handler(downloadPath);
    if (file_handler.open(QIODevice::WriteOnly))
    {
        if (m_AbortSaving == true)
        {
            qdebug() << "Abort saving: " << downloadPath;
            file_handler.remove();
            file_handler.close();

            m_SaveWarningError += m_OriginalName + ": abort saving\n";
            emit something_wrong(m_SaveWarningError);
            m_SaveWarningError.clear();

            return false;
        }
    }
    else
    {
        qdebug() << "Could not open file";

        m_SaveWarningError += m_OriginalName + ": could not open file\n";
        emit something_wrong(m_SaveWarningError);
        m_SaveWarningError.clear();

        return false;
    }

    qint64 bytesWritten = file_handler.write(m_FileBuf);
    if (-1 == bytesWritten)
    {
        qdebug() << "Error writing data to file";
        file_handler.close();

        m_SaveWarningError += m_OriginalName + ": error writing data to file\n";
        emit something_wrong(m_SaveWarningError);
        m_SaveWarningError.clear();

        return false;
    }

    file_handler.close();

    return true;
}













