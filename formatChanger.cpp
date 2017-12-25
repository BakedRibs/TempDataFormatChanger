#include "formatChanger.h"

FormatChanger::FormatChanger()
{
    openFileButton = new QPushButton("ChooseFile");                             //按钮和Label等初始化
    changeFormatButton = new QPushButton("ChangeFormat");
    changeFormatButton->setEnabled(false);
    filePathLabel = new QLabel("Click \'ChooseFile\' to load txt file");
    filePathLabel->setMinimumWidth(400);
    filePathLabel->setAlignment(Qt::AlignHCenter);
    filePath = "";

    QHBoxLayout *HLayout = new QHBoxLayout();                                   //横向布局，放置两个按钮
    HLayout->addWidget(openFileButton);
    HLayout->addWidget(changeFormatButton);

    QVBoxLayout *VLayout = new QVBoxLayout();                                   //纵向布局，放置横向布局和Label
    VLayout->addLayout(HLayout);
    VLayout->addWidget(filePathLabel);

    this->setLayout(VLayout);
    this->show();

    connect(openFileButton,SIGNAL(clicked()),this,SLOT(openFile()));            //连接'打开文件'按钮和openFile函数
    connect(changeFormatButton,SIGNAL(clicked()),this,SLOT(changeFormat()));    //连接'转换格式'按钮和changeFormat函数
}

FormatChanger::~FormatChanger()
{

}

void FormatChanger::openFile()
{
    //弹出打开文件对话框，让用户选择需要转换格式的txt文件
    filePath = QFileDialog::getOpenFileName(this, tr("Open Txt File"), ".", tr("Txt Files(*.txt)"));
    if(filePath != "")                                                          //成功打开文件，文件路径非空
    {
        filePathLabel->setText(filePath);                                       //在Label中显示路径
        changeFormatButton->setEnabled(true);                                   //可以开始格式转换
    }
    else                                                                        //未能成功打开文件，文件路径为空
    {
        filePathLabel->setText("Click \'ChooseFile\' to load txt file");        //在Label中显示说明
        changeFormatButton->setEnabled(false);                                  //不能进行格式转换
    }
}

void FormatChanger::changeFormat()
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))                       //尝试打开txt文件
    {
        filePathLabel->setText("Can not open file");
        changeFormatButton->setEnabled(false);
        return;
    }
    int fileSize = file.size()-2;                                               //去除文件中最后'\n'的两个字符
    int times = file.size()/405;                                                //每一帧数据在txt文档中的长度为405
    filePathLabel->setText(QString::number(fileSize));
    QTextStream in(&file);                                                      //生成QTextStream变量in

    QStringList list = filePath.split(".");
    QString newFilePath = list[0] + ".xml";
    QFile newFile(newFilePath);                                                 //生成新的xml文件，文件路径和文件名与txt文件相同，后缀从'txt'修改为'xml'
    if(!newFile.open(QFile::WriteOnly | QFile::Text))                           //打开文件
    {
        filePathLabel->setText("Can not write file");
        changeFormatButton->setEnabled(false);
        return;
    }
    QXmlStreamWriter xmlWriter(&newFile);                                       //生成QXmlStreamWriter，用xmlWriter在文件中写入xml结构和数据
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();                                             //xml文件起始行
    xmlWriter.writeStartElement("GridEyeData");                                 //GridEyeData起始
    for(int i=0;i<times;i++)
    {
        bool ok;
        QString line = in.read(405);
        line = line.trimmed();
        line = line.simplified();
        QStringList lineSL = line.split(" ");                                   //将405个字符的字符串以空格进行分解
        QStringList thermistor = lineSL.mid(3,2);                               //提取Thermistor对应的字节
        QStringList temperature = lineSL.mid(5,128);                            //提取Temperature对应的字节
        xmlWriter.writeStartElement("DataFrame"+QString::number(i+1));          //第i条DataFrame起始，xml文件中DataFrame数量与txt文件中数据帧的数量相同
        xmlWriter.writeTextElement("RC","8*8");

        char r1 = thermistor.at(1).toInt(&ok,16)&0xFF;                          //String转换为Char，注意高位和低位
        char r2 = thermistor.at(0).toInt(&ok,16)&0xFF;                          //参考grid-eye_communication-protocol-v1.0
        xmlWriter.writeTextElement("Thermistor",hexToTher(r1,r2));              //转换数据格式，并写入xml文件中

        for(int j=0;j<8;j++)
        {
            QString TempRow;
            for(int k=0;k<8;k++)
            {
                char c1 = temperature.at((j*8+k)*2+1).toInt(&ok,16)&0xFF;       //String转换为Char，注意高位和低位
                char c2 = temperature.at((j*8+k)*2).toInt(&ok,16)&0xFF;         //参考grid-eye_communication-protocol-v1.0
                TempRow += hexToTemp(c1,c2);                                    //转换数据格式，每8个温度对应于xml文件中的一行
                if(k != 7)
                {
                    TempRow += " ";
                }
            }
            xmlWriter.writeTextElement("DataRow"+QString::number(j+1),TempRow); //将温度写入xml文件中
        }
        xmlWriter.writeEndElement();                                            //第i条DataFrame结束
    }
    xmlWriter.writeEndElement();                                                //对应于GridEyeData结束
    xmlWriter.writeEndDocument();                                               //xml文件结束
    newFile.close();
    filePathLabel->setText("Format changed successfully");                      //转换成功
    changeFormatButton->setEnabled(false);
}

