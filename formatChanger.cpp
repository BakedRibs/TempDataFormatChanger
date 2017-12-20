#include "formatChanger.h"

FormatChanger::FormatChanger()
{
    openFileButton = new QPushButton("ChooseFile");
    changeFormatButton = new QPushButton("ChangeFormat");
    filePathLabel = new QLabel();

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addWidget(openFileButton);
    HLayout->addWidget(changeFormatButton);

    QVBoxLayout *VLayout = new QVBoxLayout();
}

FormatChanger::~FormatChanger()
{

}

void FormatChanger::openFile()
{

}

void FormatChanger::changeFormat()
{

}
