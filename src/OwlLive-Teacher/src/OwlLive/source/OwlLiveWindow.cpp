#include "OwlLiveWindow.h"
#include <QIcon>

OwlLiveWindow::OwlLiveWindow(QWidget* parent)
	: QWidget(parent)
{
	setWindowTitle("OwlLive");
	setWindowIcon(QIcon(":/all/assets/Icon.png"));
}

OwlLiveWindow::~OwlLiveWindow()
{

}
