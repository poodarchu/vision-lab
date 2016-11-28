#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include <QImage>

class MedianFilter
{
public:
    MedianFilter(QImage* srcImg);
    ~MedianFilter();

    void addNoise();
    void AMF();

private:
    QImage* sourceImage;
    QImage* noiseImage;
    QImage* resultImage;
};

#endif // MEDIANFILTER_H
