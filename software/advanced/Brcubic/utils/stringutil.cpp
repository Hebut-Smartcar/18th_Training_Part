

#include "stringutil.h"

StringUtil::StringUtil()
{

}

char* StringUtil::QString2CharStr(QString &str) {
    QByteArray sr = str.toLocal8Bit();
    int len = sr.length();
    char* filename = new char[len+2];
    filename[len] = filename[len+1] = 0;
    strcpy(filename,sr.data());
    return filename;
}



