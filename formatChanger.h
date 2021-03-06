#ifndef FORMATCHANGER_H
#define FORMATCHANGER_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QXmlStreamWriter>
#include "math.h"

class FormatChanger : public QDialog
{
    Q_OBJECT

public:
    FormatChanger();
    ~FormatChanger();

    QPushButton *openFileButton;                        //选择路径打开txt文件
    QPushButton *changeFormatButton;                    //开始转换文件格式
    QLabel *filePathLabel;                              //显示文件路径和文件名
    QString filePath;                                   //保存文件路径
    QString hexToTemp(char t1,char t2);                 //将十六进制字符串转化为64个温度值
    QString hexToTher(char t1,char t2);                 //将十六进制字符串转化为1个Thermistor

public slots:
    void openFile();                                    //根据路径打开txt文件
    void changeFormat();                                //转换文件格式
};

#endif
