#include <iostream>
#include <vector>
#include <algorithm>

#include <QTime>
#include <QThreadPool>
#include <QRunnable>

#include "MedianFilter.h"

using namespace std;
typedef unsigned char byte;
//参数：字节每像素（为了在计算中作为横轴偏移量）
int bytesPerPixel;

//复制源图像
void copySource(QImage *,QImage *, int, int, int);
//自适应中值滤波几个中间量的计算函数
void countAllvalue(byte *, int, int, int&, int&, int&, int&, int, int, int, int, int);
//自适应中值滤波的封装(对一个像素点的一个颜色的操作)
void processUnit(QImage *,QImage *,int,int,int);

//用于执行多线程操作的重载类，每个线程操作一个像素点里的一种颜色
class AMFTask : public QRunnable{
private:
    QImage * r_image;
    QImage * se_image;
    int x1;
    int y1;
    int offset1;
public:
    //利用构造函数传递线程操作参数
    AMFTask(QImage * source_image, QImage * result_image, int x,int y,int offset){
        r_image = result_image;
        se_image = source_image;
        x1 = x;
        y1 = y;
        offset1 = offset;
    }
    void run(){
        processUnit(se_image,r_image,x1,y1,offset1);
    }
};


MedianFilter::MedianFilter(QImage * s_image){
    bytesPerPixel = s_image->bytesPerLine()/s_image->width();//计算字节每像素

    cout<<bytesPerPixel<<endl;

    sourceImage = s_image;//获取源图像
    noiseImage = new QImage(s_image->width(),s_image->height(),s_image->format());//创建和源图像大小一致的噪声图像
    resultImage = new QImage(s_image->width(),s_image->height(),s_image->format());//创建和源图像大小一致的去噪图像
}

MedianFilter::~MedianFilter(){
    delete resultImage;
    delete noiseImage;
}




void MedianFilter::addNoise(){

    //设置时间种子和随机数
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    cout<<"from add noise:"<<noiseImage->bytesPerLine()/noiseImage->width()<<endl;

    //先完整复制图片
    for(int i=0;i<sourceImage->byteCount();i++){

        //源图像和滤波图像起始位置指针
        byte* image_date;
        byte* source_image_date;
        source_image_date=(byte*)(sourceImage->bits());
        image_date=(byte*)(noiseImage->bits());
        *(image_date + i) = *(source_image_date + i);
    }

    //随机添加1000个噪点
    for(int i=0;i<1000;i++){
        int col = qrand()%sourceImage->width();
        int row = qrand()%sourceImage->height();
        byte* image_date;
        image_date=(byte*)(noiseImage->bits());

        int noise;
        if(qrand()%2==1)
            noise=0;
        else
            noise=255;

        //判断是灰度图还是RBG32图
        if(bytesPerPixel == 1){
                *(image_date + row*noiseImage->bytesPerLine() + col* bytesPerPixel+0)=noise;
        }
        else{
            //添加噪声（RGB共同设置为0，即是添加黑点）
            *(image_date + row*noiseImage->bytesPerLine() + col* bytesPerPixel+0) = noise;
            *(image_date + row*noiseImage->bytesPerLine() + col* bytesPerPixel+1) = noise;
            *(image_date + row*noiseImage->bytesPerLine() + col* bytesPerPixel+2) = noise;
        }
    }
    //存储结果图像
    noiseImage->save("addnoise.jpg");
}








