#include <filesystem>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QtTest/QTest>
#include <QThread>
#include <QFileInfo>


#include "Resource.h"

// ========================================================================
class Test_ResourceClass : public QObject
{
  Q_OBJECT
private slots:
  void Zip_File_Unexisting();
  void Zip_File_Unexisting_SaveToFile();
  void Zip_File_Unexisting_SaveToFolderUnexisting();
  void Zip_File_Unexisting_SaveToFolderUnexisting_Slash();

  void Zip_File_Existing();
  void Zip_File_Existing_SaveToFile();
  void Zip_File_Existing_SaveToFolderUnexisting();
  void Zip_File_Existing_SaveToFolderExisting();
  void Zip_File_Existing_SaveToFolderExisting_Slash();

  void Zip_Folder_Unexisting();
  void Zip_Folder_Unexisting_SaveToFile();
  void Zip_Folder_Unexisting_SaveToFolderUnexisting();
  void Zip_Folder_Unexisting_SaveToFolderUnexisting_Slash();

  void Zip_Folder_Empty_Existing();
  void Zip_Folder_Empty_Existing_SaveToFile();
  void Zip_Folder_Empty_Existing_SaveToFolderUnexisting();
  void Zip_Folder_Empty_Existing_SaveToFolderUnexisting_Slash();
  void Zip_Folder_Empty_Existing_SaveToFolderExisting();
  void Zip_Folder_Empty_Existing_SaveToFolderExisting_Slash();

  void Zip_Folder_Full_Existing();
  void Zip_Folder_Full_Existing_SaveToFile();
  void Zip_Folder_Full_Existing_SaveToFolderUnexisting();
  void Zip_Folder_Full_Existing_SaveToFolderUnexisting_Slash();
  void Zip_Folder_Full_Existing_SaveToFolderExisting();
  void Zip_Folder_Full_Existing_SaveToFolderExisting_Slash();

  void Zip_Folder_Symlink();

  void Zip_File_Async_Abort();
  void Zip_File_Async_Abort_2();
  void Zip_File_Async_Abort_3();
  void Zip_File_Aync_Abort_ptr();

  void Zip_Folder_Async_Abort();
  void Zip_Folder_Async_Abort_2();

  void Zip_File_Double_Save();

  void Save_File_Async_Abort();
  void Save_Folder_Async_Abort();

  void File_Size();
  void Folder_Size();
};

void Test_ResourceClass::Zip_File_Unexisting()
{
  Resource resource("unexisting.txt");
  QCOMPARE(resource.compress(), false);
  QCOMPARE(resource.isEmpty(), true);
}

void Test_ResourceClass::Zip_File_Unexisting_SaveToFile()
{
    Resource resource("unexisting.txt");
    resource.compress();
    QCOMPARE(resource.save("/some/file.txt"), false);
}

void Test_ResourceClass::Zip_File_Unexisting_SaveToFolderUnexisting()
{
    Resource resource("unexisting.txt");
    resource.compress();
    QCOMPARE(resource.save("/some/folder"), false);
}

void Test_ResourceClass::Zip_File_Unexisting_SaveToFolderUnexisting_Slash()
{
    Resource resource("unexisting.txt");
    resource.compress();
    QCOMPARE(resource.save("/some/folder/"), false);
}




void Test_ResourceClass::Zip_File_Existing()
{
    Resource resource("ForTests/test.txt");
    QCOMPARE(resource.compress(), true);
    QCOMPARE(resource.isEmpty(), false);
}

void Test_ResourceClass::Zip_File_Existing_SaveToFile()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QCOMPARE(resource.save("/some/file.txt"), false);
}

void Test_ResourceClass::Zip_File_Existing_SaveToFolderUnexisting()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QCOMPARE(resource.save("/some/folder"), false);
}

void Test_ResourceClass::Zip_File_Existing_SaveToFolderExisting()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QCOMPARE(resource.save("ForTests"), true);
    QCOMPARE(std::filesystem::exists("ForTests/test.txt.zip"), true);
    std::filesystem::remove("ForTests/test.txt.zip");
}

void Test_ResourceClass::Zip_File_Existing_SaveToFolderExisting_Slash()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QCOMPARE(resource.save("ForTests/"), true);
    QCOMPARE(std::filesystem::exists("ForTests/test.txt.zip"), true);
    std::filesystem::remove("ForTests/test.txt.zip");
}





void Test_ResourceClass::Zip_Folder_Unexisting()
{
    Resource resource("some/folder");
    QCOMPARE(resource.compress(), false);
    QCOMPARE(resource.isEmpty(), true);
}

void Test_ResourceClass::Zip_Folder_Unexisting_SaveToFile()
{
    Resource resource("some/folder");
    resource.compress();
    QCOMPARE(resource.save("/some/file.txt"), false);
}

void Test_ResourceClass::Zip_Folder_Unexisting_SaveToFolderUnexisting()
{
    Resource resource("some/folder");
    resource.compress();
    QCOMPARE(resource.save("/some/folder"), false);
}

void Test_ResourceClass::Zip_Folder_Unexisting_SaveToFolderUnexisting_Slash()
{
    Resource resource("some/folder");
    resource.compress();
    QCOMPARE(resource.save("/some/folder/"), false);
}




void Test_ResourceClass::Zip_Folder_Empty_Existing()
{
    Resource resource("ForTests/empty");
    QCOMPARE(resource.compress(), true);
    QCOMPARE(resource.isEmpty(), false);
}

void Test_ResourceClass::Zip_Folder_Empty_Existing_SaveToFile()
{
    Resource resource("ForTests/empty");
    resource.compress();
    QCOMPARE(resource.save("some/file.txt"), false);
}

