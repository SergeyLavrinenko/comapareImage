#include <QCoreApplication>

#include <QFile>
#include <QFileDialog>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>

using namespace std;

typedef struct
{
    unsigned int    bfType;
    unsigned long   bfSize;
    unsigned int    bfReserved1;
    unsigned int    bfReserved2;
    unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
    int   rgbBlue;
    int   rgbGreen;
    int   rgbRed;
    int   rgbReserved;
} RGBQUAD;

typedef struct
{
    int   h;
    float   s;
    float   v;
} HSVQUAD;

static unsigned short read_u16(QFile* fp);
static unsigned int   read_u32(QFile* fp);
static int            read_s32(QFile* fp);
static void           RGBtoHSV(RGBQUAD* RGB, HSVQUAD* HSV);

BITMAPFILEHEADER header;
BITMAPINFOHEADER bmiHeader;


const int size_klast = 60;
int max_image_width = 400;

int data_hsv1[size_klast + 1];
int data_hsv2[size_klast + 1];


void ReadData(QFile *pFile, int * data_hsv){

    RGBQUAD** rgb;
    uint32_t** colors;
    HSVQUAD** hsv;
    // считываем заголовок файла


    header.bfType = read_u16(pFile);
    header.bfSize = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits = read_u32(pFile);

    //printf("%x %d %d\n", header.bfType, header.bfSize, header.bfOffBits);

    // считываем заголовок изображения

    bmiHeader.biSize = read_u32(pFile);
    bmiHeader.biWidth = read_s32(pFile);
    bmiHeader.biHeight = read_s32(pFile);
    bmiHeader.biPlanes = read_u16(pFile);
    bmiHeader.biBitCount = read_u16(pFile);
    bmiHeader.biCompression = read_u32(pFile);
    bmiHeader.biSizeImage = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed = read_u32(pFile);
    bmiHeader.biClrImportant = read_u32(pFile);
    printf("width\theight\n%d\t%d\n", bmiHeader.biWidth, bmiHeader.biHeight);
    //printf("bits per pixel\n%d\n", bmiHeader.biBitCount);
    //printf("biCompression\n%d\n", bmiHeader.biCompression);
    //cout << bmiHeader.biSize << ' ' << bmiHeader.biWidth << ' ' << bmiHeader.biHeight << endl;

    rgb = new RGBQUAD * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        rgb[i] = new RGBQUAD[bmiHeader.biHeight];
    }

    colors = new uint32_t * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        colors[i] = new uint32_t[bmiHeader.biHeight];
    }

    hsv = new HSVQUAD * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        hsv[i] = new HSVQUAD[bmiHeader.biHeight];
    }
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            rgb[i][j].rgbBlue = (unsigned char)pFile->read(1)[0];
            rgb[i][j].rgbGreen = (unsigned char)pFile->read(1)[0];
            rgb[i][j].rgbRed = (unsigned char)pFile->read(1)[0];
            colors[i][j] = rgb[i][j].rgbBlue + (rgb[i][j].rgbGreen << 8) + (rgb[i][j].rgbRed << 16);

            RGBtoHSV(&(rgb[i][j]), &(hsv[i][j]));

            //printf("%d %2.1f %2.1f\n", hsv[i][j].h, hsv[i][j].s*100, hsv[i][j].v*100);
            data_hsv[hsv[i][j].h / (360/size_klast)] ++;
        }

        // пропускаем последний байт в строке
        pFile->read(1)[0];
    }
    data_hsv[size_klast] = 0;
    for(int i = 0; i<size_klast; i++){
        qDebug()<<data_hsv[i]<<" "<<i<<"\n";
        if (data_hsv[i] > data_hsv[size_klast]) data_hsv[size_klast] = data_hsv[i];
    }


}
QLabel *imageLab1;
QLabel *imageLab2;
QLabel *resultLab;

QPixmap *image1;
QPixmap *image2;

