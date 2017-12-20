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

}

void FormatChanger::txtToXml()
{

}
