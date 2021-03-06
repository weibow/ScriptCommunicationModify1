/***************************************************************************
**                                                                        **
**  ScriptCommunicator, is a tool for sending/receiving data with several **
**  interfaces.                                                           **
**  Copyright (C) 2014 Stefan Zieker                                      **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Stefan Zieker                                        **
**  Website/Contact: http://sourceforge.net/projects/scriptcommunicator/  **
****************************************************************************/

#include <QApplication>
#include "mainwindow.h"
#include <QStringList>
#include <scriptFile.h>
#include <QDir>
#include <QCryptographicHash>
#include <QLibrary>
#include <QProcess>
#include <QStandardPaths>
#include <QTextCodec>
#include <QStack>
#include <QQueue>
#include <iostream>

using namespace  std;

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

///Is set to true if a thread has been terminated.
///This variabke is used un the main function.
bool g_aThreadHasBeenTerminated = false;

///The current SCEZ folder.
QString g_currentScezFolder = "";

#ifdef Q_OS_WIN32
static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &message)
{
    (void)type;
    (void)context;
    OutputDebugString(reinterpret_cast<const wchar_t *>(message.utf16()));
}
#endif

///Deletes the current SCEZ folder.
void deleteCurrentScezFolder(void)
{
    if(!g_currentScezFolder.isEmpty())
    {
        QString program = QCoreApplication::applicationDirPath() + "/DeleteFolder";
        program = QCoreApplication::applicationDirPath() + "/DeleteFolder";

#ifdef QT_DEBUG
        QDir(g_currentScezFolder).removeRecursively();
#else
        QProcess::startDetached(program, QStringList() << g_currentScezFolder, QString(""));
#endif
        g_currentScezFolder = "";
    }
}

void GetMemory(char *p, int num)
{
    p =(char*)malloc(sizeof(char) *num);
}

void GetMemory2(char **p, int num)
{
    *p = (char*)malloc(sizeof(char) * num);
}


void test(void)
{
    int p = 10;
    int *ptr = &p;
    char *s = NULL;

    GetMemory2(&s, 100);
    strcpy(s, "hello\n");
    printf(s);
    //TEST code Area
    cout << "p" << endl;

    cout << "p Value=" << p << endl;
    cout << "p pointer address" << &p << endl;
    cout << "*ptr value" << *ptr ;
    cout << "ptr address" << ptr << endl;
    QStack <int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);

    while(!stack.isEmpty()) {
        cout << stack.pop() << endl;
    }

    QQueue <int> queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    while (!queue.isEmpty())
    {
        cout << queue.dequeue() << endl;
    }

    bool ok;
    double d;
    QLocale::setDefault(QLocale::C);

    d = QString("1234.56").toDouble(&ok);
    if (ok) qDebug("TRUE");
    else qDebug("FALSE");

    QLatin1String latin("Qt");
    QString str = QString("Qt");

    if (str == latin)
        qDebug("Equal.2");
    else
        qDebug("Not equal.");

    bool is_equal = latin.operator ==(str);

    if (is_equal)
        qDebug("Equal");
    else
        qDebug("Not Equal");

//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));    //设置本地编码对象
//        //啊的gbk编码: 0xB0A1            啊的utf8编码:0xE5958A
//        //哈的gbk编码: 0xB9FE            哈的utf8编码:0xE59388
//
//        QString str1="啊哈";               //对应编码为:  0xB0A1  0xB9FE
//
//        QTextCodec *gbk = QTextCodec::codecForName("gbk");
//
//        QString unicode=gbk->toUnicode(str1.toLocal8Bit());    //通过gbk编码对象将啊哈转为utf-16
//
//
//        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
//
//        QByteArray arr=utf8->fromUnicode(unicode);           //从Unicode编码转为自身编码类型(utf-8)
//
//        qDebug()<<arr.size();
//        for(int i=0; i <arr.size();i++)
//        {
//            qDebug("%x",(unsigned char)arr[i]);
//        }

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));    //设置本地编码对象
//utf8向gdk转换的过程是utf8->unicode, unicode->gdk
//在PC，Unicode一般代表utf-16,而utf-8是单独列出来的
//啊的gbk编码: 0xB0A1            啊的utf8编码:0xE5958A
//哈的gbk编码: 0xB9FE            哈的utf8编码:0xE59388

    unsigned char encode[6]={0xe5,0x95,0x8A,0xE5,0x93,0x88};             	//啊哈 utf-8编码
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");       			//获取UTF-8编码对象
    QString unicode = utf8->toUnicode((const char*)encode,6);               //通过UTF-8编码对象将啊哈转为utf-16

    QTextCodec *gbk = QTextCodec::codecForName("GBK");          //获取GBK编码对象
    QByteArray arr=gbk->fromUnicode(unicode);                   //从Unicode编码转为自身编码类型(GBK)i
    qDebug() << gbk;
    qDebug() << QString::fromLocal8Bit(arr);                     //打印GBK码

    qDebug() << arr.size();
    for(int i=0; i <arr.size();i++)
    {
        qDebug("%x",(unsigned char)arr[i]);
    }
}


