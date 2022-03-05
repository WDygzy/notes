#include "mainwindow.h"
#include "menu.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //********************************************//
    //系统函数

    //设置鼠标位于窗口边缘样式
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    ui->mainwidget->setMouseTracking(true);
    ui->mainDisplayWidget->setMouseTracking(true);
    ui->showText->setMouseTracking(true);
    ui->titleBar->setMouseTracking(true);
    ui->Close->setMouseTracking(true);
    ui->Min->setMouseTracking(true);
    ui->Max->setMouseTracking(true);

    //初始化图标
    ui->Close->setIcon(QIcon(":/menu/close.png"));
    ui->Max->setIcon(QIcon(":/menu/up.png"));
    ui->Min->setIcon(QIcon(":/menu/min.png"));
    ui->edit->setIcon(QIcon(":/menu/edit.png"));
    //设置任务栏图标
    this->setWindowIcon(QIcon(":/menu/ico.png"));

    //获取当前窗口
    thisGeometry = this->frameGeometry();

    //获取最大可用窗口
    desktopWidget = QApplication::desktop();
    clientRect = desktopWidget->availableGeometry();
    //设置最大窗口为最大可用窗口
    this->setMaximumSize(clientRect.width(), clientRect.height());

    //去除系统边框
    setWindowFlags(Qt::FramelessWindowHint);

    //初始文本框可以更改
    ui->showText->setReadOnly(false);
    //文本框字体大小
    QFont font = ui->showText->font();
    font.setPointSize(10);
    ui->showText->setFont(font);

    //设置文本框背景
    QColor textColor;
    textColor.setRgb(242, 242, 242, 255);
    QPalette pal = ui->showText->palette();
    pal.setBrush(QPalette::Base, textColor);
    ui->showText->setPalette(pal);

    //设置窗口最前面
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    //********************************************//
    //自定义函数

    //设置个人菜单
    menu(ui, this);

    //获取文件内容
    getShowText();
    //获取数据内容
    getDat();

    QTimer* timer = new QTimer(this);
    //存储文件内容
    connect(timer, SIGNAL(timeout()), this, SLOT(saveShowText()));
    //存储数据内容
    connect(timer, SIGNAL(timeout()), this, SLOT(saveDat()));
    timer->start(500); // 每隔1s
}