QString FormatChanger::hexToTemp(char t1,char t2)                               //将字符转化为温度值
{
    bool bool_arr[16];
    bool twoSComplement[12];                                                    //温度值的存放方式为12位补码，具体参见grid-eye_communication-protocol-v1.0
    int tempTimes = 0;
    for(int m=0;m<8;m++)
    {
        bool_arr[7-m] = ((t1 & 1<<m) != 0);                                     //将字符转化为二进制格式
        bool_arr[7-m+8] = ((t2 & 1<<m) != 0);
    }
    for(int m=0;m<12;m++)
    {
        twoSComplement[m] = bool_arr[m+4];                                      //提取16位二进制的后12位（因为只用到了后12位）
    }
    if(twoSComplement[0] == false)                                              //如果首位为0，补码为正数
    {
        for(int m=1;m<12;m++)
        {
            if(twoSComplement[m] == true)
            {
                tempTimes += pow(2,11-m);                                       //得到补码对应的十进制数
            }
        }
    }
    else                                                                        //如果首位为1，补码为负数
    {
        for(int m=11;m>=0;m--)
        {
            if(twoSComplement[m] == true)
            {
                twoSComplement[m] = false;
                break;
            }
            else
            {
                twoSComplement[m] = true;
            }
        }
        for(int m=1;m<12;m++)
        {
            if(twoSComplement[m] == false)
            {
                tempTimes -= pow(2,11-m);                                       //处理结束后，得到补码对应的十进制数
            }
        }
    }
    return QString::number(tempTimes * 0.25);
    //将倍数乘以0.25，得到温度值，此0.25是参考https://eu.industrial.panasonic.com/grideye-evalkit中提供的源代码
}

QString FormatChanger::hexToTher(char t1, char t2)                              //将字符转化为Thermistor值
{
    bool bool_arr[16];
    bool twoSComplement[12];                                                    //Thermistor的存放方式为12位带符号数，具体参见grid-eye_communication-protocol-v1.0
    int tempTimes = 0;
    for(int m=0;m<8;m++)
    {
        bool_arr[7-m] = ((t1 & 1<<m) != 0);                                     //将字符转化为二进制格式
        bool_arr[7-m+8] = ((t2 & 1<<m) != 0);
    }
    for(int m=0;m<12;m++)
    {
        twoSComplement[m] = bool_arr[m+4];                                      //提取16位二进制的后12位（因为只用到了后12位）
    }
    for(int m=1;m<12;m++)
    {
        if(twoSComplement[m] == true)
        {
            if(twoSComplement[0] == false)
            {
                tempTimes += pow(2,11-m);                                       //将二进制数转换为signed int值
            }
            else
            {
                tempTimes -= pow(2,11-m);
            }
        }
    }
    return QString::number(tempTimes * 0.0125);
    //将倍数乘以0.0125，得到Thermistor，此0.0125是参考https://eu.industrial.panasonic.com/grideye-evalkit中提供的源代码
}
