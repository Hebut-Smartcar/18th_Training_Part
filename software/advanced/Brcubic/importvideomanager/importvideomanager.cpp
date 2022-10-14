

#include "importvideomanager.h"

extern "C" {

#include "../ffmpeg/inputstream.h"
}

void static read_callback(void *caller_p) {
    ImportVideoManager* instance = static_cast<ImportVideoManager*>(caller_p);
    instance->DoSaveVideo();
}


ImportVideoManager::ImportVideoManager()
{

}

ImportVideoManager::~ImportVideoManager()
{

}



void ImportVideoManager::SetInputPath(QString file_path) {
    input_file_path_and_name_ = file_path;
}

int ImportVideoManager::Import() {
    if(DirectoryUtil::IsFileExist(input_file_path_and_name_) == false) {
        return FileExistError;
    }

    //open ffmepg
    input_filename = StringUtil::QString2CharStr(input_file_path_and_name_);
    int retvalue = open_import_file(input_filename);
    if(retvalue != 0) {
        //E << "open_import_file error     .input_filename="<<input_filename << "code="<<retvalue;
        return OpenVideoError;
    }
    get_video_info(&frame_count_, &width_, &height_);
   // D << "frame_count=" << frame_count_ << "  width=" << width_ << "   height=" << height_;

    //start thread to input
    start();
    emit onImportProgress(false, 0);


    return NoError;
}

void ImportVideoManager::run() {


    int array_len = width_ * height_;
    r_byte_p = new unsigned char[array_len];
    g_byte_p = new unsigned char[array_len];
    b_byte_p = new unsigned char[array_len];

    set_read_callback(this, read_callback);
    read_a_frame(r_byte_p, g_byte_p, b_byte_p);

    //close ffmpeg
    //close the virtual video
    close_import_file();

    delete[] input_filename;

    delete[] r_byte_p;
    delete[] g_byte_p;
    delete[] b_byte_p;

    emit onImportProgress(true, 100);
}

void ImportVideoManager::DoSaveVideo() {
    //read frame width and height
    width_;
    height_;

    //todo: save the read frame  at here
    //read frame data, you need copy the rbg array to your array
    //returned rgb frame is made by three channel, list as below
    r_byte_p;//the r channel of frame, size = width_ * height_
    g_byte_p;//the g channel of frame, size = width_ * height_
    b_byte_p;//the b channel of frame, size = width_ * height_

    //calc import percent of progress
    float write_out_percent;
    current_frame_count_ ++;
    write_out_percent = static_cast<float>(current_frame_count_) /
                                                        static_cast<float>(frame_count_);
    emit onImportProgress(false, static_cast<int>(write_out_percent * 100));
}