void MedianFilter::AMF(){

    //设置线程池，并设置最大线程并发量为5
    QThreadPool * threadPool = QThreadPool::globalInstance();
    threadPool->setMaxThreadCount(8);


    //遍历每一个像素点
    for (int y=0; y<noiseImage->height(); y++){
        for (int x=0; x<noiseImage->width(); x++)
        {
//            已废弃的单线程方式
//            processUnit(sourceImage,resultImage,x,y,0);
//            processUnit(sourceImage,resultImage,x,y,1);
//            processUnit(sourceImage,resultImage,x,y,2);

            //创建线程并通过线程池运行
            AMFTask *amf1 = new AMFTask(noiseImage,resultImage,x,y,0);//B部分
            AMFTask *amf2 = new AMFTask(noiseImage,resultImage,x,y,1);//G部分
            AMFTask *amf3 = new AMFTask(noiseImage,resultImage,x,y,2);//R部分
            threadPool->start(amf1);
            threadPool->start(amf2);
            threadPool->start(amf3);
        }
    }
    //阻塞线程直到完成任务
//    threadPool->waitForDone();
    //存储结果图像
    resultImage->save("result.jpg");
}


void copySource(QImage *source_image, QImage * image, int x,int y,int offset){

    //源图像和滤波图像起始位置指针
    byte* image_date;
    byte* source_image_date;
    source_image_date=(byte*)(source_image->bits());
    image_date=(byte*)(image->bits());
    //行偏移量
    int bytesPerline = image->bytesPerLine();
    int bytesPerline_source = source_image->bytesPerLine();

    *(image_date + bytesPerline*y + x* bytesPerPixel + offset) = *(source_image_date + bytesPerline_source*y + x* bytesPerPixel + offset);
}

void countAllvalue(byte * image_date,int x,int y,int& zmin,int& zmed,int& zmax,int& zxy,int sint,int bytesPerline,int width,int height,int offset){
    //用标准库记录window内的所有像素的灰度值(对彩图即是R/G/B值)
    vector<int> values;

    for(int ym = -(sint-1)/2; ym<=(sint-1)/2; ym++){
        for(int xm = -(sint-1)/2; xm<=(sint-1)/2; xm++){
            //排除边缘点的判断
            if((y+ym)>=0 && (x+xm)>=0 && (y+ym)<height && (x+xm)<width){
                values.push_back(int(*(image_date+(y+ym)*bytesPerline+(x+xm)* bytesPerPixel+offset)));
            }
        }
    }
    //对values内值排序
    sort(values.begin(),values.end());
    //获取算法中间量
    zmax = values[values.size()-1];
    zmin = values[0];
    zmed = values[values.size()/2];
    //获取(x,y)点的值
    zxy = *(image_date+y*bytesPerline+x*bytesPerPixel+offset);
    //清楚values，安全防错误
    values.clear();
}

void processUnit(QImage * source_image, QImage * result_image, int x,int y,int offset){

    //源图像和滤波图像起始位置指针
    byte* result_image_date;
    byte* source_image_date;
    result_image_date=(byte*)(result_image->bits());
    source_image_date=(byte*)(source_image->bits());
    //行偏移量
    int bytesPerline = result_image->bytesPerLine();
    int bytesPerline_source = source_image->bytesPerLine();

    /*一些算法参数
     */
    //window最大半径
    int smax = 13;
    //window大小
    int sint = 3;

    /*一些算法中间量
     */
    int zmin,zmax,zmed,zxy,A1,A2,B1,B2;

    //循环直到计算出结果
    while(true){
        //计算中间量
        countAllvalue(source_image_date,x,y,zmin,zmed,zmax,zxy,sint,bytesPerline_source,source_image->width(),source_image->height(),offset);
        A1 = zmed-zmin;
        A2 = zmed-zmax;
        //核心算法部分
        if(A1>0 && A2<0){
            //进入B层
            B1 = zxy-zmin;
            B2 = zxy-zmax;
            if(B1>0 && B2<0){
                *(result_image_date + bytesPerline*y + x* bytesPerPixel + offset) = zxy;
                break;
            }
            else{
                *(result_image_date + bytesPerline*y + x* bytesPerPixel + offset) = zmed;
                break;
            }
        }
        else{
            //仍在A层
            sint += 2;
            if(sint>smax){
                *(result_image_date + bytesPerline*y + x* bytesPerPixel + offset) = zmed;
                break;
            }
        }
    }

}
