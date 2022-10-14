
#include "directoryutil.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>


DirectoryUtil::DirectoryUtil()
{

}

bool DirectoryUtil::IsDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    return false;
}

bool DirectoryUtil::IsFileExist(QString fullFilePath)
{
    QFileInfo fileInfo(fullFilePath);
    if(fileInfo.exists())
    {
        return true;
    }
    return false;
}

bool DirectoryUtil::CreatePath(QString dir) {
    QDir qdir;
    return qdir.mkpath(dir);
}

QString DirectoryUtil::GetVirtualVideoCacheDir() {

    QStringList path_list1 = QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation);
    QString dir =  path_list1[0] + "/"+SUB_DIR;
    //D << "GenericCacheLocation" << dir;

    QStringList path_list2 = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    QString dir2 =  path_list2[0] + "/"+SUB_DIR;
   // D << "DataLocation" << dir2;

    QStringList path_list3 = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString dir3 =  path_list3[0] + "/"+SUB_DIR;
   // D << "AppDataLocation" << dir3;

    return dir;
}
