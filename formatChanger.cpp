#include "formatChanger.h"

FormatChanger::FormatChanger()
{
    openFileButton = new QPushButton("ChooseFile");
    changeFormatButton = new QPushButton("ChangeFormat");
    changeFormatButton->setEnabled(false);
    filePathLabel = new QLabel("Click \'ChooseFile\' to load txt file");
    filePathLabel->setMinimumWidth(400);
    filePathLabel->setAlignment(Qt::AlignHCenter);
    filePath = "";

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addWidget(openFileButton);
    HLayout->addWidget(changeFormatButton);

    QVBoxLayout *VLayout = new QVBoxLayout();
    VLayout->addLayout(HLayout);
    VLayout->addWidget(filePathLabel);

    this->setLayout(VLayout);
    this->show();

    connect(openFileButton,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(changeFormatButton,SIGNAL(clicked()),this,SLOT(changeFormat()));
}

FormatChanger::~FormatChanger()
{

}

void FormatChanger::openFile()
{
    filePath = QFileDialog::getOpenFileName(this, tr("Open Txt File"), ".", tr("Txt Files(*.txt)"));
    if(filePath != "")
    {
        filePathLabel->setText(filePath);
        changeFormatButton->setEnabled(true);
    }
    else
    {
        filePathLabel->setText("Click \'ChooseFile\' to load txt file");
        changeFormatButton->setEnabled(false);
    }
}

void FormatChanger::changeFormat()
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        filePathLabel->setText("Can not open file");
        changeFormatButton->setEnabled(false);
        return;
    }
    int fileSize = file.size()-2;
    int times = file.size()/405;
    filePathLabel->setText(QString::number(fileSize));
    QTextStream in(&file);

    QStringList list = filePath.split(".");
    QString newFilePath = list[0] + ".xml";
    QFile newFile(newFilePath);
    if(!newFile.open(QFile::WriteOnly | QFile::Text))
    {
        filePathLabel->setText("Can not write file");
        changeFormatButton->setEnabled(false);
        return;
    }
    QXmlStreamWriter xmlWriter(&newFile);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("GridEyeData");
    for(int i=0;i<times;i++)
    {
        bool ok;
        QString line = in.read(405);
        line = line.trimmed();
        line = line.simplified();
        QStringList lineSL = line.split(" ");
        QStringList thermistor = lineSL.mid(3,2);
        QStringList temperature = lineSL.mid(5,128);
        xmlWriter.writeStartElement("DataFrame"+QString::number(i+1));
        xmlWriter.writeTextElement("RC","8*8");
        for(int j=0;j<8;j++)
        {
            QString TempRow;
            for(int k=0;k<8;k++)
            {
                char c1 = temperature.at((j*8+k)*2+1).toInt(&ok,16)&0xFF;
                char c2 = temperature.at((j*8+k)*2).toInt(&ok,16)&0xFF;
                TempRow += hexToTemp(c1,c2);
                if(k != 7)
                {
                    TempRow += " ";
                }
            }
            xmlWriter.writeTextElement("DataRow"+QString::number(j+1),TempRow);
        }
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    newFile.close();
}

QString FormatChanger::hexToTemp(char t1,char t2)
{
    QByteArray temp;
    temp.append(t1);
    temp.append(t2);
    bool bool_arr[16];
    bool twoSComplement[12];
    int tempTimes = 0;
    for(int m=0;m<8;m++)
    {
        bool_arr[7-m] = ((t1 & 1<<m) != 0);
        bool_arr[7-m+8] = ((t2 & 1<<m) != 0);
    }
    for(int m=0;m<12;m++)
    {
        twoSComplement[m] = bool_arr[m+4];
    }
    if(twoSComplement[0] == false)
    {
        for(int m=1;m<12;m++)
        {
            if(twoSComplement[m] == true)
            {
                tempTimes += pow(2,11-m);
            }
        }
    }
    else
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
                tempTimes += pow(2,11-m);
            }
        }
    }
    return QString::number(tempTimes * 0.25);
}