void displayGBKFile()
{
    QFile file("main.c");
    file.open(QFile::ReadOnly | QFile::Text);

    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    QString content = gbk->toUnicode(file.readAll());

    qDebug()<<"Display GBK.txt file\n" << content;
    file.close();
    char buffer[2048];
    qDebug() << "start read\n";
    if (file.open(QIODevice::ReadOnly)){
        qint64 lineLen = file.readLine(buffer, sizeof(buffer));
        if (lineLen != -1)
        {
        qDebug() << buffer;
        }
        file.close();
    }
    QFile data("data.c");
    if (data.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&data);
        out.setCodec("utf-8");
        out << utf8->fromUnicode(content);
        qDebug() << utf8;
    }
    qDebug() << "\nhello\n";
    QFile file2("main.c");
    if (file2.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "dde\n";;
        QTextStream stream(&file2);
        const QString &text = stream.readAll();
        stream.seek(0);
        stream.setCodec(QTextCodec::codecForName("utf-8"));
        stream.setGenerateByteOrderMark(true);
        stream << text;
        stream.seek(0);
    } else {
    }
}

void GBK2utf8_convert(QString filename)
{
   QFile file(filename);

   qDebug() << filename;
   if (file.open(QIODevice::ReadWrite | QIODevice::Text))
   {
       QTextStream stream(&file);
       const QString &text = stream.readAll();
       stream.seek(0);
       stream.setCodec(QTextCodec::codecForName("utf-8"));
       stream.setGenerateByteOrderMark(true); //Add the BOM(Byte Order Mark)
       stream << text;
       file.close();
   }
}


void displayUTF8File()
{
    QFile file("main.c");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QString content = codec->toUnicode(file.readAll());
    qDebug()<<"Display UTF8.txt File:" << content;
}


void gbkutf(void)
{
    displayGBKFile();
  //  displayUTF8File();
}

/*
 * file name display
 */
qint64 du(const QString &path)
{
    QDir dir(path);
    QString pathname;
    qint64 size = 0;
    QStringList string;
    string << "*.c" << "*.h";

    foreach (QFileInfo fileInfo, dir.entryInfoList(string, QDir::Files, QDir::Name)) {
        size += fileInfo.size();
        if (pathname != dir.absolutePath()){
          //  qDebug() << dir.absolutePath();
            pathname = dir.absolutePath();
        }
        qDebug() << "see" << fileInfo.fileName();
//        qDebug() << fileInfo.absoluteFilePath();
//        pathname = pathname + fileInfo.fileName();
        pathname = fileInfo.absoluteFilePath();
        GBK2utf8_convert(pathname);
    }

    foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        size += du(path + QDir::separator() + subDir);
        qDebug() <<"I'm" << path + QDir::separator() + subDir;
    }
    return size;
}