QChart *chart;
QChartView *chartView;
QLineSeries *series0;
QLineSeries *series1;
void setLabVector(){
    unsigned long long res = 0;
    for(int i=0; i<size_klast; i++){
        res += pow(data_hsv1[i] - data_hsv2[i], 2);
    }
    res = pow(res, 0.5);
    QString result("Расстояние между векторами(картинками): ");
    resultLab->setText(result.append(QString::number(res)));
}
void openDialog0(){
    QWidget* a = new QWidget();
    QString fileName = QFileDialog::getOpenFileName(a, "Open Document",
                                                    QDir::currentPath(),
                                                    "BMP files (*.bmp)");
    for(int i = 0; i<size_klast; i++){
        data_hsv1[i] = 0;
    }
    qDebug() << fileName;
    QFile pFile1(fileName);
    pFile1.open( QIODevice::ReadOnly );
    if(pFile1.isOpen()){
        ReadData(&pFile1, &(data_hsv1[0]));
        pFile1.close();


        series0->clear();
        for(int i = 0; i<size_klast; i++){
            *series0 << QPointF(i, data_hsv1[i]);
        }
        chart->axes(Qt::Vertical).first()->setRange(0, max(data_hsv1[size_klast], data_hsv2[size_klast]));
        chartView->repaint();


        image1->load(fileName);
        if(image1->width() > max_image_width){
            imageLab1->setPixmap(image1->scaledToWidth(max_image_width));
        }
        else{
            imageLab1->setPixmap(*image1);
        }
        imageLab1->repaint();
        setLabVector();
    }
}
void openDialog1(){
    QWidget* a = new QWidget();
    QString fileName = QFileDialog::getOpenFileName(a, "Open Document",
                                                    QDir::currentPath(),
                                                    "BMP files (*.bmp)");
    for(int i = 0; i<size_klast; i++){
        data_hsv2[i] = 0;
    }
    qDebug() << fileName;
    QFile pFile2(fileName);
    pFile2.open( QIODevice::ReadOnly );
    if(pFile2.isOpen()){
        ReadData(&pFile2, &(data_hsv2[0]));
        pFile2.close();


        series1->clear();
        for(int i = 0; i<size_klast; i++){
            qDebug() << i << data_hsv2[i];
            *series1 << QPointF(i, data_hsv2[i]);
        }
        chart->axes(Qt::Vertical).first()->setRange(0, max(data_hsv1[size_klast], data_hsv2[size_klast]));
        chartView->repaint();

        image2->load(fileName);
        if(image2->width() > max_image_width){
            imageLab2->setPixmap(image2->scaledToWidth(max_image_width));
        }
        else{
            imageLab2->setPixmap(*image2);
        }
        imageLab2->repaint();
        setLabVector();
    }

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    series0 = new QLineSeries();
    series1 = new QLineSeries();
    *series0 << QPointF(0, 0);
    *series1 << QPointF(0, 0);

    chart = new QChart();

    QAreaSeries *series_0 = new QAreaSeries(series0);
    QAreaSeries *series_1 = new QAreaSeries(series1);
    series_0->setName("1");
    series_1->setName("2");

    QPen pen(QColor(255, 0, 0));
    pen.setWidth(1);
    series_0->setPen(pen);
    pen.setColor(QColor(0, 255, 0));
    series_1->setPen(pen);

    series_0->setBrush(QColor(255, 0, 0, 127));
    series_1->setBrush(QColor(0, 255, 0, 127));


    chart->addSeries(series_0);
    chart->addSeries(series_1);
    chart->setTitle("Сравнение картинок");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, size_klast);
    chart->axes(Qt::Vertical).first()->setRange(0,100);
    chartView = new QChartView(chart);


    QMainWindow window;
    QWidget *main_window = new QWidget;

    QPushButton *button1 = new QPushButton("1");
    QPushButton *button2 = new QPushButton("2");

    imageLab1 = new QLabel;
    imageLab2 = new QLabel;
    imageLab1->setAlignment(Qt::AlignCenter);
    imageLab2->setAlignment(Qt::AlignCenter);
    imageLab1->setMaximumWidth(max_image_width);
    imageLab2->setMaximumWidth(max_image_width);

    resultLab = new QLabel;

    image1 = new QPixmap();
    image2 = new QPixmap();

    QVBoxLayout *imageBox1 = new QVBoxLayout;
    imageBox1->addWidget(imageLab1);
    imageBox1->addWidget(button1);

    QVBoxLayout *imageBox2 = new QVBoxLayout;
    imageBox2->addWidget(imageLab2);
    imageBox2->addWidget(button2);

    QHBoxLayout *imagesBox = new QHBoxLayout;
    imagesBox->addLayout(imageBox1);
    imagesBox->addLayout(imageBox2);

    QVBoxLayout *menuBox = new QVBoxLayout;
    menuBox->addLayout(imagesBox);
    menuBox->addWidget(resultLab);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(chartView);
    layout->addLayout(menuBox);

    main_window->setLayout(layout);

    window.setCentralWidget(main_window);
    window.setWindowTitle("Сравнение картинок");
    window.resize(1000, 800);
    window.show();
    QObject::connect(button1, &QPushButton::clicked, openDialog0);
    QObject::connect(button2, &QPushButton::clicked, openDialog1);

    return a.exec();

}
#define _CRT_SECURE_NO_WARNINGS



static unsigned short read_u16(QFile* fp)
{
    unsigned char b0, b1;

    b0 = fp->read(1)[0];
    b1 = fp->read(1)[0];

    return ((b1 << 8) | b0);
}


static unsigned int read_u32(QFile* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = fp->read(1)[0];
    b1 = fp->read(1)[0];
    b2 = fp->read(1)[0];
    b3 = fp->read(1)[0];

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


static int read_s32(QFile* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = fp->read(1)[0];
    b1 = fp->read(1)[0];
    b2 = fp->read(1)[0];
    b3 = fp->read(1)[0];

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

static void RGBtoHSV(RGBQUAD* RGB, HSVQUAD* HSV){
    float R = RGB->rgbRed / 255.0;
    float G = RGB->rgbGreen / 255.0;
    float B = RGB->rgbBlue / 255.0;

    float max_rgb = max(max(R, G), B);
    float min_rgb = min(min(R, G), B);

    int h;
    float s, v;

    // h
    if(max_rgb == min_rgb){
        h = 0;
    }
    else if(max_rgb == R){
        if(G >= B){
            h = 60 * ((G-B) / (max_rgb - min_rgb));
        }
        else{
            h = 60 * ((G-B) / (max_rgb - min_rgb)) + 360;
        }
    }
    else if(max_rgb == G){
        h = 60 * ((B-R) / (max_rgb - min_rgb)) + 120;
    }
    else if(max_rgb == B){
        h = 60 * ((R-G) / (max_rgb - min_rgb)) + 240;
    }
    // s
    if(max_rgb == 0){
    }
    else{
        s = ((max_rgb-min_rgb)/max_rgb);
    }
    // v
    v = max_rgb;


    HSV->h = h;
    HSV->s = s;
    HSV->v = v;

}
