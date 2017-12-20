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
    QString line = in.read(405);

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
        xmlWriter.writeStartElement("DataFrame"+QString::number(i+1));
    }
}
