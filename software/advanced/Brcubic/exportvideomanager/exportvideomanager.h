

#ifndef EXPORTVIDEOMANAGER_H
#define EXPORTVIDEOMANAGER_H

#include <QThread>
#include <QList>
#include <QString>

#include "../utils/directoryutil.h"
#include "../utils/stringutil.h"

//note : support export file type:  mp4 avi mov
class ExportVideoManager : public QThread
{
    Q_OBJECT
public:
    ExportVideoManager();
    ~ExportVideoManager() override;

    enum ErrorCode {
        NoError = 0,
        NotSetPathError,
        NotSetVideoError,
        OpenVideoError,
        FileExistError
    };

    int Export(QString abs_file_path);

signals:
    void onExportProgress(bool is_finished, int percent);


private :
    void run() override;

private:


private:
    const QString suffix_ = ".mp4";

    QString abs_export_file_path_;
};




#endif // EXPORTVIDEOMANAGER_H
