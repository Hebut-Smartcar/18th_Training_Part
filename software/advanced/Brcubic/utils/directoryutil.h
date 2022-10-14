
#ifndef DIRECTORYUTIL_H
#define DIRECTORYUTIL_H

#include <QString>

const static QString SUB_DIR = "HYKT";

class DirectoryUtil
{
private:
    DirectoryUtil();

public:
    static QString GetVirtualVideoCacheDir();
    static bool IsDirExist(QString fullPath);
    static bool IsFileExist(QString fullFilePath);
    static bool CreatePath(QString dir);

};



#endif // DIRECTORYUTIL_H
