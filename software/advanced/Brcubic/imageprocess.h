#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <cmath>


//#define RED 249
//#define BLUE 248
#define RED 2
#define BLUE 3
#define GREEN 4
#define YELLOW 5


// 尽量使用 stdint.h 定义的类型名称 避免冲突 这里可以裁剪
typedef unsigned char       uint8;                                              // 无符号  8 bits
typedef unsigned short int  uint16;                                             // 无符号 16 bits
typedef unsigned long int   uint32;                                             // 无符号 32 bits
typedef unsigned long long  uint64;                                             // 无符号 64 bits

typedef char                int8;                                               // 有符号  8 bits
typedef short int           int16;                                              // 有符号 16 bits
typedef long  int           int32;                                              // 有符号 32 bits
typedef long  long          int64;                                              // 有符号 64 bits

typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef struct image {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t step;
} image_t;

typedef struct fimage {
    float *data;
    uint32_t width;
    uint32_t height;
    uint32_t step;
} fimage_t;








enum direct{
    LEFT,
    RIGHT,
};  // 0左1右
extern enum direct yroad_direct;

enum yroad_type_e {
    YROAD_NONE = 0,                     // 非三岔路
    YROAD_FOUND,                        // 找到三岔，即两个Y角点
    YROAD_NEAR,                         // 三岔距离较近，即两个Y角点较近
    YROAD_LEFT_RUN, YROAD_RIGHT_RUN,    // 三岔内部
    YROAD_LEFT_OUT, YROAD_RIGHT_OUT,    // 出三岔路，即在三岔内部识别到两个Y角点
    YROAD_NUM,
};
extern enum yroad_type_e yroad_type;
enum cross_type_e {
    CROSS_NONE = 0,     // 非十字模式
    CROSS_BEGIN,        // 找到左右两个L角点
    CROSS_CROSS_1,        // 穿过十字(路口)
    CROSS_CROSS_2,        // 穿过十字(路口)
    CROSS_IN,           // 十字内(圆环)
    CROSS_NUM,
};
extern enum cross_type_e cross_type;
enum circle_type_e {
    CIRCLE_NONE = 0,                            // 非圆环模式
    CIRCLE_LEFT_BEGIN_1, CIRCLE_RIGHT_BEGIN_1,      // 圆环开始，识别到单侧L角点另一侧长直道。
    CIRCLE_LEFT_BEGIN_2, CIRCLE_RIGHT_BEGIN_2,      // 圆环开始，识别到单侧L角点另一侧长直道。
    CIRCLE_LEFT_IN, CIRCLE_RIGHT_IN,            // 圆环进入，即走到一侧直道，一侧圆环的位置。
    CIRCLE_LEFT_RUNNING, CIRCLE_RIGHT_RUNNING,  // 圆环内部。
    CIRCLE_LEFT_OUT, CIRCLE_RIGHT_OUT,          // 准备出圆环，即识别到出环处的L角点。
    CIRCLE_LEFT_END, CIRCLE_RIGHT_END,          // 圆环结束，即再次走到单侧直道的位置。
    CIRCLE_NUM,                                 //
};
extern enum circle_type_e circle_type;
enum garage_type_e {
    GARAGE_NONE = 0,                        // 非车库模式
    GARAGE_OUT_LEFT, GARAGE_OUT_RIGHT,      // 出库，陀螺仪转过45°，即出库完毕
    GARAGE_FOUND_LEFT, GARAGE_FOUND_RIGHT,  // 发现车库，即斑马线+单侧L角点
    GARAGE_IN_LEFT, GARAGE_IN_RIGHT,        // 进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_PASS_LEFT, GARAGE_PASS_RIGHT,    // 不进库，发现车库后判断第几次，从而决定是否进库
    GARAGE_STOP,                            // 进库完毕，停车
    GARAGE_NUM,
};
extern enum garage_type_e garage_type;








class ImageProcess : public QWidget
{
    Q_OBJECT
public:
    explicit ImageProcess(QWidget *parent = nullptr);
    uchar *  Imageprocess(uchar * origPic);
    uchar *  Imageprocess_OTSU(uchar *origPic);
    uchar *  Imageprocess_adaptive(uchar *origPic);

signals:

public slots:

private:

    void imageInit(uchar *Pic);
    uint8 OTSU(uint8 *IMG, uint16_t col, uint16_t row);
    void adaptiveThreshold(uint8_t* img_data, uint8_t* output_data, uint16_t width, uint16_t height, uint16_t block, uint8_t clip_value);

    void process_image(void);

    int clip(int x, int low, int up);
    float fclip(float x, float low, float up);
    void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
    void nms_angle(float angle_in[], int num, float angle_out[], int kernel, u8 index_out[], u8 *index_num);
    void blur_points(uint16_t pts_in[][2], int num, float pts_out[][2], int kernel);
    void findline_lefthand_adaptive(image_t *img, uint8_t block_size, uint8_t clip_value, uint16_t x, uint16_t y, uint16_t pts[][2], uint16_t *num);
    void findline_righthand_adaptive(image_t *img, uint8_t block_size, uint8_t clip_value, uint16_t x, uint16_t y, uint16_t pts[][2], uint16_t *num);
    void local_angle_points_int(u16 pts_in[][2], int num, float angle_out[], int dist);
    void find_line_up(u8 x00, u8 y00, u8 dir);
    void regression(int startline,int endline, uint16_t input[][2]);
    float my_sqrt(float number);
    void add_line_common(u8 type,  u16 pts0_in[][2], int num0, u16 pts1_in[][2], int num1, float pts_out[][2]);
    void track_line_common(u16 pts0_in[][2], int num0, u16 pts1_in[][2], int num1, float pts_out[][2], int *pts_num);
    u16 FindNumInArray(u16 array[][2], u8 num, u8 numofarray);
    void yroadjudge(void);
    void corssjudge(void);
    void circlejudge(void);
    void garagejudge(void);

};




#endif // IMAGEPROCESS_H
