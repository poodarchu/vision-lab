#include "cvlabapp.h"
#include "ui_cvlabapp.h"
#include "qmath.h"
#include <iostream>
#include "medianfilter.h"
#include <QImage>

CVLabApp::CVLabApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CVLabApp)
{
    ui->setupUi(this);
    imgLabel = ui->srcImgLabel;

    width = imgLabel->width();
    height = imgLabel->height();

    openFileBtn = ui->openFileBtn;
    connect(openFileBtn, SIGNAL(clicked(bool)), SLOT(open()));

    addNoiseBtn = ui->addNoiseBtn;
    connect(addNoiseBtn, SIGNAL(clicked(bool)), this, SLOT(addNoise()) );

    AMFBtn = ui->AMFBtn;
    connect(AMFBtn, SIGNAL(clicked(bool)), this, SLOT(cutNoise()) );
}

CVLabApp::~CVLabApp()
{
    delete ui;
}

void CVLabApp::open() {
    //show a file open dialog at QStandardPaths::PicturesLocation.
    QString file = QFileDialog::getOpenFileName(this, tr("Open an image"),
                                                      QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                                      "*.jpg *.png *.jpeg *.xmp *.gif");

    if(!file.isNull()) {
        QImage* cImage = new QImage(file);
        mf = new MedianFilter(cImage);
        displaySource(cImage);
    }
//    QPixmap pix(file);
//    int width = imgLabel->width();
//    int height = imgLabel->height();
////    imgLabel->setStyleSheet(QString("border-image:url(") + file + QString(");") );
////    imgLabel->setPixmap(pix.scaled(width, height, Qt::AspectRatioMode));
//    imgLabel->setPixmap(pix.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void CVLabApp::addNoise() {
    mf->addNoise();
    QImage cImage("addnoise.jpg");
    displayNoise(&cImage);
}

void CVLabApp::cutNoise() {
    mf->AMF();
    QImage cImage("result.jpg");
    displayResult(&cImage);
}

void CVLabApp::displaySource(QImage *srcImg) {
    ui->srcImgLabel->setPixmap(QPixmap::fromImage(*srcImg).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void CVLabApp::displayNoise(QImage *noiseImg) {
    ui->srcImgLabel->setPixmap(QPixmap::fromImage(*noiseImg).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void CVLabApp::displayResult(QImage *dstImg) {
    ui->srcImgLabel->setPixmap(QPixmap::fromImage(*dstImg).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}




