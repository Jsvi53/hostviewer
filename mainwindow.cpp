/***
 * @Date: 2024-09-04 20:05:41
 * @LastEditors: jsvi53
 * @LastEditTime: 2024-09-05 03:31:40
 * @FilePath: \18_hostviewer\mainwindow.cpp
 */
#include <QDateTime>
#include <QInputDialog>
#include <QPainter>
#include <QTimer>
#include <Qdebug>
#include <bitset>
#include <iostream>
#include <string>

#include "./ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // set graphicsView background color to black
    ui->graphicsView->setStyleSheet("background-color: black;");

    // set combo box item 'custom' background color
    ui->comboBox_5->setItemData(ui->comboBox_5->findText("custom"), QColor(Qt::yellow), Qt::BackgroundRole);

    // set date time
    ui->statusbar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // create port
    COM = new SerialPort(this);
    // serial port list
    serialPortList = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &info, serialPortList)
    {
        ui->comboBox->addItem(info.portName());
    }
    // set default parameters
    ui->comboBox_5->setCurrentText("115200");
    ui->comboBox_3->setCurrentText("8");

    // update date
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timer->start(1000);

    // progress bar
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->hide();
    bartimer = new QTimer(this);
    connect(this, &MainWindow::updateProgress, this, &MainWindow::onProgressUpdated);

    image = QImage(240, 320, QImage::Format_RGB888);
    connect(this, &MainWindow::showlineSignal, this, &MainWindow::showline);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete COM;
    delete timer;
    delete bartimer;
}

void MainWindow::on_pushButton_5_clicked()
{
    if(ui->comboBox->currentText() == "")
    {
        QMessageBox::warning(nullptr, "Warning", "Please select the serial port first!");
        return;
    }

    if(COM->serialPort->isOpen())
    {
        bool flag = COM->closeSerialPort();
        if(flag)
        {
            ui->textEdit->append("Serial port is already closed!");
            ui->radioButton->setChecked(false);
            ui->pushButton_5->setText("打开串口");
            return;
        }
    }

    COM->portName = ui->comboBox->currentText();
    COM->baudRate = ui->comboBox_5->currentText().toInt();

    if(ui->comboBox_4->currentText() == "None")
    {
        COM->partyBit = QSerialPort::NoParity;
    } else if(ui->comboBox_4->currentText() == "Odd")
    {
        COM->partyBit = QSerialPort::OddParity;
    } else if(ui->comboBox_4->currentText() == "Even")
    {
        COM->partyBit = QSerialPort::EvenParity;
    }
    if(ui->comboBox_2->currentText() == "1")
    {
        COM->stopBit = QSerialPort::OneStop;
    } else if(ui->comboBox_2->currentText() == "1.5")
    {
        COM->stopBit = QSerialPort::OneAndHalfStop;
    } else if(ui->comboBox_2->currentText() == "2")
    {
        COM->stopBit = QSerialPort::TwoStop;
    }

    if(ui->comboBox_3->currentText() == "5")
    {
        COM->dataBit = QSerialPort::Data5;
    } else if(ui->comboBox_3->currentText() == "6")
    {
        COM->dataBit = QSerialPort::Data6;
    } else if(ui->comboBox_3->currentText() == "7")
    {
        COM->dataBit = QSerialPort::Data7;
    } else if(ui->comboBox_3->currentText() == "8")
    {
        COM->dataBit = QSerialPort::Data8;
    }

    COM->openSerialPort();
    // open serial port
    if(COM->serialPort->isOpen())
    {
        ui->textEdit->append("Serial port is already opened!");
        ui->radioButton->setChecked(true);
        ui->pushButton_5->setText("关闭串口");
        connect(COM->serialPort, &QSerialPort::readyRead, this, &MainWindow::showReceiveData);
    } else
    {
        ui->textEdit->append("Serial port is not opened!");
    }
}

