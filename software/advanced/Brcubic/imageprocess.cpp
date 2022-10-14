#include "imageprocess.h"
#include <math.h>


#define MT9V03X_DVP_W               188
#define MT9V03X_DVP_H               120
#define MT9V03X_HALVED_W               MT9V03X_DVP_W/2
#define MT9V03X_HALVED_H               MT9V03X_DVP_H/2

uint8 Image_Use[MT9V03X_HALVED_H][MT9V03X_HALVED_W];  //灰度图
uint8_t halvedImage[MT9V03X_HALVED_H][MT9V03X_HALVED_W];
uint8_t *halvedImage_addr = halvedImage[0];
uint8 Pixle[MT9V03X_HALVED_H][MT9V03X_HALVED_W];      //二值化图像

#define POINTS_MAX_LEN  (MT9V03X_HALVED_H*2)//(MT9V03X_HALVED_H)
#define AT                  AT_IMAGE
#define AT_CLIP(img, x, y)  AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));
#define AT_IMAGE(img, x, y)          ((img)->data[(y)*(img)->step+(x)])  //读取图像像素点数据  step为一行有多少数据
#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))
#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}
#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}


image_t img_raw = DEF_IMAGE(NULL, MT9V03X_HALVED_W, MT9V03X_HALVED_H);  //定义图像结构体 数据指向空



uint8 Midlinecompensationvalue = 10;  //单边找中线时对校正数组的校正值 大概为10
uint8 half_width_group[MT9V03X_HALVED_H]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                       10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                       20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                                       30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                                       40, 41, 42, 43, 44, 45, 46, 47, 48, 49};

// 原图左右边线   采集到的边线数组小下标的y值大
uint16_t ipts0[POINTS_MAX_LEN][2];  //左
uint16_t ipts1[POINTS_MAX_LEN][2];  //右
uint16_t ipts0_num, ipts1_num;
// 二次巡线的边线
uint16_t ipts0_2[POINTS_MAX_LEN][2];  //左
uint16_t ipts1_2[POINTS_MAX_LEN][2];  //右
uint16_t ipts0_2_num, ipts1_2_num;
// 向上巡线的边线
uint16_t ipts2[POINTS_MAX_LEN][2];
uint16_t ipts2_num;
// 变换后左右边线+滤波
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;

float rpts2b[POINTS_MAX_LEN][2];
u16 rpts2b_num;

// 左右边线局部角度变化率
float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];
int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];
int rpts0an_num, rpts1an_num;

float rpts2a[POINTS_MAX_LEN];
int rpts2a_num;
float rpts2an[POINTS_MAX_LEN];
int rpts2an_num;

// 非极大抑制后的角点索引值
uint8 rpts0index[20];
uint8 rpts1index[20];
uint8 rpts0index_num, rpts1index_num;

uint8 rpts2index[20];
uint8 rpts2index_num;

//中线
float rptsc[POINTS_MAX_LEN][2];
int rpts_num;
//边线退出记录
uint16 edgeL_exist[MT9V03X_DVP_H];
uint16 edgeR_exist[MT9V03X_DVP_H];
// Y角点
int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
bool Ypt0_found, Ypt1_found;

int Ypt2_rpts2s_id;
bool Ypt2_found;

// L角点
int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
bool Lpt0_found, Lpt1_found;
// 长直道
bool is_straight0, is_straight1;
// 弯道
bool is_turn0, is_turn1;


/***************************************************** 摄像头参数 *****************************************/
#define CAMERA_HEIGHT     // 220


/***************************************************** 卷积核参数 *****************************************/
float block_size = 7;   //卷积核大小
float clip_value = 10;   //自适应二值化经验值 4

float begin_x = 15;   // 8
float begin_y = 55;  // 最大55
float line_blur_kernel = 7;   //三角滤波滤波核大小
float sample_dist = 0.04;  //0.02
float angle_dist = 0.2;

uint8 gray[127] = {0};


/***************************************************** 预锚点 *****************************************/
u8 Preanchorpoint = MT9V03X_HALVED_H;  // 预锚点纵坐标
u8 Pre_point[2];  // 预锚点坐标



float parameterB = 0;  //斜率
float parameterA = 0;  //截距
float A, B;


/***************************************************** 识别斑马线参数 *****************************************/
#define GARAGE_NUM_TRES 5  // 识别斑马线的阈值 3 5 6 7
float BLACK_THRES = MT9V03X_HALVED_W/3;  // 底部黑色像素点数量阈值
u8 thres = 150;  //灰度图下黑色阈值



/***************************************************** 元素识别计数 *****************************************/
// 当元素识别计数大于阈值，即为判断到了元素。在元素中不用。
#define judge_num 3
u8 cross_judge = 0;
u8 yroad_judge = 0;
u8 garage_judge = 0;
u8 circle_judge = 0;
u8 ramp_judge = 0;
#define ADD_ELEMENT(element) (element < 255)?element++:element = element
#define ZERO_ELEMENT(element) element = 0
#define JUDGE_ELEMENT(element) (element >= judge_num)?1:0


/***************************************************** 三叉的参数 *****************************************/

enum direct yroad_direct = RIGHT;


// 初始化元素状态
enum yroad_type_e yroad_type = YROAD_NONE;
enum cross_type_e cross_type = CROSS_NONE;
enum circle_type_e circle_type = CIRCLE_NONE;
enum garage_type_e garage_type = GARAGE_OUT_LEFT;


/***************************************************** 三叉的参数 *****************************************/
u8 up_coner_point[2][2];   // 三叉上角点
u8 up_coner_point_flag[2]; // 三叉上角点标志
u8 cross_rpts[2][2];  // 十字上角点x和y




ImageProcess::ImageProcess(QWidget *parent) : QWidget(parent)
{

}




/**
* @brief       大津法计算二值化阈值
* @param       threshold
* @return      threshold 二值化阈值
*/
uint8 ImageProcess::OTSU(uint8 *IMG, uint16_t col, uint16_t row)   //注意计算阈值的一定要是原图像
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    uint32 gray_sum=0;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;

    uint8* data = IMG;  //指向像素数据的指针
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (i = 0; i < GrayScale; i++) {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }


    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2) {
        for (j = 0; j < width; j+=2) {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++) {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    //遍历灰度级[0,255]
    for (j = 0; j < GrayScale; j++) {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax) {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax) {
            break;
        }
    }
    return threshold;
}

/**
* @brief       自适应二值化
* @param       adaptiveThreshold
* @return      None
*/
void ImageProcess::adaptiveThreshold(uint8_t* img_data, uint8_t* output_data, uint16_t width, uint16_t height, uint16_t block, uint8_t clip_value)
{
    assert(block % 2 == 1); // block必须为奇数
    int half_block = block / 2;
    for(int y = half_block; y < height-half_block; y++){
        for(int x = half_block; x < width-half_block; x++){
            // 计算局部阈值
            int thres = 0;
            for(int dy=-half_block; dy<=half_block; dy++){
                for(int dx=-half_block; dx<=half_block; dx++){
                    thres += img_data[(x+dx)+(y+dy)*width];
                }
            }
            thres = thres / (block * block) - clip_value;
            // 进行二值化
            output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
        }
    }
}





