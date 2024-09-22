/*** 
 * @Date: 2024-09-04 20:05:41
 * @LastEditors: jsvi53
 * @LastEditTime: 2024-09-22 02:38:56
 * @FilePath: \18_hostviewer\mainwindow.h
 */
/***
 * @Date: 2024-09-04 20:05:41
 * @LastEditors: jsvi53
 * @LastEditTime: 2024-09-05 02:47:21
 * @FilePath: \18_hostviewer\mainwindow.h
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QSerialPortInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QThread>
#include <QOpenGLTexture>
#include <QGraphicsPixmapItem>
#include <QCoreApplication>
#include "serialport.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef enum {
    frame_mode,
    line_mode,
    pixel_mode,
} SendMode;

class MyOpenGLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QList<QSerialPortInfo>                              serialPortList;
    SerialPort*                                         COM;


private:
    Ui::MainWindow *                                    ui;
    QTimer *                                            timer;
    QTimer *                                            bartimer;
    uint32_t                                            RGB565_to_RGB24(uint16_t rgb565);
    uint32_t                                            RGB24_to_RGB565(uint32_t rgb24);
    void                                                graphicsView(QImage &image);
    bool                                                sendImage(QImage image, SendMode mode);
    bool                                                sendImage(QByteArray imagedata);
    QImage                                              image;
    int                                                 __currentLine{0};


signals:
    void                                                updateProgress(int value);
    void                                                showlineSignal(QByteArray data);

private slots :
    void                                                updateDateTime();
    void                                                on_pushButton_5_clicked();
    void                                                on_comboBox_5_activated(int index);  // comboBox_5
    void                                                on_pushButton_4_clicked();      // send btn
    void                                                on_pushButton_3_clicked();      // clear btn
    void                                                on_pushButton_2_clicked();
    void                                                onProgressUpdated(int value);
    void                                                showProgressBar();
    void                                                showReceiveData();
    void                                                showline(const QByteArray &data);

};


#endif  // MAINWINDOW_H