void MainWindow::on_comboBox_5_activated(int index)
{
    if(index == 0)
    {
        QString text = QInputDialog::getText(this, "Custom baud rate", "Enter baud rate:");
        if((text.isEmpty()) || (text.toInt() == 0))
        {
            ui->comboBox_5->setCurrentText("115200");
        } else
        {
            int index = ui->comboBox_5->findText(text);
            if(index == -1)
            {
                // 将custom选项，改为 text
                ui->comboBox_5->setItemText(0, text);
            }
            ui->comboBox_5->setCurrentText(text);
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    // Check if the serial port is open
    if(!(COM->serialPort->isOpen()))
    {
        QMessageBox::warning(nullptr, "Warning", "Please open the serial port first!");
        return;
    }

    // Read a line of text from ui->lineEdit
    QString text = ui->lineEdit->text();

    // Check if the file exists
    QFileInfo checkFile(text);
    if(checkFile.exists() && checkFile.isFile())
    {
        // Check if the file is a .bin file
        if(checkFile.suffix() == "bin")
        {
            // Read the binary file
            QFile file(text);
            if(file.open(QIODevice::ReadOnly))
            {
                QByteArray data = file.readAll();
                // Create an image from the binary data assuming RGB565 format
                QImage image((uchar *)data.data(), 240, 320, QImage::Format_RGB16);

                sendImage(data);
                graphicsView(image);
            }
        }
        // Check if the file is a standard image format (.jpg or .png)
        else if(checkFile.suffix() == "jpg" || checkFile.suffix() == "png")
        {
            // Load and handle the image
            QImage image(text);
            sendImage(image, line_mode);
            graphicsView(image);
        }
    } else
    {
        // Send the text data through the serial port
        COM->writeData(text.toUtf8());
        ui->textEdit->append(text.toUtf8());
        qDebug() << text.toUtf8();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    if(ui->graphicsView->scene() != nullptr)
    {
        ui->graphicsView->scene()->clear();
    } else
    {
        QGraphicsScene *scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->scene()->clear();
    }
    ui->graphicsView->update();
}

void MainWindow::updateDateTime()
{
    ui->statusbar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void MainWindow::graphicsView(QImage &image)
{
    QPixmap              pixmapImage = QPixmap::fromImage(image);
    QGraphicsPixmapItem *item        = new QGraphicsPixmapItem(pixmapImage);
    if(!ui->graphicsView->scene())
    {
        ui->graphicsView->setScene(new QGraphicsScene(this));
    }
    ui->graphicsView->scene()->clear();
    ui->graphicsView->scene()->addItem(item);
    ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);  // Scale the image to fit the size of the graphicsView
    ui->graphicsView->update();
    ui->graphicsView->show();
}

uint32_t MainWindow::RGB565_to_RGB24(uint16_t rgb565)
{
    uint8_t r = (rgb565 >> 11) & 0x1F;
    uint8_t g = (rgb565 >> 5) & 0x3F;
    uint8_t b = rgb565 & 0x1F;

    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);

    return qRgb(r, g, b);
}

uint32_t MainWindow::RGB24_to_RGB565(uint32_t rgb24)
{
    uint32_t r = (rgb24 & 0xFF0000) >> 16;
    uint32_t g = (rgb24 & 0x00FF00) >> 8;
    uint32_t b = (rgb24 & 0x0000FF);
    return ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | (b >> 3);
}

bool MainWindow::sendImage(QImage image, SendMode mode)
{
    // Read image data
    if(image.isNull())
    {
        QMessageBox::warning(nullptr, "Warning", "Read image data failed!");
        return false;
    }

    // progress bar
    int maxProgress = image.height() - 1;
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(maxProgress);

    switch(mode)
    {
        case pixel_mode: {
            for(int i = 0; i < image.height(); i++)
            {
                for(int j = 0; j < image.width(); j++)
                {
                    QByteArray imageData;
                    QRgb       rgb    = image.pixel(j, i);
                    uint16_t   rgb565 = ((qRed(rgb) & 0xF8) << 8) | ((qGreen(rgb) & 0xFC) << 3) | (qBlue(rgb) >> 3);

                    // Debugging information
                    qDebug() << "qRed(rgb):" << qRed(rgb) << "qGreen(rgb):" << qGreen(rgb) << "qBlue(rgb):" << qBlue(rgb);
                    qDebug() << "RGB565:" << rgb565;

                    // Ensure correct byte order
                    uint8_t highByte = (rgb565 >> 8) & 0xFF;
                    uint8_t lowByte  = rgb565 & 0xFF;
                    imageData.append(lowByte);
                    imageData.append(highByte);
                    qDebug() << QString("hex: %1 %2").arg(highByte, 2, 16, QLatin1Char('0')).arg(lowByte, 2, 16, QLatin1Char('0'));
                    COM->writeData(imageData);
                }

                emit updateProgress(i);
                // every 10 lines, process events to update ui by QCoreApplication
                if(i % 10 == 0)
                {
                    QCoreApplication::processEvents();
                }
            }
            break;
        }

        case line_mode: {
            for(int i = 0; i < image.height(); i++)
            {
                QByteArray imageData;
                for(int j = 0; j < image.width(); j++)
                {
                    QRgb     rgb    = image.pixel(j, i);
                    uint16_t rgb565 = ((qRed(rgb) & 0xF8) << 8) | ((qGreen(rgb) & 0xFC) << 3) | (qBlue(rgb) >> 3);

                    // Debugging information
                    qDebug() << "qRed(rgb):" << qRed(rgb) << "qGreen(rgb):" << qGreen(rgb) << "qBlue(rgb):" << qBlue(rgb);
                    qDebug() << "RGB565:" << rgb565;

                    // Ensure correct byte order
                    uint8_t highByte = (rgb565 >> 8) & 0xFF;
                    uint8_t lowByte  = rgb565 & 0xFF;
                    imageData.append(lowByte);
                    imageData.append(highByte);
                    qDebug() << QString("hex: %1 %2").arg(highByte, 2, 16, QLatin1Char('0')).arg(lowByte, 2, 16, QLatin1Char('0'));
                }
                COM->writeData(imageData);
                emit updateProgress(i);
                // every 10 lines, process events to update ui by QCoreApplication
                if(i % 10 == 0)
                {
                    QCoreApplication::processEvents();
                }
            }
            break;
        }

        case frame_mode: {
            QByteArray imageData;
            for(int i = 0; i < image.height(); i++)
            {
                for(int j = 0; j < image.width(); j++)
                {
                    QRgb     rgb    = image.pixel(j, i);
                    uint16_t rgb565 = ((qRed(rgb) & 0xF8) << 8) | ((qGreen(rgb) & 0xFC) << 3) | (qBlue(rgb) >> 3);

                    // Debugging information
                    qDebug() << "qRed(rgb):" << qRed(rgb) << "qGreen(rgb):" << qGreen(rgb) << "qBlue(rgb):" << qBlue(rgb);
                    qDebug() << "RGB565:" << rgb565;

                    // Ensure correct byte order
                    uint8_t highByte = (rgb565 >> 8) & 0xFF;
                    uint8_t lowByte  = rgb565 & 0xFF;
                    imageData.append(lowByte);
                    imageData.append(highByte);
                    qDebug() << QString("hex: %1 %2").arg(highByte, 2, 16, QLatin1Char('0')).arg(lowByte, 2, 16, QLatin1Char('0'));
                }
            }
            COM->writeData(imageData);
            showProgressBar();
            break;
        }

        default: {
            break;
        }
    }
    return true;
}

bool MainWindow::sendImage(QByteArray imagedata)
{
    // Send image data
    if(imagedata.isEmpty())
    {
        QMessageBox::warning(nullptr, "Warning", "Read image data failed!");
        return false;
    }

    COM->writeData(imagedata);
    return true;
}

void MainWindow::showProgressBar()
{
    ui->progressBar->setValue(0);  // init bar value 0
    ui->progressBar->show();       // show bar

    // use timer to update 100ms at a time
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        int value = ui->progressBar->value();
        if(value < 100)
        {
            ui->progressBar->setValue(value + 1);  // add 1
        } else
        {
            timer->stop();
            QThread::sleep(1);  // wait 1s
            ui->progressBar->hide();
        }
    });

    timer->start(1);  // 100ms update at a time
}