MainWindow::~MainWindow()
{
    thisGeometry = this->frameGeometry();
    saveShowText();
    saveDat();
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    //窗口拖动
    if (event->button() == Qt::LeftButton) {
        startPoint = event->globalPos();
        thisGeometry = this->frameGeometry();
        qDebug() << thisGeometry.x();
        isPressButton = true;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    //边缘时鼠标样式改变
    if (!isPressButton) {
        mouseState = 0;
        if (!isMaxWindow && abs(event->globalX() - this->frameGeometry().left()) <= 2) {
            mouseState |= MOUSE_LEFT;
        }
        if (!isMaxWindow && abs(event->globalX() - this->frameGeometry().right()) <= 2) {
            mouseState |= MOUSE_RIGHT;
        }
        if (!isMaxWindow && abs(event->globalY() - this->frameGeometry().top()) <= 2) {
            mouseState |= MOUSE_TOP;
        }
        if (!isMaxWindow && abs(event->globalY() - this->frameGeometry().bottom()) <= 2) {
            mouseState |= MOUSE_BOTTOM;
        }
        if (mouseState == MOUSE_LEFT_TOP || mouseState == MOUSE_RIGHT_BOTTOM) {
            this->setCursor(Qt::SizeFDiagCursor);
        } else if (mouseState == MOUSE_LEFT_BOTTOM || mouseState == MOUSE_RIGHT_TOP) {
            this->setCursor(Qt::SizeBDiagCursor);
        } else if (mouseState == MOUSE_LEFT || mouseState == MOUSE_RIGHT) {
            this->setCursor(Qt::SizeHorCursor);
        } else if (mouseState == MOUSE_TOP || mouseState == MOUSE_BOTTOM) {
            this->setCursor(Qt::SizeVerCursor);
        } else {
            unsetCursor();
        }
    }
    //窗口拖动
    else {
        if (mouseState != 0) {
            movePoint = event->globalPos();
            if (mouseState & MOUSE_LEFT) {
                thisGeometry.setLeft(movePoint.x());
            }
            if (mouseState & MOUSE_RIGHT) {
                thisGeometry.setRight(movePoint.x());
            }
            if (mouseState & MOUSE_TOP) {
                thisGeometry.setTop(movePoint.y());
            }
            if (mouseState & MOUSE_BOTTOM) {
                thisGeometry.setBottom(movePoint.y());
            }
            this->move(thisGeometry.x(), thisGeometry.y());
            this->resize(thisGeometry.width(), thisGeometry.height());
            isMaxWindow = false;
        } else {
            movePoint = event->globalPos();
            QPoint displacement = movePoint - startPoint;
            if (!isMaxWindow) {
                if (thisGeometry.right() + displacement.x() <= clientRect.right()
                    && thisGeometry.left() + displacement.x() >= clientRect.left()
                    && thisGeometry.bottom() + displacement.y() <= clientRect.bottom()) {
                    this->move(thisGeometry.x() + displacement.x(), thisGeometry.y() + displacement.y());
                }
            } else {
                double a = (1.0 - double(preGeometry.width()) / double(clientRect.width()));
                double b = (1.0 - double(preGeometry.height()) / double(clientRect.height()));
                int xLocation = int(double(movePoint.x() - clientRect.x()) * a) + clientRect.x();
                int yLocation = int(double(movePoint.y() - clientRect.y()) * b) + clientRect.y();
                this->move(xLocation, yLocation);
                this->resize(preGeometry.width(), preGeometry.height());
                isMaxWindow = false;
                thisGeometry.setX(xLocation);
                thisGeometry.setY(yLocation);
            }
            changeReturnIcon();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    //窗口拖动
    thisGeometry = this->frameGeometry();
    isPressButton = false;

    //窗口拖到上方最大化
    if (thisGeometry.top() < 0 && event->globalY() == 0) {
        preGeometry = this->frameGeometry();
        this->move(clientRect.x(), clientRect.y());
        this->resize(clientRect.width(), clientRect.height());
        isMaxWindow = true;
    }
    changeReturnIcon();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    //双击向上还原
    if (!isMaxWindow) {
        preGeometry = this->frameGeometry();
        this->move(clientRect.x(), clientRect.y());
        this->resize(clientRect.width(), clientRect.height());
        isMaxWindow = true;
    }
    //双击向下还原
    else if (isMaxWindow && preGeometry.y() >= 0) {
        this->move(preGeometry.x(), preGeometry.y());
        this->resize(preGeometry.width(), preGeometry.height());
        isMaxWindow = false;
    }
    //当preGeometry.y()<0时将其y()移至0处
    else if (isMaxWindow && preGeometry.y() <= 0) {
        int preHeight = preGeometry.height();
        preGeometry.setY(0);
        preGeometry.setHeight(preHeight);
        this->move(preGeometry.x(), preGeometry.y());
        this->resize(preGeometry.width(), preGeometry.height());
        isMaxWindow = false;
    }
    changeReturnIcon();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // esc退出程序
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}

void MainWindow::changeReturnIcon()
{
    //还原图标改变
    if (isMaxWindow) {
        ui->Max->setIcon(QIcon(":/menu/down.png"));
    } else {
        ui->Max->setIcon(QIcon(":/menu/up.png"));
    }
}

void MainWindow::getShowText()
{
    //获取文本内容
    QFile getFilename(appCurrentFilePath + "/content.txt");
    if (!getFilename.exists()) {
        bool result = getFilename.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!result) {
            std::cout << "can't write file!";
        }
    }
    getFilename.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream txtInput(&getFilename);
    QString lineStr;
    while (!txtInput.atEnd()) {
        lineStr = txtInput.readLine(); //读取数据
        ui->showText->appendPlainText(lineStr);
    }
    getFilename.close();
}

void MainWindow::saveShowText()
{
    //储存文本内容
    QString showTextString = ui->showText->toPlainText();
    QFile saveFileName(appCurrentFilePath + "/content.txt");
    bool result = saveFileName.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!result) {
        std::cout << "can't write file!";
    }
    QTextStream out(&saveFileName); //写入
    out << showTextString;
    saveFileName.close();
}

void MainWindow::getDat()
{
    QFile ExpandData(appCurrentFilePath + "/thisGeometry.dat");
    int thisWindow[4];
    if (!ExpandData.open(QIODevice::ReadOnly)) {
        return;
    } else {
        /*文本输出流，用于保存数据*/
        QTextStream In(&ExpandData);
        for (int i = 0; i < 4; i++) {
            In >> thisWindow[i];
        }
        ExpandData.close();
    }
    this->move(thisWindow[0], thisWindow[1]);
    this->resize(thisWindow[2], thisWindow[3]);
}

void MainWindow::saveDat()
{
    QByteArray array;
    int thisWindow[4];
    thisWindow[0] = thisGeometry.x();
    thisWindow[1] = thisGeometry.y();
    thisWindow[2] = thisGeometry.width();
    thisWindow[3] = thisGeometry.height();
    QFile ExpandData(appCurrentFilePath + "/thisGeometry.dat");
    if (ExpandData.open(QIODevice::WriteOnly)) {
        /*文本输出流，用于保存数据*/
        QTextStream Out(&ExpandData);
        /*加入空格的好处是以流的形式读取恰好是相反的操作，这样一来方便快速对参数的读取*/
        for (int i = 0; i < 4; i++) {
            Out << thisWindow[i];
            Out << ' ';
        }
        ExpandData.close();
    }
}