void Test_ResourceClass::Zip_Folder_Empty_Existing_SaveToFolderUnexisting()
{
    Resource resource("ForTests/empty");
    resource.compress();
    QCOMPARE(resource.save("some/unexisting/folder"), false);
}

void Test_ResourceClass::Zip_Folder_Empty_Existing_SaveToFolderUnexisting_Slash()
{
    Resource resource("ForTests/empty");
    resource.compress();
    QCOMPARE(resource.save("some/unexisting/folder/"), false);
}

void Test_ResourceClass::Zip_Folder_Empty_Existing_SaveToFolderExisting()
{
    Resource resource("ForTests/empty");
    resource.compress();
    QCOMPARE(resource.save("ForTests"), true);
    QCOMPARE(std::filesystem::exists("ForTests/empty.zip"), true);
    std::filesystem::remove("ForTests/empty.zip");
}

void Test_ResourceClass::Zip_Folder_Empty_Existing_SaveToFolderExisting_Slash()
{
    Resource resource("ForTests/empty");
    resource.compress();
    QCOMPARE(resource.save("ForTests/"), true);
    QCOMPARE(std::filesystem::exists("ForTests/empty.zip"), true);
    std::filesystem::remove("ForTests/empty.zip");
}




void Test_ResourceClass::Zip_Folder_Full_Existing()
{
    Resource resource("ForTests/non_empty");
    QCOMPARE(resource.compress(), true);
    QCOMPARE(resource.isEmpty(), false);
}

void Test_ResourceClass::Zip_Folder_Full_Existing_SaveToFile()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QCOMPARE(resource.save("ForTests/test.txt"), false);
}

void Test_ResourceClass::Zip_Folder_Full_Existing_SaveToFolderUnexisting()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QCOMPARE(resource.save("some/unexisting/folder"), false);
}

void Test_ResourceClass::Zip_Folder_Full_Existing_SaveToFolderUnexisting_Slash()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QCOMPARE(resource.save("some/unexisting/folder/"), false);
}

void Test_ResourceClass::Zip_Folder_Full_Existing_SaveToFolderExisting()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QCOMPARE(resource.save("ForTests"), true);
    QCOMPARE(std::filesystem::exists("ForTests/non_empty.zip"), true);
    std::filesystem::remove("ForTests/non_empty.zip");
}

void Test_ResourceClass::Zip_Folder_Full_Existing_SaveToFolderExisting_Slash()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QCOMPARE(resource.save("ForTests/"), true);
    QCOMPARE(std::filesystem::exists("ForTests/non_empty.zip"), true);
    std::filesystem::remove("ForTests/non_empty.zip");
}

void Test_ResourceClass::Zip_Folder_Symlink()
{
    Resource resource("ForTests/symlink");
    QCOMPARE(resource.compress(), true);
    QCOMPARE(resource.save("ForTests/"), true);
    QCOMPARE(std::filesystem::exists("ForTests/symlink.zip"), true);
    std::filesystem::remove("ForTests/symlink.zip");
}

void Test_ResourceClass::Zip_File_Async_Abort()
{
    Resource resource("ForTests/hugefile.txt");
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::compress);
    resource.abort_compression();
    result.waitForFinished();
}


void Test_ResourceClass::Zip_File_Async_Abort_2()
{
    Resource resource("ForTests/hugefile.txt");
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::compress);
    QThread::usleep(1);
    resource.abort_compression();
    result.waitForFinished();
}

void Test_ResourceClass::Zip_File_Async_Abort_3()
{
    Resource resource("ForTests/hugefile.txt");
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::compress);
    QThread::msleep(875);
    resource.abort_compression();
    result.waitForFinished();
}


void Test_ResourceClass::Zip_File_Aync_Abort_ptr()
{
    Resource * resource = new Resource("ForTests/hugefile.txt");
    QFuture<bool> result = QtConcurrent::run(resource, &Resource::compress);
    resource->abort_compression();
    result.waitForFinished();
    delete resource;
}


void Test_ResourceClass::Zip_Folder_Async_Abort()
{
    Resource resource("ForTests/huge_folder");
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::compress);
    resource.abort_compression();
    result.waitForFinished();
}

void Test_ResourceClass::Zip_Folder_Async_Abort_2()
{
    Resource resource("ForTests/huge_folder");
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::compress);
    QThread::usleep(500);
    resource.abort_compression();
    result.waitForFinished();
}


void Test_ResourceClass::Zip_File_Double_Save()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QCOMPARE(resource.save("ForTests"), true);
    QCOMPARE(resource.save("ForTests"), true);
    std::filesystem::remove("ForTests/test.txt.zip");
}


void Test_ResourceClass::Save_File_Async_Abort()
{
    Resource resource("ForTests/test.txt");
    resource.compress();
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::save, QString("ForTests"));
    resource.abort_saving();
    result.waitForFinished();
}

void Test_ResourceClass::Save_Folder_Async_Abort()
{
    Resource resource("ForTests/non_empty");
    resource.compress();
    QFuture<bool> result = QtConcurrent::run(&resource, &Resource::save, QString("ForTests"));
    resource.abort_saving();
    result.waitForFinished();
    std::filesystem::remove("ForTests/non_empty.zip");
}

void Test_ResourceClass::File_Size()
{
    Resource resource("ForTests/hugefile.txt");
    qDebug() << "File size: " << resource.size();
    QCOMPARE(resource.size(), 100'000'000);

}

void Test_ResourceClass::Folder_Size()
{
    Resource resource("ForTests/non_empty");
    qDebug() << "Folder size: " << resource.size();
    QCOMPARE(resource.size(), 76'176);
}



QTEST_MAIN(Test_ResourceClass)
#include "main.moc"