void MainWindow::onProgressUpdated(int value)
{
    ui->progressBar->setValue(value);
    ui->progressBar->show();

    if(value >= ui->progressBar->maximum())
    {
        ui->progressBar->setValue(ui->progressBar->maximum());
        QThread::sleep(1);
        ui->progressBar->hide();
        QCoreApplication::processEvents();
    }
}

void MainWindow::showReceiveData()
{
    QByteArray data = COM->readData();
    qDebug() << data.size();
    if(data.size() == 480)
    {
        if(data.isEmpty())
        {
            return;
        }
        emit showlineSignal(data);
    } else
    {
        if(data.isEmpty())
        {
            return;
        }
        ui->textEdit->append(data);
    }
}

void MainWindow::showline(const QByteArray &data)
{
    if(__currentLine == image.height() + 1)
    {
        __currentLine = 0;
    }
    for(int i = 0; i < image.width(); ++i)
    {
        uint16_t rgb565 = (static_cast<uint8_t>(data[2 * i]) << 8) | static_cast<uint8_t>(data[2 * i + 1]);
        uint32_t rgb24  = RGB565_to_RGB24(rgb565);
        image.setPixel(i, __currentLine, rgb24);
    }

    __currentLine++;

    if(__currentLine == image.height())
    {
        graphicsView(image);
    }
}
