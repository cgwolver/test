#include "capbarevent.h"
#include <QMessageBox>

CapBarEvent::CapBarEvent(QObject *parent) : QObject(parent)
{
}

void CapBarEvent::onClose()
{
    emit close();

}

void CapBarEvent::onMaxi()
{
    emit maximize();
}

void CapBarEvent::onMini()
{
     emit minimize();
}

void CapBarEvent::onDbClicked()
{
    emit dbclicked();
}
void CapBarEvent::onClicked()
{
    emit sgclicked();
}
