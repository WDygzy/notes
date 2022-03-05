#include "menu.h"

void menu(Ui::MainWindow* ui, MainWindow* thisWindow)
{
    ui->Close->setFlat(true);
    ui->Max->setFlat(true);
    ui->Min->setFlat(true);
    ui->edit->setFlat(true);

    //关闭
    QObject::connect(ui->Close, &QPushButton::clicked, [=]() {
        thisWindow->close();
    });

    //最小化
    QObject::connect(ui->Min, &QPushButton::clicked, [=]() {
        thisWindow->showMinimized();
    });

    //最大化
    QObject::connect(ui->Max, &QPushButton::clicked, [=]() mutable {
        if (!thisWindow->isMaxWindow) {
            thisWindow->preGeometry = thisWindow->frameGeometry();
            thisWindow->move(thisWindow->clientRect.x(), thisWindow->clientRect.y());
            thisWindow->resize(thisWindow->clientRect.width(), thisWindow->clientRect.height());
            thisWindow->isMaxWindow = true;
        } else {
            thisWindow->move(thisWindow->preGeometry.x(), thisWindow->preGeometry.y());
            thisWindow->resize(thisWindow->preGeometry.width(), thisWindow->preGeometry.height());
            thisWindow->isMaxWindow = false;
        }
        thisWindow->changeReturnIcon();
    });

    QObject::connect(ui->edit, &QPushButton::clicked, [=]() {
        if (thisWindow->showTextCanBeEdit) {
            ui->edit->setIcon(QIcon(":/menu/cantedit.png"));
            thisWindow->showTextCanBeEdit = false;
            ui->showText->setReadOnly(true);
        } else {
            ui->edit->setIcon(QIcon(":/menu/edit.png"));
            thisWindow->showTextCanBeEdit = true;
            ui->showText->setReadOnly(false);
        }
    });
}
