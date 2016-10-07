#ifndef CVLABAPP_H
#define CVLABAPP_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtConcurrent>
#include "medianfilter.h"

namespace Ui {
class CVLabApp;
}

class CVLabApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit CVLabApp(QWidget *parent = 0);
    ~CVLabApp();

    void displaySource(QImage* );
    void displayNoise(QImage* );
    void displayResult(QImage* );

    MedianFilter* mf;

private:
    Ui::CVLabApp *ui;
    QLabel* imgLabel;

    QPushButton* openFileBtn;
    QPushButton* addNoiseBtn;
    QPushButton* AMFBtn;

    int width;
    int height;

private slots:
    void open();
    void addNoise();
    void cutNoise();
};

#endif // CVLABAPP_H
