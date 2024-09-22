/*** 
 * @Date: 2024-09-05 00:19:49
 * @LastEditors: jsvi53
 * @LastEditTime: 2024-09-17 14:34:11
 * @FilePath: \18_hostviewer\include\serialport.h
 */
#pragma once
#include <QSerialPort>
#include <QObject>

class SerialPort : public QObject
{
    Q_OBJECT  // Add this line
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    // serial port parameters
    QString                                portName;
    qint32                                 baudRate;
    QSerialPort::Parity                    partyBit;
    QSerialPort::StopBits                  stopBit;
    QSerialPort::DataBits                  dataBit;

    // serial port
    QSerialPort *                          serialPort;

    // init
    bool                                  openSerialPort();
    bool                                  closeSerialPort();
    bool                                  writeData(const QByteArray &data);
    QByteArray                            readData();


private:

};