int main(int argc, char *argv[])
{
    QApplication* a = new QApplication(argc, argv);
    QStringList extraPluginPaths;
    QStringList scriptArguments;
    QStringList extraLibPaths;
    QString configFile;
    int result = 0;

    QStringList scripts;
    bool withScriptWindow = false;
    bool scriptWindowIsMinimized = true;
    QString minimumScVersion;
    QString iconFile;
    QString ("hello%1%2").arg("ni hao ").arg("jos").arg("33");

    QFile data("data.txt");
    if (data.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream out(&data);
        out << uppercasedigits <<"de" << QObject::tr("score") <<qSetFieldWidth(10) << center << 90 << endl;
    }
//    test();
//    gbkutf();
    QString path = QDir::currentPath();
//    path = path + "/Scale_Machines";
    path = "./Scale_Machines";

    qDebug() << path;
    qDebug() << du(path);
//    qDebug() << QDir::currentPath();
//    qDebug() << (QDir::currentPath() + "/Scale_Machines");
//    qDebug() << du(QDir::currentPath() + "/Scale_Machines");

#ifdef Q_OS_WIN32
    qInstallMessageHandler(messageHandler);
#endif

//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);

    //Parse all arguments.
    for(int i = 1; i < argc; i++)
    {
        QString currentArg = QString::fromLocal8Bit(argv[i]);

        if(currentArg.indexOf("-") == 0)
        {//The current argument is not a script.

            if(QString("-withScriptWindow") == currentArg)
            {
                withScriptWindow = true;
            }
            else if(QString("-notMinimized") == currentArg)
            {
                scriptWindowIsMinimized = false;
            }
            else if(currentArg.startsWith("-P"))
            {//Extra plugin path.
                extraPluginPaths << currentArg.remove("-P");
            }
            else if(currentArg.startsWith("-L"))
            {//Extra library path.
                extraLibPaths << currentArg.remove("-L");
            }
            else if(currentArg.startsWith("-C"))
            {//Config file.
                configFile = currentArg.remove("-C");
            }
            else if(currentArg.startsWith("-A"))
            {//Script argument (script can read these arguments withScriptThread::getScriptArguments).
                scriptArguments << currentArg.remove("-A");
            }
            else if(currentArg.startsWith("-minScVersion"))
            {
                minimumScVersion = currentArg.remove("-minScVersion");
            }
            else if(currentArg.startsWith("-I"))
            {
                iconFile = currentArg.remove("-I");
            }
            else
            {
                QMessageBox box(QMessageBox::Warning, "ScriptCommunicator", QString("unknown command line argument: ") + currentArg);
                QApplication::setActiveWindow(&box);
                box.exec();
                return -1;
            }
        }
        else
        {//The current argument is a script, a SCE or a SCEZ File.
            if(currentArg.endsWith(".sce"))
            {//The current argument is an SCE file.
                if(!MainWindow::parseSceFile(currentArg.replace("\\", "/"), &scripts, &extraPluginPaths, &scriptArguments,
                                             &withScriptWindow, &scriptWindowIsMinimized, &minimumScVersion, &extraLibPaths))
                {
                    return -1;
                }
            }
            else if(currentArg.endsWith(".scez"))
            {
                if(MainWindow::checkScezFileHash(currentArg))
                {
                    g_currentScezFolder = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + QString("%1").arg(QDateTime::currentDateTime().currentMSecsSinceEpoch());
                    if(ScriptFile::extractZipFile(currentArg, g_currentScezFolder))
                    {
                        QString currentSceFile;
                        QStringList dirContent = ScriptFile(0, "", false).readDirectory(g_currentScezFolder, false, false, true, false);

                        for(auto el : dirContent)
                        {
                            if(el.endsWith(".sce"))
                            {
                                currentSceFile = el;
                                break;
                            }
                        }

                        if(!currentSceFile.isEmpty())
                        {
                            if(!MainWindow::parseSceFile(currentSceFile.replace("\\", "/"), &scripts, &extraPluginPaths, &scriptArguments,
                                                         &withScriptWindow, &scriptWindowIsMinimized, &minimumScVersion, &extraLibPaths))
                            {
                                return -1;
                            }
                        }
                        else
                        {
                            QMessageBox box(QMessageBox::Warning, "ScriptCommunicator", QString("no sce file found in: ") + currentArg);
                            QApplication::setActiveWindow(&box);
                            box.exec();
                            return -1;
                        }
                    }
                    else
                    {
                        QMessageBox box(QMessageBox::Warning, "ScriptCommunicator", QString("could not unzip: ") + currentArg);
                        QApplication::setActiveWindow(&box);
                        box.exec();
                        return -1;
                    }

                }//if(MainWindow::checkScezFileHash(currentArg))
                else
                {
                    return -1;
                }
            }
            else
            {
#ifdef Q_OS_WIN32
                scripts << currentArg.replace("\\", "/");
#else
                scripts << currentArg;
#endif
            }
        }
    }

    if(!MainWindow::checkParsedScVersion(minimumScVersion))
    {
        return -1;
    }

    //Load all libraries in extraLibPaths.

    bool oneLibraryLoadFailed = true;
    for(int i = 0; (i < 5) && oneLibraryLoadFailed; i++)
    {
        oneLibraryLoadFailed = false;
        for(auto el : extraLibPaths)
        {
            QDir dir(el);
            QStringList foundEntries = dir.entryList(QDir::Files);

            //Load all found files.
            for(qint32 i = 0; i < foundEntries.length(); i++)
            {
                QString libName = foundEntries[i];

                QLibrary lib(el + "/" + libName);
                if(!lib.load())
                {
                    oneLibraryLoadFailed = true;
                }
            }
        }
    }

    try
    {
        MainWindow* w = new MainWindow(scripts, withScriptWindow, scriptWindowIsMinimized, extraPluginPaths,
                                       scriptArguments, configFile, iconFile);

        if(scripts.isEmpty())
        {
            w->show();
        }
        result = a->exec();

        if(!w->closedByScript())
        {//No script has called exitScriptCommunicator.
            if(!scripts.isEmpty())
            {
                if(!withScriptWindow)
                {
                    //If no script window is shown, the last running script
                    //will exit ScriptCommunicator(in withScriptWindow)
                    while(1)
                    {
                        a->processEvents();
                    }
                }
                w->close();
            }
        }

        if(w->closedByScript())
        {//A script has called exitScriptCommunicator.

            //Get the exit code.
            result = w->getExitCode();
        }

        //Note: The main window must be deleted before QApplication.
        delete w;

        if(!g_aThreadHasBeenTerminated)
        {//No thread has been ternminated.
            //If a thread has been terminated then the Destructor of QApplication will crash.
            delete a;
        }

    }
    catch(...)
    {

    }

    deleteCurrentScezFolder();

    return result;
}