/************************************************** 图像处理主函数 **********************************************/
uchar *a = new uchar[MT9V03X_DVP_W*MT9V03X_DVP_H];
uchar *ImageProcess::Imageprocess(uchar *origPic)
{

    imageInit(origPic);
    img_raw.data = halvedImage_addr;  //数据指针指向halvedImage_addr
    for (int i=0;i<MT9V03X_HALVED_H;i++) {
        for(int j=0;j<MT9V03X_HALVED_W;j++) {
            a[i*MT9V03X_HALVED_W+j] = halvedImage[i][j];
        }
    }

    process_image();

#if 0
    // 向上寻找角点
    find_line_up(MT9V03X_HALVED_W/2-10, MT9V03X_HALVED_H);
    qDebug() << "up_Y_found: " << Ypt2_found;
#endif

#if 0
    // 最小二乘拟合直线  （角点处）
    regression(clip(rpts1index[0]-10, 0, MT9V03X_HALVED_H),   clip(rpts1index[0], clip(rpts1index[0]-10, 0, MT9V03X_HALVED_H)+1, MT9V03X_HALVED_H), ipts1);
    // 画出角点
    a[ipts1[rpts1index[0]][1]*MT9V03X_HALVED_W + ipts1[rpts1index[0]][0]] = GREEN;
    // 画出拟合的直线
    for(int i = 1; i < MT9V03X_HALVED_W; i++) {
        a[int(clip((B*i+A), 0, MT9V03X_HALVED_H)*MT9V03X_HALVED_W) + i] = GREEN;
    }
#endif

#if 0
    // 丢线寻中线
    add_line_common(0, ipts0, ipts0_num, ipts1, ipts1_num, rptsc);
#endif

#if 0
    // 常规寻中线
    track_line_common(ipts0, ipts0_num, ipts1, ipts1_num, rptsc, &rpts_num);
#endif


    // 元素判定和寻线
#if 1
    Ypt2_found = 0;
    garagejudge();
    yroadjudge();
//    corssjudge();
    circlejudge();

#endif


    // 正常状况寻线
#if 1
    if(yroad_type == YROAD_NONE && cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && garage_type == GARAGE_NONE) {
        Preanchorpoint = MT9V03X_HALVED_H/2;

        u8 xx = 0, yy = 0;
        if(ipts0_num < ipts1_num/2 && ipts0_num < MT9V03X_HALVED_H/2) {  // 左边线短，寻右线
            xx = ipts1[ipts1[0][1]-Preanchorpoint][0] - (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else if(ipts1_num < ipts0_num/2 && ipts1_num < MT9V03X_HALVED_H/2) {  // 右边线短，寻左线
            xx = ipts0[ipts0[0][1]-Preanchorpoint][0] + (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else {  // 双边寻线
            xx = (u8)(ipts0[ipts0[0][1]-Preanchorpoint][0] + ipts1[ipts1[0][1]-Preanchorpoint][0])/2;
            yy = Preanchorpoint;
        }

        /* 写入预锚点 */
        Pre_point[0] = xx;
        Pre_point[1] = yy;
    }

#endif


#if 1
    // 绘制边线
    for(u8 i = 0; i < POINTS_MAX_LEN; i++) {

        if(i <= ipts0_num) a[ipts0[i][1]*MT9V03X_HALVED_W + ipts0[i][0]] = RED;
        if(i <= ipts1_num) a[ipts1[i][1]*MT9V03X_HALVED_W + ipts1[i][0]] = BLUE;

        //if(i <= ipts2_num) a[ipts2[i][1]*MT9V03X_HALVED_W + ipts2[i][0]] = GREEN;

        //if(i <= rpts_num) a[int(rptsc[i][1]*MT9V03X_HALVED_W) + int(rptsc[i][0])] = GREEN;
        //if(i <= ipts2_num) a[ipts2[i][1]*MT9V03X_HALVED_W + ipts2[i][0]] = GREEN;

        a[cross_rpts[0][1]*MT9V03X_HALVED_W+cross_rpts[0][0]] = GREEN;
        a[cross_rpts[1][1]*MT9V03X_HALVED_W+cross_rpts[1][0]] = GREEN;

        a[Pre_point[1]*MT9V03X_HALVED_W + Pre_point[0]] = YELLOW;  // 标记预锚点
    }
#endif
    return a;
}

/************************************************** 图像处理主函数 **********************************************/


void ImageProcess::imageInit(uchar *Pic)
{
/******图像数组赋值***********/
    for (int i=0;i<MT9V03X_HALVED_H;i++) {
        for(int j=0;j<MT9V03X_HALVED_W;j++) {
            halvedImage[i][j] = 13*sqrt(Pic[i*MT9V03X_HALVED_W+j]);  // 对图像进行γ变换
            //halvedImage[i][j] = Pic[i*MT9V03X_HALVED_W+j];
        }
    }
}



uchar *ImageProcess::Imageprocess_OTSU(uchar *origPic)
{
    uchar *a = new uchar[MT9V03X_DVP_W*MT9V03X_DVP_H];
    imageInit(origPic);
/*************二值化***********/
    uint8 thres = OTSU(halvedImage[0], MT9V03X_HALVED_W, MT9V03X_HALVED_H);
    for (int i=0;i<MT9V03X_HALVED_H;i++) {
        for(int j=0;j<MT9V03X_HALVED_W;j++) {
            a[i*MT9V03X_HALVED_W+j] = halvedImage[i][j] > thres ? 255:0;
        }
    }
    return a;
}
uchar *ImageProcess::Imageprocess_adaptive(uchar *origPic)
{
    uchar *a = new uchar[MT9V03X_DVP_W*MT9V03X_DVP_H];
    imageInit(origPic);
/*************二值化***********/
    adaptiveThreshold(halvedImage[0], Pixle[0], MT9V03X_HALVED_W, MT9V03X_HALVED_H, 7, 4);
    for (int i=0;i<MT9V03X_HALVED_H;i++) {
        for(int j=0;j<MT9V03X_HALVED_W;j++) {
            a[i*MT9V03X_HALVED_W+j] = Pixle[i][j];
        }
    }
    return a;
}
/************************************************** 处理函数 **********************************************/
/************************************************** 处理函数 **********************************************/
/************************************************** 处理函数 **********************************************/
/************************************************** 处理函数 **********************************************/





void ImageProcess::yroadjudge(void)
{
    if(yroad_type == YROAD_NONE && cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && garage_type == GARAGE_NONE) {  //在对应状态条件下执行
        if(rpts0index_num && rpts1index_num && (ipts0_num != MT9V03X_HALVED_H*2 && ipts1_num != MT9V03X_HALVED_H*2)) {
            if(ipts0[rpts0index[0]][1] >= MT9V03X_HALVED_H/2 && ipts1[rpts1index[0]][1] >= MT9V03X_HALVED_H/2 && fabs(ipts1[rpts1index[0]][1]-ipts0[rpts0index[0]][1]) <= 7) {  // 判断角点横坐标和纵坐标特性
                qDebug() << "double point";


                // 寻找上面两个角点
                /* 这里有待改进，因为巡线算法在图像边缘不能正常运行，所以输入值限制最小4，需要改进巡线算法 */
                find_line_up(clip(ipts0[rpts0index[0]][0]-6, 4, MT9V03X_HALVED_W-4), clip(ipts0[rpts0index[0]][1]-6, 3, MT9V03X_HALVED_H), 0);
                if(rpts2index_num) {
                    up_coner_point[0][0] = ipts2[rpts2index[0]][0]; up_coner_point[0][1] = ipts2[rpts2index[0]][1];
                    up_coner_point_flag[0] = 1;
                }
                else up_coner_point_flag[0] = 0;

                find_line_up(clip(ipts1[rpts1index[0]][0]+6, 4, MT9V03X_HALVED_W-4), clip(ipts1[rpts1index[0]][1]-6, 3, MT9V03X_HALVED_H), 1);
                if(rpts2index_num) {
                    up_coner_point[1][0] = ipts2[rpts2index[0]][0]; up_coner_point[1][1] = ipts2[rpts2index[0]][1];
                    up_coner_point_flag[1] = 1;
                }
                else up_coner_point_flag[1] = 0;


                // 根据上面两个角点的特征来判断十字和三叉
                if(up_coner_point_flag[0] && up_coner_point_flag[1]) {  // 上方有两个角点
                    if(fabs(up_coner_point[0][0]-up_coner_point[1][0]) >= 25 && up_coner_point[0][1] >= 5 && up_coner_point[1][1] >= 5) {  // 横坐标和纵坐标特性
                        cross_type = CROSS_BEGIN;  // 判断为十字
                    }
                    else if(fabs(up_coner_point[0][0]-up_coner_point[1][0]) <= 10) {  // 横坐标特性 （两个角点距离太近）
                        yroad_type = YROAD_FOUND;
                    }
                }
                else yroad_type = YROAD_FOUND;  // 没有两个角点就是三叉



            }
        }
        else if(rpts0an_num && !rpts1an_num) {  // 左边有角点

        }
        else if(!rpts0an_num && rpts1an_num) {  // 右边有角点

        }
    }

/************************************************** 处理三叉 **********************************************/

    // 找到三叉
    else if(yroad_type == YROAD_FOUND) {
        if(rpts0an_num || rpts1an_num) {
            if(ipts0[rpts0index[0]][1] >= MT9V03X_HALVED_H/2-10 || ipts1[rpts1index[0]][1] >= MT9V03X_HALVED_H/2-10) {  // 靠近三叉
                yroad_type = YROAD_NEAR;

                Preanchorpoint = MT9V03X_HALVED_H/2-10;  // 设置预锚点
            }
        }
    }

    else if(yroad_type == YROAD_NEAR) {  // 靠近三叉，进行补线
        if(yroad_direct == RIGHT) {  // 走三叉的右边
            find_line_up(MT9V03X_HALVED_W/2-5, MT9V03X_HALVED_H-10, 0);  // 左
            if(ipts2[0][1] < Preanchorpoint) {  // 向上扫的线的起始点的纵坐标小于预锚点的纵坐标         **这里的设置不是很精确，应该找到扫到的线的最大的纵坐标**
                Pre_point[0] = MT9V03X_HALVED_W/2+20;
                Pre_point[1] = Preanchorpoint;
            }
            else {
                Pre_point[0] = ipts2[ipts2[0][1]-Preanchorpoint][0] + (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
                Pre_point[1] = Preanchorpoint;
            }
        }
        else {  // 左边
            find_line_up(MT9V03X_HALVED_W/2+5, MT9V03X_HALVED_H-10, 1);  // 右
            if(ipts2[0][1] < Preanchorpoint) {  // 向上扫的线的起始点的纵坐标小于预锚点的纵坐标
                Pre_point[0] = MT9V03X_HALVED_W/2+20;
                Pre_point[1] = Preanchorpoint;
            }
            else {
                Pre_point[0] = ipts2[ipts2[0][1]-Preanchorpoint][0] - (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
                Pre_point[1] = Preanchorpoint;
            }
        }

        if(ipts1_num > 10) {
            if(yroad_direct == RIGHT) yroad_type = YROAD_RIGHT_RUN;
            else yroad_type = YROAD_LEFT_RUN;
        }
    }

    /* 在三叉里面，正常寻线 */
    else if(yroad_type == YROAD_LEFT_RUN) {
        u8 xx = 0, yy = 0;
        if(ipts0_num < ipts1_num/2 && ipts0_num < MT9V03X_HALVED_H/2) {  // 左边线短，寻右线
            xx = ipts1[ipts1[0][1]-Preanchorpoint][0] - (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else if(ipts1_num < ipts0_num/2 && ipts1_num < MT9V03X_HALVED_H/2) {  // 右边线短，寻左线
            xx = ipts0[ipts0[0][1]-Preanchorpoint][0] + (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else {  // 双边寻线
            xx = (u8)(ipts0[ipts0[0][1]-Preanchorpoint][0] + ipts1[ipts1[0][1]-Preanchorpoint][0])/2;
            yy = Preanchorpoint;
        }

        /* 写入预锚点 */
        Pre_point[0] = xx;
        Pre_point[1] = yy;

        if(0) {  // 出三叉
            yroad_type = YROAD_LEFT_OUT;
        }
    }

    else if(yroad_type == YROAD_RIGHT_RUN) {
        u8 xx = 0, yy = 0;
        if(ipts0_num < ipts1_num/2 && ipts0_num < MT9V03X_HALVED_H/2) {  // 左边线短，寻右线
            xx = ipts1[ipts1[0][1]-Preanchorpoint][0] - (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else if(ipts1_num < ipts0_num/2 && ipts1_num < MT9V03X_HALVED_H/2) {  // 右边线短，寻左线
            xx = ipts0[ipts0[0][1]-Preanchorpoint][0] + (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else {  // 双边寻线
            xx = (u8)(ipts0[ipts0[0][1]-Preanchorpoint][0] + ipts1[ipts1[0][1]-Preanchorpoint][0])/2;
            yy = Preanchorpoint;
        }

        /* 写入预锚点 */
        Pre_point[0] = xx;
        Pre_point[1] = yy;

        if(0) {  // 出三叉
            yroad_type = YROAD_RIGHT_OUT;
        }
    }


    /* 出三叉 */
    else if(yroad_type == YROAD_LEFT_OUT) {

    }

    else if(yroad_type == YROAD_RIGHT_OUT) {

    }


/************************************************** 处理十字 **********************************************/


    else if(cross_type == CROSS_BEGIN) {  // 路口以外，寻近线
        Preanchorpoint = (u8)((ipts0[rpts0index[0]][1] + ipts1[rpts1index[0]][1])/2);  // 预锚点y为角点y  寻近线

        /* 有两个角点 */
        if(rpts0an_num && rpts1an_num) {
            Pre_point[0] = (u8)((ipts0[rpts0index[0]][0] + ipts1[rpts1index[0]][0])/2);
            Pre_point[1] = Preanchorpoint;
        }
        /* 只有左角点 */
        /* 只有右角点 */


        if(ipts0[rpts0index[0]][1] > 50 || ipts1[rpts1index[0]][1] > 50 || ipts0_num<5 || ipts1_num<5) {  // 左或者右角点靠近，切换寻远线，切换到CROSS_CROSS_1状态
        //if(ipts0_num<5 && ipts1_num<5) {  // 左或者右角点靠近，切换寻远线，切换到CROSS_CROSS_1状态
            cross_type = CROSS_CROSS_1;
        }
    }

    else if(cross_type == CROSS_CROSS_1) {  // 穿过路口
        /* 寻找上角点 */
        find_line_up(5, MT9V03X_HALVED_H-10, 0);  // 左
        cross_rpts[0][0] = ipts2[rpts2index[0]][0]; cross_rpts[0][1] = ipts2[rpts2index[0]][1];
        find_line_up(MT9V03X_HALVED_W-5, MT9V03X_HALVED_H-10, 1);  // 右
        cross_rpts[1][0] = ipts2[rpts2index[0]][0]; cross_rpts[1][1] = ipts2[rpts2index[0]][1];


        Preanchorpoint = (u8)((cross_rpts[0][1] + cross_rpts[1][1])/2);  // 预锚点y为角点y  寻近线

        Pre_point[0] = (u8)((cross_rpts[0][0] + cross_rpts[1][0])/2);
        Pre_point[1] = Preanchorpoint;

        if((u8)((cross_rpts[0][1] + cross_rpts[1][1])/2) <= MT9V03X_HALVED_H-10) {  // 寻巡远线和寻近线的空窗，直接用上一次的值，直到不丢线
            Pre_point[0] = Pre_point[0];
            Pre_point[1] = Pre_point[1];
        }

        if(ipts0_num >= MT9V03X_HALVED_H-15 || ipts1_num >= MT9V03X_HALVED_H-15) {  // 结束丢线， 切换为正常巡线模式
            cross_type = CROSS_IN;
            Preanchorpoint = MT9V03X_HALVED_H/2;
        }
    }

    else if(cross_type == CROSS_CROSS_2) {  // 穿过路口 (状态待改进)
        /* 寻找上角点 */
        find_line_up(5, MT9V03X_HALVED_H-10, 0);  // 左
        cross_rpts[0][0] = ipts2[rpts2index[0]][0]; cross_rpts[0][1] = ipts2[rpts2index[0]][1];
        find_line_up(MT9V03X_HALVED_W-5, MT9V03X_HALVED_H-10, 1);  // 右
        cross_rpts[1][0] = ipts2[rpts2index[0]][0]; cross_rpts[1][1] = ipts2[rpts2index[0]][1];


        Preanchorpoint = (u8)((cross_rpts[0][1] + cross_rpts[1][1])/2);  // 预锚点y为角点y  寻近线

        Pre_point[0] = (u8)((cross_rpts[0][0] + cross_rpts[1][0])/2);
        Pre_point[1] = Preanchorpoint;

        if((u8)((cross_rpts[0][1] + cross_rpts[1][1])/2) <= MT9V03X_HALVED_H-10) {  // 寻巡远线和寻近线的空窗，直接用上一次的值，直到不丢线
            Pre_point[0] = Pre_point[0];
            Pre_point[1] = Pre_point[1];
        }

        if(ipts0_num >= 15 || ipts0_num >= 15) {  // 结束丢线， 结束十字
            cross_type = CROSS_NONE;
            Preanchorpoint = MT9V03X_HALVED_H/2;
        }
    }

    else if(cross_type == CROSS_IN) {  // 十字内(圆环)

        u8 xx = 0, yy = 0;
        if(ipts0_num < ipts1_num/2 && ipts0_num < MT9V03X_HALVED_H/2) {  // 左边线短，寻右线
            xx = ipts1[ipts1[0][1]-Preanchorpoint][0] - (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else if(ipts1_num < ipts0_num/2 && ipts1_num < MT9V03X_HALVED_H/2) {  // 右边线短，寻左线
            xx = ipts0[ipts0[0][1]-Preanchorpoint][0] + (half_width_group[Preanchorpoint]+Midlinecompensationvalue);
            yy = Preanchorpoint;
        }
        else {  // 双边寻线
            xx = (u8)(ipts0[ipts0[0][1]-Preanchorpoint][0] + ipts1[ipts1[0][1]-Preanchorpoint][0])/2;
            yy = Preanchorpoint;
        }

        /* 写入预锚点 */
        Pre_point[0] = xx;
        Pre_point[1] = yy;

        if(ipts0[rpts0index[0]][1] > 50 || ipts1[rpts1index[0]][1] > 50) {  // 角点靠近，切换寻远线
            cross_type = CROSS_CROSS_2;
        }
    }
}





void ImageProcess::corssjudge(void)
{
    if(yroad_type == YROAD_NONE) {  //在对应状态条件下执行
        if(rpts0an_num && rpts1an_num) {  // 有角点
            if(ipts0[rpts0index[0]][1] >= MT9V03X_HALVED_H/2 && ipts1[rpts1index[0]][1] >= MT9V03X_HALVED_H/2) {  // 判断角点位置
                qDebug() << "double point";

                //find_line_up(MT9V03X_HALVED_W/2-15, MT9V03X_HALVED_H-15);

//                if(!Ypt2_found) {  // 找到上Y角点
//                    cross_type = CROSS_BEGIN;

//                    // 最小二乘拟合直线  （角点处）
//                    regression(clip(rpts1index[0]-10, 0, MT9V03X_HALVED_H),   clip(rpts1index[0], clip(rpts1index[0]-10, 0, MT9V03X_HALVED_H)+1, MT9V03X_HALVED_H), ipts1);
//                    // 画出角点
//                    a[ipts1[rpts1index[0]][1]*MT9V03X_HALVED_W + ipts1[rpts1index[0]][0]] = GREEN;
//                    // 画出拟合的直线
//                    for(int i = 1; i < MT9V03X_HALVED_W; i++) {
//                        a[int(clip((B*i+A), 0, MT9V03X_HALVED_H)*MT9V03X_HALVED_W) + i] = GREEN;
//                    }

//                }
            }
        }
    }
}


void ImageProcess::circlejudge(void)
{
    if(yroad_type == YROAD_NONE && cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && garage_type == GARAGE_NONE) {  //在对应状态条件下执行

        if(( rpts0index_num || rpts1index_num ) && (ipts0_num != MT9V03X_HALVED_H*2 && ipts1_num != MT9V03X_HALVED_H*2)) {  // 至少一边有角点  并且防止巡线不正常的情况（一直在一点附近）

            if(rpts0index_num && rpts1index_num && ( fabs(ipts1[rpts1index[0]][1] - ipts1[0][1])>2 )) {  // 两边有角点
                /* 判断角点坐标特性 */
                if(ipts0[rpts0index[0]][1] >= MT9V03X_HALVED_H/2 && ipts1[rpts1index[0]][1] <= 10) {  // 左环岛
                    circle_type = CIRCLE_LEFT_BEGIN_1;
                }
                else if(ipts1[rpts1index[0]][1] >= MT9V03X_HALVED_H/2 && ipts0[rpts0index[0]][1] <= 10) {  // 右环岛
                    circle_type = CIRCLE_RIGHT_BEGIN_1;
                }
                else {  // 不是环岛
                    return;
                }
            }

            else if((rpts0index_num && !rpts1index_num) || (rpts0index_num && fabs(ipts1[rpts1index[0]][1] - ipts1[0][1])<=2)) {  // 左边有角点
                if(ipts0[rpts0index[0]][1] >= MT9V03X_HALVED_H/2) {  // 左环岛
                    circle_type = CIRCLE_LEFT_BEGIN_1;
                }
                else {
                    return;
                }
            }

            else if(!rpts0index_num && rpts1index_num || (rpts1index_num && fabs(ipts0[rpts0index[0]][1] - ipts0[0][1])<=2)) {  // 右边有角点
                if(ipts1[rpts1index[0]][1] >= MT9V03X_HALVED_H/2) {  // 右环岛
                    circle_type = CIRCLE_RIGHT_BEGIN_1;
                }
                else {
                    return;
                }
            }
        }

    }
    /* 二次判定是否丢线   待添加错误退出逻辑 */
    else if(circle_type == CIRCLE_LEFT_BEGIN_1) {
        if(ipts0_num < 5) circle_type = CIRCLE_LEFT_BEGIN_2;
    }
    else if(circle_type == CIRCLE_RIGHT_BEGIN_1) {
        if(ipts1_num < 5) circle_type = CIRCLE_RIGHT_BEGIN_2;
    }
    /* 丢的线出现，切换状态进入圆环 */
    else if(circle_type == CIRCLE_LEFT_BEGIN_2) {  // 左圆环开始
        if(ipts0_num > 5) circle_type = CIRCLE_LEFT_IN;
    }

    else if(circle_type == CIRCLE_RIGHT_BEGIN_2) {  // 右圆环开始
        if(ipts0_num > 5) circle_type = CIRCLE_RIGHT_IN;
    }


    /* 开始补线进入圆环 */
    else if(circle_type == CIRCLE_LEFT_IN) {

    }
    else if(circle_type == CIRCLE_RIGHT_IN) {

    }
}



#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MINMAX(input, low, upper) MIN(MAX(input, low), upper)


u8 garage_num = 0;  // 记录当前第几次车库

float garage_rpts[POINTS_MAX_LEN][2];
u8 garage_rpts_num;
int zebra_cross_flag_begin = 0;
int zebra_cross_flag0[40];
int zebra_cross_flag0_num = 0;
int zebra_cross_flag1[40];
int zebra_cross_flag1_num = 0;

void ImageProcess::garagejudge(void)
{
    if(yroad_type == YROAD_NONE && cross_type == CROSS_NONE && circle_type == CIRCLE_NONE && garage_type == GARAGE_NONE) {  //在对应状态条件下执行

        u8 L_flag = 0, R_flag = 0;  // 左右标志位

        // 仅左角点，在右中线上寻找斑马线(使用右边线单边补线)
        if( ( rpts0index_num && ipts0[rpts0index[0]][1] > MT9V03X_HALVED_H/2 ) &&  ( !rpts1index_num || ( rpts1index_num && ipts1[rpts1index[0]][1] < MT9V03X_HALVED_H/4 ) )  ) {
            garage_rpts_num = ipts1_num;
            add_line_common(0, ipts0, ipts0_num, ipts1, ipts1_num, garage_rpts);
            L_flag = 1;
        }
        // 仅右角点，在左中线上寻找斑马线(使用左边线单边补线)
        else if( ( rpts1index_num && ipts1[rpts1index[0]][1] > MT9V03X_HALVED_H/2 ) &&  ( !rpts0index_num || ( rpts0index_num && ipts0[rpts0index[0]][1]<MT9V03X_HALVED_H/4 ) )   ) {
            garage_rpts_num = ipts0_num;
            add_line_common(1, ipts0, ipts0_num, ipts1, ipts1_num, garage_rpts);
            R_flag = 1;
        }
        // 其余情况不找
        else garage_rpts_num = 0;
        if(garage_rpts_num == 0) return; // 没有能找到的中线就退出


        //u8 line_max = (L_flag)? (u8)ipts0[rpts0index[0]][1] : (u8)ipts1[rpts1index[0]][1];
        // 在中线上一定范围内寻找斑马线
        //for(u8 i = 10; i < line_max; i++) {  // MT9V03X_HALVED_H-10  (u8)ipts0[rpts0index[0]][1]
        for(u8 i = 6; i < garage_rpts_num; i++) {  // 这里遍历的是中线的索引
            zebra_cross_flag_begin = (AT_IMAGE(&img_raw, (u8)garage_rpts[i][0], (u8)garage_rpts[i][1]) < thres)?1:0;


            memset(zebra_cross_flag0, 0, sizeof(zebra_cross_flag0));
            zebra_cross_flag0_num = 0;
            for (u8 x = MAX(0, garage_rpts[i][0] - 1); x >= MAX(1, garage_rpts[i][0] - 25); x--) {
                if (zebra_cross_flag_begin == 0) { // 起始是白色，偶数白，奇数黑
                    if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current even, current white
                        zebra_cross_flag0[zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current even, current black
                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current odd, current white
                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current odd, current black
                        zebra_cross_flag0[zebra_cross_flag0_num]++;
                    }
                } else { // 起始是黑色，偶数黑，奇数白
                    if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current even, current white
                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current even, current black
                        zebra_cross_flag0[zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current odd, current white
                        zebra_cross_flag0[zebra_cross_flag0_num]++;
                    } else if (zebra_cross_flag0_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current odd, current black
                        zebra_cross_flag0[++zebra_cross_flag0_num]++;
                    }
                }
            }

            memset(zebra_cross_flag1, 0, sizeof(zebra_cross_flag1));
            zebra_cross_flag1_num = 0;
            for (int x = (u8)garage_rpts[i][0] + 1; x <= MIN(img_raw.width - 1, (u8)garage_rpts[i][0] + 25); x++) {
                if (zebra_cross_flag_begin == 0) { // even white, odd black
                    if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current even, current white
                        zebra_cross_flag1[zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current even, current black
                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current odd, current white
                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current odd, current black
                        zebra_cross_flag1[zebra_cross_flag1_num]++;
                    }
                }
                else { // even black, odd white
                    if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current even, current white
                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 0 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current even, current black
                        zebra_cross_flag1[zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) > thres) { // current odd, current white
                        zebra_cross_flag1[zebra_cross_flag1_num]++;
                    } else if (zebra_cross_flag1_num % 2 == 1 && AT_IMAGE(&img_raw, x, (u8)garage_rpts[i][1]) < thres) { // current odd, current black
                        zebra_cross_flag1[++zebra_cross_flag1_num]++;
                    }
                }
            }

            // 判断连续变换的阈值条件以识别斑马线   阈值需调节
            int i0 = 1;
            for (; i0 < zebra_cross_flag0_num - 1; i0++) {
                if (zebra_cross_flag0[i0] < 2 || zebra_cross_flag0[i0] >= 20 || fabs(zebra_cross_flag0[i0 + 1] - zebra_cross_flag0[i0]) >= 10) break;
            }
            bool is_zebra0 = (i0 > GARAGE_NUM_TRES)?1:0;

            int i1 = 1;
            for (; i1 < zebra_cross_flag1_num - 1; i1++) {
                if (zebra_cross_flag1[i1] < 2 || zebra_cross_flag1[i1] >= 20 || fabs(zebra_cross_flag1[i1 + 1] - zebra_cross_flag1[i1]) >= 10) break;
            }
            bool is_zebra1 = (i1 > GARAGE_NUM_TRES)?1:0;

            if (is_zebra0 && is_zebra1) {  // 判定为车库
                if(L_flag) garage_type = GARAGE_FOUND_LEFT;
                if(R_flag) garage_type = GARAGE_FOUND_RIGHT;
            }
        }
    }

    else if(garage_type == GARAGE_FOUND_LEFT) {  // 找到左车库
        /* 统计底部的黑色像素数量 */
        u16 num_black = 0;
        for(u8 i = 0; i < MT9V03X_HALVED_W; i++) {
            for(u8 j = 0; j < 3; j++) {
                if(AT_IMAGE(&img_raw, i, j+MT9V03X_HALVED_H-4) < thres) {
                    num_black++;
                }
            }
        }
        if(num_black >= BLACK_THRES*3) {
            garage_type = GARAGE_IN_LEFT;  // 开始进入车库
        }
    }
    else if(garage_type == GARAGE_FOUND_RIGHT) {  // 找到右车库
        /* 统计底部的黑色像素数量 */
        u16 num_black = 0;
        for(u8 i = 0; i < MT9V03X_HALVED_W; i++) {
            for(u8 j = 0; j < 3; j++) {
                if(AT_IMAGE(&img_raw, i, j+MT9V03X_HALVED_H-4) < thres) {
                    num_black++;
                }
            }
        }
        if(num_black >= BLACK_THRES*3) {
            garage_type = GARAGE_IN_RIGHT;  // 开始进入车库
        }
    }
}




/**
 * @brief process_image   图像处理函数
 *
 * @param   None
 */
void ImageProcess::process_image(void)
{
    uint16_t half = 7 / 2;
/******************************************************* 原图找左边线 ***************************************************/
    u8 x1 = img_raw.width/2 - begin_x, y1 = begin_y;
    ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);
    //从下向上寻找起始点
    for(; y1 > MT9V03X_HALVED_H/2; y1--) {
        for(; x1 > 3; x1--) {
            //二值化开始
            int local_thres = 0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    local_thres += AT(&img_raw, x1 + dx, y1 + dy);
                }
            }
            local_thres /= (block_size * block_size);
            local_thres -= clip_value;
            //二值化结束

            if( AT(&img_raw, x1, y1) > local_thres && AT(&img_raw, x1-1, y1) < local_thres && AT(&img_raw, x1-2, y1) < local_thres && AT(&img_raw, x1-3, y1) < local_thres && AT(&img_raw, x1-3, y1) < local_thres \
                //&& fabs(difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1])) >= g_threshold_value_min && fabs(difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1])) <= g_threshold_value_max
                ){
                //PRINTF("%d\n", difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1]));
                //IPS_Drawcross(x1, y1, YELLOW);  //画出起始点
                break;
            }
        }
        if(x1 != 3) break; //找到边线
        else x1 = img_raw.width/2 - begin_x; //没有找到
    }
    if(y1 != MT9V03X_HALVED_H/2) findline_lefthand_adaptive(&img_raw, block_size, clip_value, x1, y1, ipts0, &ipts0_num);
    else {  //没有找到起始点
        memset(ipts0, 0, ipts0_num);
        ipts0_num = 0;
    }
/******************************************************* 原图找右边线 ***************************************************/
    u8 x2 = img_raw.width/2 + begin_x, y2 = begin_y;
    ipts1_num = sizeof(ipts0) / sizeof(ipts0[0]);
    //PRINTF("%d %d %d\n", halvedImage[y2][x2+20], halvedImage[y2][x2],difference_sum(halvedImage[y2][x2+20], halvedImage[y2][x2]));
    //从下向上寻找起始点
    for(; y2 > MT9V03X_HALVED_H/2; y2--) {
        for(; x2 < MT9V03X_HALVED_W-3; x2++) {
            //二值化开始
            int local_thres = 0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    local_thres += AT(&img_raw, x2 + dx, y2 + dy);
                }
            }
            local_thres /= (block_size * block_size);
            local_thres -= clip_value;
            //二值化结束

            if( AT(&img_raw, x2, y2) > local_thres && AT(&img_raw, x2+1, y2) < local_thres && AT(&img_raw, x2+2, y2) < local_thres && AT(&img_raw, x2+3, y2) < local_thres && AT(&img_raw, x2+3, y2) < local_thres \
               //&& fabs(difference_sum(halvedImage[y2][x2+COL_SPACE], halvedImage[y2][x2])) >= g_threshold_value  // mark_middle_num(halvedImage[y2][x2+COL_SPACE], halvedImage[y2][x2], threshold1)
               ){
                //IPS_Drawcross(x2, y2, YELLOW);  //画出起始点
                break;
            }
        }
        if(x2 != MT9V03X_HALVED_W-3) break;  //找到边线
        else x2 = img_raw.width/2 + begin_x;  //没有找到
    }
    if(y2 != MT9V03X_HALVED_H/2) findline_righthand_adaptive(&img_raw, block_size, clip_value, x2, y2, ipts1, &ipts1_num);
    else {  //没有找到起始点
        memset(ipts1, 0, ipts1_num);
        ipts1_num = 0;
    }

/******************************************************* 边线处理 ***************************************************/
#if 1
    // 边线三角滤波  输出 rpts0b rpts1b
    blur_points(ipts0, ipts0_num, rpts0b, (int) round(line_blur_kernel));
    rpts0b_num = ipts0_num;
    blur_points(ipts1, ipts1_num, rpts1b, (int) round(line_blur_kernel));
    rpts1b_num = ipts1_num;

    // 边线局部角度变化率    输出 rpts0a rpts1a
    local_angle_points(rpts0b, rpts0b_num, rpts0a, (int) round(angle_dist / sample_dist));  //round返回四舍五入的整数值
    rpts0a_num = rpts0b_num;
    local_angle_points(rpts1b, rpts1b_num, rpts1a, (int) round(angle_dist / sample_dist));
    rpts1a_num = rpts1b_num;

    // 角度变化率非极大抑制  输出 rpts0an rpts1an    注意可能会出现的nan
    nms_angle(rpts0a, rpts0a_num, rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1, rpts0index, &rpts0index_num);
    rpts0an_num = rpts0a_num;
    nms_angle(rpts1a, rpts1a_num, rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1, rpts1index, &rpts1index_num);
    rpts1an_num = rpts1a_num;

    //中线跟踪
    //track_line_common(ipts0, ipts0_num, ipts1, ipts1_num, rptsc, &rpts_num);
    //add_line_common(1, ipts0, ipts0_num, ipts1, ipts1_num, rptsc);

#endif
}



u16 ImageProcess::FindNumInArray(u16 array[][2], u8 num, u8 numofarray)
{
    for(int i = 0; i < numofarray; i++) {
        if(num == array[i][1]) {
            return i;
        }
    }
    return 255;
}

//基本中线追踪(直道和弯道状态下的扫线)(左右取均值)
void ImageProcess::track_line_common(u16 pts0_in[][2], int num0, u16 pts1_in[][2], int num1, float pts_out[][2], int *pts_num)
{
    if(num0 == 0 || num1 == 0) {
        *pts_num = 0;  //中线数目为0
        return;  //有一边丢线   退出
    }
    else {
        u8 index = 0;
        for(u8 i = MT9V03X_HALVED_H; i > 0; i--) {
            u16 Lx, Rx, Lindex, Rindex;
            Lindex = FindNumInArray(pts0_in, i, num0);
            Rindex = FindNumInArray(pts1_in, i, num1);
            if(Lindex!=255 && Rindex!=255) {
                Lx = pts0_in[(u16)Lindex][0];
                Rx = pts1_in[(u16)Rindex][0];
                pts_out[index][0] = (float)(Lx+Rx)/2;
                pts_out[index][1] = (float)i;
                index++;
            }
        }
        *pts_num = index;
    }
}



/**
 * @brief add_line_common   基本丢线补线函数
 *
 * @param   type        丢线类型  0丢左线  1丢右线
 * @param   pts0_in     左边线
 * @param   num0        左边线数量  0代表没有
 * @param   pts1_in     右边线
 * @param   num1        右边线数量  0代表没有
 * @param   pts_out     输出的中线
 */
void ImageProcess::add_line_common(u8 type,  u16 pts0_in[][2], int num0, u16 pts1_in[][2], int num1, float pts_out[][2])
{
    if(type == 0) {  //丢左线
        rpts_num = 0;
        for(u8 i = 0; i < num1; i++) {
#if 0
            if(pts1_in[i][1] <= 20) continue;
            else if(pts1_in[i][1] >= MT9V03X_HALVED_H-5) continue;


            float dx = pts1_in[clip(i + 10, 0, num1 - 1)][0] - pts1_in[clip(i - 10, 0, num1 - 1)][0];
            float dy = pts1_in[clip(i + 10, 0, num1 - 1)][1] - pts1_in[clip(i - 10, 0, num1 - 1)][1];
            float dn = my_sqrt(dx * dx + dy * dy);
            dx /= dn;
            dy /= dn;

            pts_out[i][0] = pts1_in[i][0] + dy * (half_width_group[pts1_in[i][1]]+Midlinecompensationvalue);
            pts_out[i][1] = pts1_in[i][1] - dx * (half_width_group[pts1_in[i][1]]+Midlinecompensationvalue);


            if((u8)pts_out[i][0] >= MT9V03X_HALVED_W-3) pts_out[i][0] = MT9V03X_HALVED_W-3;
            if((u8)pts_out[i][1] >= MT9V03X_HALVED_H-3) pts_out[i][1] = MT9V03X_HALVED_H-3;
            if((u8)pts_out[i][0] <= 3) pts_out[i][0] = 1;
            if((u8)pts_out[i][1] <= 3) pts_out[i][1] = 1;
#endif
            pts_out[i][0] = pts1_in[i][0] - (half_width_group[pts1_in[i][1]]+Midlinecompensationvalue);
            pts_out[i][1] = pts1_in[i][1];

            rpts_num++;
        }
    }
    else if(type == 1) {  //丢右线
        rpts_num = 0;
        for(u8 i = 0; i < num0; i++) {
#if 0
            if(pts0_in[i][1] <= 20) continue;
            else if(pts0_in[i][1] >= MT9V03X_DVP_H-10) continue;

            float dx = pts0_in[clip(i + 30, 0, num0 - 1)][0] - pts0_in[clip(i - 30, 0, num0 - 1)][0];
            float dy = pts0_in[clip(i + 30, 0, num0 - 1)][1] - pts0_in[clip(i - 30, 0, num0 - 1)][1];
            float dn = my_sqrt(dx * dx + dy * dy);
            dx /= dn;
            dy /= dn;
            pts_out[i][0] = pts0_in[i][0] - dy * (half_width_group[pts0_in[i][1]]+Midlinecompensationvalue);
            pts_out[i][1] = pts0_in[i][1] + dx * (half_width_group[pts0_in[i][1]]+Midlinecompensationvalue);

            if((u8)pts_out[i][0] >= MT9V03X_HALVED_W-3) pts_out[i][0] = MT9V03X_HALVED_W-3;
            if((u8)pts_out[i][1] >= MT9V03X_HALVED_H-3) pts_out[i][1] = MT9V03X_HALVED_H-3;
            if((u8)pts_out[i][0] <= 3) pts_out[i][0] = 1;
            if((u8)pts_out[i][1] <= 3) pts_out[i][1] = 1;
#endif
            pts_out[i][0] = pts0_in[i][0] + (half_width_group[pts0_in[i][1]]+Midlinecompensationvalue);
            pts_out[i][1] = pts0_in[i][1];

            rpts_num++;
        }
    }
}




/************************************线性回归计算中线斜率************************************/
// y = Bx+A
void ImageProcess::regression(int startline,int endline, uint16_t input[][2])
{
    int i = 0, SumX = 0, SumY = 0, SumLines = 0;
    float SumUp = 0, SumDown = 0, avrX = 0, avrY = 0;
    SumLines = endline - startline;   // startline 为开始行， //endline 结束行 //SumLines

    for(i=startline;i<endline;i++) {
        SumX+=input[i][0];
        SumY+=input[i][1];    //这里Middle_black为存放中线的数组
    }
    avrX=SumX/SumLines;     //X的平均值
    avrY=SumY/SumLines;     //Y的平均值
    SumUp=0;
    SumDown=0;

    for(i=startline;i<endline;i++) {
        SumUp+=(input[i][1]-avrY)*(input[i][0]-avrX);
        SumDown+=(input[i][0]-avrX)*(input[i][0]-avrX);
    }
    if(SumDown==0) B=0;
    else B=(SumUp/SumDown);

    A=(SumY-B*SumX)/SumLines;  //截距
}






int ImageProcess::clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

float ImageProcess::fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}


// 点集三角滤波
void ImageProcess::blur_points(uint16_t pts_in[][2], int num, float pts_out[][2], int kernel)
{
    assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - fabs(j));
            pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - fabs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}

// 点集局部角度变化率
void ImageProcess::local_angle_points(float pts_in[][2], int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}

// 点集局部角度变化率(整型输入)
void ImageProcess::local_angle_points_int(u16 pts_in[][2], int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = (double)pts_in[i][0] - (double)pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = (double)pts_in[i][1] - (double)pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = (double)pts_in[clip(i + dist, 0, num - 1)][0] - (double)pts_in[i][0];
        float dy2 = (double)pts_in[clip(i + dist, 0, num - 1)][1] - (double)pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}

// 角度变化率非极大抑制
void ImageProcess::nms_angle(float angle_in[], int num, float angle_out[], int kernel, u8 index_out[], u8 *index_num)
{
    u8 indexnum_cnt = 0;
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
        // 统计角点的位置
        if(fabs(angle_out[i]) > 0.4 && angle_out[i] != NAN) index_out[indexnum_cnt++] = i;
    }
    *index_num = indexnum_cnt;
}

/* 前进方向定义：
 *   0
 * 3   1
 *   2
 */
const int dir_front[4][2] = {{0,  -1},
                            {1,  0},
                            {0,  1},
                            {-1, 0}};
const int dir_frontleft[4][2] = {{-1, -1},
                                {1,  -1},
                                {1,  1},
                                {-1, 1}};
const int dir_frontright[4][2] = {{1,  -1},
                                 {1,  1},
                                 {-1, 1},
                                 {-1, -1}};

/**
 * @brief findline_lefthand_adaptive 左手迷宫巡线
 *
 * @param   *img          传入的图像数据
 * @param   block_size    卷积核大小(奇数)
 * @param   clip_value    二值化校准值
 * @param   x             要二值化的位置x坐标
 * @param   y             要二值化的位置x坐标
 * @param   pts           原图左右边线输出
 * @param   *num          图像高度
 *
 * @return  None
 */
void ImageProcess::findline_lefthand_adaptive(image_t *img, uint8_t block_size, uint8_t clip_value, uint16_t x, uint16_t y, uint16_t pts[][2], uint16_t *num)
{
    uint16_t half = block_size / 2;
    uint16_t step = 0, dir = 0, turn = 0;
    while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4) {
        //二值化开始
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, clip(x + dx, 0, MT9V03X_HALVED_W), clip(y + dy, 0, MT9V03X_HALVED_H) );
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;
        //二值化结束

        //开始寻找边线
        //uint16_t current_value = AT(img, x, y);
        uint16_t front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        uint16_t frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 1) % 4;
            turn++;
        }
        else if (frontleft_value < local_thres) {
            x = clip(x + dir_front[dir][0], 0, MT9V03X_HALVED_W);
            y = clip(y + dir_front[dir][1], 0, MT9V03X_HALVED_H);
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
        else {
            x = clip(x + dir_frontleft[dir][0], 0, MT9V03X_HALVED_W);
            y = clip(y + dir_frontleft[dir][1], 0, MT9V03X_HALVED_H);
            dir = (dir + 3) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}


/**
 * @brief findline_righthand_adaptive 右手迷宫巡线
 *
 * @param   *img          传入的图像数据
 * @param   block_size    卷积核大小(奇数)
 * @param   clip_value    二值化校准值   二值化后的阈值减去此值
 * @param   x             要二值化的位置x坐标
 * @param   y             要二值化的位置x坐标
 * @param   pts           原图左右边线输出
 * @param   *num          图像高度
 *
 * @return  None
 */
void ImageProcess::findline_righthand_adaptive(image_t *img, uint8_t block_size, uint8_t clip_value, uint16_t x, uint16_t y, uint16_t pts[][2], uint16_t *num)
{
    uint16_t half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width - 1 && 0 < y && y < img->height - 1 && turn < 4) {
        //二值化开始
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, clip(x + dx, 0, MT9V03X_HALVED_W), clip(y + dy, 0, MT9V03X_HALVED_H));
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;
        //二值化结束


        //开始寻找边线
        //uint16_t current_value = AT(img, x, y);
        uint16_t front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        uint16_t frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
        else {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}


/* 向上寻找黑白跳变点并巡线 */
void ImageProcess::find_line_up(u8 x00, u8 y00, u8 dir)
{
    uint16_t half = 7 / 2;
    u8 x1 = x00, y1 = y00;

    ipts2_num = sizeof(ipts0_2) / sizeof(ipts0_2[0]);
    //从下向上寻找起始点
    for(; y1 > 0; y1--) {
        //二值化开始
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(&img_raw, clip(x1 + dx, 0, MT9V03X_HALVED_W), clip(y1 + dy, 0, MT9V03X_HALVED_H));
            }
        }
        local_thres /= (block_size * block_size);
        local_thres -= clip_value;
        //二值化结束
        if( AT(&img_raw, x1, y1) > local_thres && AT(&img_raw, clip(x1, 0, MT9V03X_HALVED_W), clip(y1-1, 0, MT9V03X_HALVED_H)) < local_thres && AT(&img_raw, clip(x1, 0, MT9V03X_HALVED_W), clip(y1-2, 0, MT9V03X_HALVED_H)) < local_thres && AT(&img_raw, clip(x1, 0, MT9V03X_HALVED_W), clip(y1-3, 0, MT9V03X_HALVED_H)) < local_thres && AT(&img_raw, clip(x1, 0, MT9V03X_HALVED_W), clip(y1-3, 0, MT9V03X_HALVED_H)) < local_thres \
            //&& fabs(difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1])) >= g_threshold_value_min && fabs(difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1])) <= g_threshold_value_max
            ){
            //PRINTF("%d\n", difference_sum(halvedImage[y1][x1-COL_SPACE], halvedImage[y1][x1]));
            //IPS_Drawcross(x1, y1, YELLOW);  //画出起始点
            break;
        }
    }

    if(y1 != 0)  {
        if(dir == 0) findline_lefthand_adaptive(&img_raw, block_size, clip_value, x1, y1, ipts2, &ipts2_num);  // 左手寻线
        else findline_righthand_adaptive(&img_raw, block_size, clip_value, x1, y1, ipts2, &ipts2_num);  // 右手寻线
    }

    else {  //没有找到起始点
        memset(ipts2, 0, ipts2_num);
        ipts2_num = 0;
    }

    // 边线局部角度变化率    输出rpts2a
    local_angle_points_int(ipts2, ipts2_num, rpts2a, (int) round(angle_dist / sample_dist));  //round返回四舍五入的整数值
    rpts2a_num = ipts2_num;
    // 角度变化率非极大抑制  输出 rpts2an    注意可能会出现的nan
    nms_angle(rpts2a, rpts2a_num, rpts2an, (int) round(angle_dist / sample_dist) * 2 + 1, rpts2index, &rpts2index_num);
    rpts2an_num = rpts2a_num;

//    Ypt2_found = 0;
//    // 0.50-0.86之间是Y角点        0.98-1.50之间是L角点
//    int im1 = clip(rpts2index[0] - (int) round(angle_dist / sample_dist), 0, ipts2_num - 1);
//    int ip1 = clip(rpts2index[0] + (int) round(angle_dist / sample_dist), 0, ipts2_num - 1);
//    float conf = fabs(rpts2a[rpts2index[0]]) - (fabs(rpts2a[im1]) + fabs(rpts2a[ip1])) / 2;
//    //PRINTF("%.2f\n", conf);

//    qDebug() << conf << ipts2[rpts2index[0]][0];

//    // Y角点阈值
//    if (Ypt2_found==0 && 0.10<conf && conf<0.92 && rpts2index[0]<1.6/sample_dist) {  // 0.35-0.92
//        Ypt2_rpts2s_id = rpts2index[0];
//        Ypt2_found = 1;
//    }
}

float ImageProcess::my_sqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );

    y = * ( float * ) &i;
    y = y * ( f - ( x * y * y ) );
    y = y * ( f - ( x * y * y ) );

    return number * y;
}
