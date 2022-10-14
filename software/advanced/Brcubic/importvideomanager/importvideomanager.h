

#ifndef IMPORTVIDEOMANAGER_H
#define IMPORTVIDEOMANAGER_H

#include <QThread>
#include <QList>
#include <QString>

#include "../utils/directoryutil.h"
#include "../utils/stringutil.h"


//note : support import file type:  mp4 avi mov
class ImportVideoManager : public QThread
{
    Q_OBJECT
public:
    ImportVideoManager();
    ~ImportVideoManager() override;

    enum ErrorCode {
        NoError = 0,
        NotSetPathError,
        NotSetVideoError,
        OpenVideoError,
        FileExistError
    };

    void SetInputPath(QString file_path);
    int Import();
    void DoSaveVideo();


signals:
    void onImportProgress(bool is_finished, int percent);


private :
    void run() override;

private:

    QString input_file_path_and_name_;

    int64_t frame_count_;
    int width_;
    int height_;

    char* input_filename;

    unsigned char *r_byte_p;
    unsigned char *g_byte_p;
    unsigned char *b_byte_p;
    uint current_frame_count_ = 0;

};


#endif // IMPORTVIDEOMANAGER_H
