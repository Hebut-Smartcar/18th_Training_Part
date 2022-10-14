

#include "exportvideomanager.h"

extern "C" {

#include "../ffmpeg/outputstream.h"
}


ExportVideoManager::ExportVideoManager()
{

}

ExportVideoManager::~ExportVideoManager()
{

}

void ExportVideoManager::run() {
    char* filename = StringUtil::QString2CharStr(abs_export_file_path_);

    int width = 200;
    int height = 300;

    unsigned char *r_byte;
    unsigned char *g_byte;
    unsigned char *b_byte;

    int index_base;

    int total_frame_count = 25;
    int current_frame_count = 0;
    float write_out_percent;

    if( open_media_file(filename, width,height) != 0) {
        goto CLEAN;
    }

    r_byte  = new unsigned char[width * height];
    g_byte  = new unsigned char[width * height];
    b_byte  = new unsigned char[width * height];

    for(int y = 0; y < height; y ++)
        for(int x = 0; x < width; x ++) {
            index_base = y * width + x;

            r_byte[index_base] = 30;
            g_byte[index_base] = 30;
            b_byte[index_base] = 30;

            if(y > 20)
            r_byte[index_base] = 255;

            if(y > 60)
            g_byte[index_base] = 255;
            if(y > 200)
            b_byte[index_base] = 255;
        }

    for (int k = 0; k < 25; k++) {
        printf("output frame count = %d \n", k+1);
        write_media_file(r_byte, g_byte, b_byte);

        current_frame_count ++;

        write_out_percent = static_cast<float>(current_frame_count) /
                                                            static_cast<float>(total_frame_count);
        emit onExportProgress(false, static_cast<int>(write_out_percent * 100));
    }

    delete r_byte;
    delete g_byte;
    delete b_byte;

CLEAN:
     close_media_file();
     delete[] filename;
     emit onExportProgress(true, 100);
}


int ExportVideoManager::Export(QString abs_file_path) {

    abs_export_file_path_ = abs_file_path;

    //start thread to output
    start();
    emit onExportProgress(false, 0);
    return NoError;
}


