#include "selfdrive.h"

//设置程序自启动 appPath程序路径
void SetProcessAutoRunSelf()
{
    QString application_name = QApplication::applicationName(); //获取应用名称
    QSettings* settings = new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat); //创建QSetting, 需要添加QSetting头文件
    QString application_path = QApplication::applicationFilePath(); //找到应用的目录
    settings->setValue(application_name, application_path.replace("/", "\\")); //写入注册表
}
