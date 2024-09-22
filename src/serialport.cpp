/*** 
 * @Date: 2024-09-05 00:20:05
 * @LastEditors: jsvi53
 * @LastEditTime: 2024-09-17 14:16:10
 * @FilePath: \18_hostviewer\src\serialport.cpp
 */
#include <QMessageBox>
#include <QDebug>

#include "serialport.h"

SerialPort::SerialPort(QObject *parent) : QObject(parent)  // Initialize the QObject base class
{
    // Constructor implementation
    serialPort = new QSerialPort(this);
}

SerialPort::~SerialPort()
{
    delete serialPort;
}

bool SerialPort::openSerialPort()
{
    qDebug() << portName;
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(dataBit);
    serialPort->setParity(partyBit);
    serialPort->setStopBits(stopBit);
    if(!serialPort->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(nullptr, "Warning", "Open serial port failed!");
        return false;
    }

    return true;
}

bool SerialPort::closeSerialPort()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
        if(serialPort->isOpen())
        {
            QMessageBox::warning(nullptr, "Warning", "Close serial port failed!");
            return false;
        }
    }
    return true;
}

bool SerialPort::writeData(const QByteArray &data)
{
    if(!serialPort->isOpen())
    {
        QMessageBox::warning(nullptr, "Warning", "Please open the serial port first!");
        return false;
    }
    serialPort->write(data);
    return true;
}

QByteArray SerialPort::readData()
{
    if(!serialPort->isOpen())
    {
        QMessageBox::warning(nullptr, "Warning", "Please open the serial port first!");
        return QByteArray();
    }
    return serialPort->readAll();
}





