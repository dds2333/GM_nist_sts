#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/stat_fncs.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <memory.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <QTime>
#include <QVector>
#include <QCloseEvent>
#include <QThread>
#include "QDebug"
#include "global.h"

QByteArray MainWindow::byteData;
int MainWindow::byteData_Len;
int MainWindow::bit_Len=0;
BitSequence* MainWindow::rand_bits;

QVector<QByteArray>MainWindow::m_byteData;
QVector<int>MainWindow::m_byteData_Len;
QVector<int>MainWindow::m_bit_Len;
QVector<BitSequence*>MainWindow::m_rand_bits;

bool MainWindow::flag=false;
bool MainWindow::is_multi;
QStringList MainWindow::file_list;
QStringList MainWindow::f_name;

int MainWindow::t_pass_num=0;
int MainWindow::t_pass_num_2=0;

bool MainWindow::t_finish=false;
bool MainWindow::t_finish_2=false;

bool MainWindow::is_pass = true;

//统计线程1单项通过数
int MainWindow::pass_num=0;//单比特频数

int MainWindow::pass_num_2=0;//块内频数

int MainWindow::pass_num_3_4=0;//扑克
int MainWindow::pass_num_3_8=0;

int MainWindow::pass_num_4_2_1=0;//重叠子序列
int MainWindow::pass_num_4_2_2=0;
int MainWindow::pass_num_4_5_1=0;
int MainWindow::pass_num_4_5_2=0;

int MainWindow::pass_num_5=0;//游程总数

int MainWindow::pass_num_6=0;//游程分布

int MainWindow::pass_num_7=0;//块内最大"1"游程

int MainWindow::pass_num_8_3=0;//二元推导
int MainWindow::pass_num_8_7=0;

int MainWindow::pass_num_9_1=0;//自相关
int MainWindow::pass_num_9_2=0;
int MainWindow::pass_num_9_8=0;
int MainWindow::pass_num_9_16=0;

int MainWindow::pass_num_10=0;//矩阵秩

int MainWindow::pass_num_11_f=0;//累加和
int MainWindow::pass_num_11_b=0;

int MainWindow::pass_num_12_2=0;//近似熵
int MainWindow::pass_num_12_5=0;

int MainWindow::pass_num_13=0;//线性复杂度

int MainWindow::pass_num_14=0;//Maurer通用统计

int MainWindow::pass_num_15=0;//离散傅立叶

//detect_thread* MainWindow::pthread=new detect_thread();
//detect_thread_2* MainWindow::pthread_2=new detect_thread_2();
detect_thread* MainWindow::pthread = nullptr;
detect_thread_2* MainWindow::pthread_2 = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qDebug()<<QThread::currentThread();

    qRegisterMetaType<Data>("Data");
    qRegisterMetaType<Data_2>("Data_2");
    //qRegisterMetaType<detect_thread>("detect_thread&");

    //setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);//禁止最大化
    //setFixedSize(this->width(),this->height());//禁止拖动窗口大小
    setWindowIcon(QIcon("binary_code.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
    //m_thread->quit();
}

//选择文件
void MainWindow::on_pushButton_clicked()
{
    //QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="选择待检测的随机数二进制文件";
    QString filter="二进制文件(*.bin);;所有文件(*.*)";//文件过滤器
    //记住上次选择的路径
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString();

    QString fileName=QFileDialog::getOpenFileName(this,dlgTitle,lastPath,filter);
    if (!fileName.isEmpty())
    {
        is_multi=false;
        if(!file_list.isEmpty())
        {
            file_list.clear();
        }
        setting.setValue("LastFilePath", fileName);
        ui->lineEdit->setText(fileName);
        QFile file(fileName);
        bool isOK=file.open(QFile::ReadOnly);
        if(!isOK){
            QMessageBox::critical(this,"错误","打开文件失败!");
            return;
        }
        byteData=file.readAll();
        byteData_Len=byteData.length();
        bit_Len=byteData_Len*8;
        QString len=QString::number(byteData_Len);
        //ui->textBrowser->append(len);
        rand_bits=(BitSequence*)calloc((unsigned int)bit_Len,sizeof (BitSequence));
        int count=bit_Len/8;
        BytesToBitSequence((unsigned char*)(byteData.data()),count,rand_bits,bit_Len);
    }
    else
    {
        QMessageBox::information(this,"提示","请选择文件！");
        return;
    }
}

//选择文件夹
void MainWindow::on_pushButton_3_clicked()
{
    //记住上次选择的路径
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString();

    QString srcDirPath=QFileDialog::getExistingDirectory(this,"选择文件夹",lastPath);
    if(srcDirPath.isEmpty())
    {
        QMessageBox::information(this,"提示","请选择文件夹！");
        return;
    }
    else
    {
        setting.setValue("LastFilePath", srcDirPath);
        srcDirPath+="/";
        is_multi=true;
    }
    m_byteData.clear();
    m_byteData_Len.clear();
    m_bit_Len.clear();
    m_rand_bits.clear();
    ui->lineEdit->setText(srcDirPath);
    //qDebug()<<srcDirPath;
    QDir dir(srcDirPath);
    if(!dir.exists())
    {
        return;
    }
    QStringList filters;
    filters<<QString("*.bin");
    dir.setFilter(QDir::Files |QDir::NoSymLinks);//设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);
    int dir_count=(int)dir.count();
    if(dir_count<=0)
    {
        return;
    }
    QStringList files;
    for(int i=0;i<dir_count;i++)
    {
        QString file_name=srcDirPath + dir[i];
        f_name.append(dir[i]);
        files.append(file_name);
    }
    file_list=files;
    if(file_list.count()<=0)
    {
        //QMessageBox::information(this,"提示","样本数<1000（依据规范，样本数应不少于1000）！");
        QMessageBox::information(this,"提示","样本数 <= 0！");
        return;
    }
    else
    {
        for(int i=0;i<file_list.count();i++)
        {
            QFile file(file_list[i]);
            bool isOK=file.open(QFile::ReadOnly);
            if(!isOK){
                QMessageBox::critical(this,"错误","打开文件失败!");
                return;
            }
            m_byteData.append(file.readAll());
            m_byteData_Len.append(m_byteData[i].length());
            m_bit_Len.append(m_byteData_Len[i]*8);
            QString len=QString::number(m_byteData_Len[i]);
            //ui->textBrowser->append(len);
            m_rand_bits.append((BitSequence*)calloc((unsigned int)m_bit_Len[i],sizeof (BitSequence)));
            int count=m_bit_Len[i]/8;
            BytesToBitSequence((unsigned char*)(m_byteData[i].data()),count,m_rand_bits[i],m_bit_Len[i]);
        }
    }
}

//开始检测
void MainWindow::on_pushButton_2_clicked()
{
    if(ui->lineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请选择文件或者文件夹！");
        return;
    }
    if(ui->checkBox->isChecked()==false&&ui->checkBox_2->isChecked()==false
            &&ui->checkBox_3->isChecked()==false&&ui->checkBox_4->isChecked()==false
            &&ui->checkBox_5->isChecked()==false&&ui->checkBox_6->isChecked()==false
            &&ui->checkBox_7->isChecked()==false&&ui->checkBox_8->isChecked()==false
            &&ui->checkBox_9->isChecked()==false&&ui->checkBox_10->isChecked()==false
            &&ui->checkBox_11->isChecked()==false&&ui->checkBox_12->isChecked()==false
            &&ui->checkBox_13->isChecked()==false&&ui->checkBox_14->isChecked()==false
            &&ui->checkBox_15->isChecked()==false)
    {
        QMessageBox::information(this,"提示","请选择一项或者这多项检测项！");
        return;
    }

    checkbox_ischeck=ui->checkBox->isChecked();
    checkbox2_ischeck=ui->checkBox_2->isChecked();
    checkbox3_ischeck=ui->checkBox_3->isChecked();
    checkbox4_ischeck=ui->checkBox_4->isChecked();
    checkbox5_ischeck=ui->checkBox_5->isChecked();
    checkbox6_ischeck=ui->checkBox_6->isChecked();
    checkbox7_ischeck=ui->checkBox_7->isChecked();
    checkbox8_ischeck=ui->checkBox_8->isChecked();
    checkbox9_ischeck=ui->checkBox_9->isChecked();
    checkbox10_ischeck=ui->checkBox_10->isChecked();
    checkbox11_ischeck=ui->checkBox_11->isChecked();
    checkbox12_ischeck=ui->checkBox_12->isChecked();
    checkbox13_ischeck=ui->checkBox_13->isChecked();
    checkbox14_ischeck=ui->checkBox_14->isChecked();
    checkbox15_ischeck=ui->checkBox_15->isChecked();

    ui->pushButton_2->setDisabled(true);

    if(file_list.isEmpty()&&is_multi==false)
    {
        //单文件检测
        detect_fun(rand_bits,bit_Len);
    }
    else
    {
        //文件夹检测，多线程
        pthread=new detect_thread();
        pthread_2=new detect_thread_2();
        connect(pthread,SIGNAL(Send2UI(Data)),this,SLOT(show_thread_info(Data)));
        connect(pthread_2,SIGNAL(Send2UI_2(Data_2)),this,SLOT(show_thread_2_info(Data_2)));

        if(pthread->isRunning()||pthread_2->isRunning())
        {
            pthread->quit();
            pthread_2->quit();
        }
        pthread->start(QThread::HighestPriority);
        pthread_2->start(QThread::NormalPriority);

        //防止卡住主线程
        QEventLoop eventLoop_1;
        connect(pthread,SIGNAL(finished()),&eventLoop_1,SLOT(quit()));
        pthread->wait(1);
        eventLoop_1.exec();

        QEventLoop eventLoop_2;
        connect(pthread_2,SIGNAL(finished()),&eventLoop_2,SLOT(quit()));
        pthread_2->wait(1);
        eventLoop_2.exec();

        //QMessageBox::information(this,"提示","滴~，未实现");
        //return;
        //        int count=m_bit_Len.count();
        //        for(int i=0;i<count;i++)
        //        {
        //            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
        //            ui->textBrowser->append("文件名："+f_name[i]);
        //            detect_fun(m_rand_bits[i],m_bit_Len[i]);
        //            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
        //        }
    }
}

//线程1执行结果的处理函数
void MainWindow::show_thread_info(Data data)
{
    ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
    ui->textBrowser->append("文件名："+data.f_Name);
    ui->textBrowser->append(data.t_bit_data);
    if(ui->checkBox->isChecked())
    {
        ui->textBrowser->append(data.t_val);
    }
    if(ui->checkBox_2->isChecked())
    {
        ui->textBrowser->append(data.t_val_2);
    }
    if(ui->checkBox_3->isChecked())
    {
        ui->textBrowser->append(data.t_val_3_4);
        ui->textBrowser->append(data.t_val_3_8);
    }
    if(ui->checkBox_4->isChecked())
    {
        ui->textBrowser->append(data.t_val_4_2_1);
        ui->textBrowser->append(data.t_val_4_2_2);
        ui->textBrowser->append(data.t_val_4_5_1);
        ui->textBrowser->append(data.t_val_4_5_2);
    }
    if(ui->checkBox_5->isChecked())
    {
        ui->textBrowser->append(data.t_val_5);
    }
    if(ui->checkBox_6->isChecked())
    {
        ui->textBrowser->append(data.t_val_6);
    }
    if(ui->checkBox_7->isChecked())
    {
        ui->textBrowser->append(data.t_val_7);
    }
    if(ui->checkBox_8->isChecked())
    {
        ui->textBrowser->append(data.t_val_8_3);
        ui->textBrowser->append(data.t_val_8_7);
    }
    if(ui->checkBox_9->isChecked())
    {
        ui->textBrowser->append(data.t_val_9_1);
        ui->textBrowser->append(data.t_val_9_2);
        ui->textBrowser->append(data.t_val_9_8);
        ui->textBrowser->append(data.t_val_9_16);
    }
    if(ui->checkBox_10->isChecked())
    {
        ui->textBrowser->append(data.t_val_10);
    }
    if(ui->checkBox_11->isChecked())
    {
        ui->textBrowser->append(data.t_val_11_f);
        ui->textBrowser->append(data.t_val_11_b);
    }
    if(ui->checkBox_12->isChecked())
    {
        ui->textBrowser->append(data.t_val_12_2);
        ui->textBrowser->append(data.t_val_12_5);
    }
    if(ui->checkBox_13->isChecked())
    {
        ui->textBrowser->append(data.t_val_13);
    }
    if(ui->checkBox_14->isChecked())
    {
        ui->textBrowser->append(data.t_val_14);
    }
    if(ui->checkBox_15->isChecked())
    {
        ui->textBrowser->append(data.t_val_15);
    }
    ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
    if(data.finish==true)
    {
        t_finish=true;

        t_pass_num=data.pass_num_th1;
        //        qDebug()<<t_pass_num;
        pass_num=data.pass_num;
        pass_num_2=data.pass_num_2;
        pass_num_3_4=data.pass_num_3_4;
        pass_num_3_8=data.pass_num_3_8;
        pass_num_4_2_1=data.pass_num_4_2_1;
        pass_num_4_2_2=data.pass_num_4_2_2;
        pass_num_4_5_1=data.pass_num_4_5_1;
        pass_num_4_5_2=data.pass_num_4_5_2;
        pass_num_5=data.pass_num_5;
        pass_num_6=data.pass_num_6;
        pass_num_7=data.pass_num_7;
        pass_num_8_3=data.pass_num_8_3;
        pass_num_8_7=data.pass_num_8_7;
        pass_num_9_1=data.pass_num_9_1;
        pass_num_9_2=data.pass_num_9_2;
        pass_num_9_8=data.pass_num_9_8;
        pass_num_9_16=data.pass_num_9_16;
        pass_num_10=data.pass_num_10;
        pass_num_11_f=data.pass_num_11_f;
        pass_num_11_b=data.pass_num_11_b;
        pass_num_12_2=data.pass_num_12_2;
        pass_num_12_5=data.pass_num_12_5;
        pass_num_13=data.pass_num_13;
        pass_num_14=data.pass_num_14;
        pass_num_15=data.pass_num_15;

        pthread->quit();
    }
}

//线程2执行结果的处理函数
void MainWindow::show_thread_2_info(Data_2 data_2)
{
    ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
    ui->textBrowser->append("文件名："+data_2.f_Name);
    ui->textBrowser->append(data_2.t_bit_data);
    if(ui->checkBox->isChecked())
    {
        ui->textBrowser->append(data_2.t_val);
    }
    if(ui->checkBox_2->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_2);
    }
    if(ui->checkBox_3->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_3_4);
        ui->textBrowser->append(data_2.t_val_3_8);
    }
    if(ui->checkBox_4->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_4_2_1);
        ui->textBrowser->append(data_2.t_val_4_2_2);
        ui->textBrowser->append(data_2.t_val_4_5_1);
        ui->textBrowser->append(data_2.t_val_4_5_2);
    }
    if(ui->checkBox_5->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_5);
    }
    if(ui->checkBox_6->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_6);
    }
    if(ui->checkBox_7->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_7);
    }
    if(ui->checkBox_8->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_8_3);
        ui->textBrowser->append(data_2.t_val_8_7);
    }
    if(ui->checkBox_9->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_9_1);
        ui->textBrowser->append(data_2.t_val_9_2);
        ui->textBrowser->append(data_2.t_val_9_8);
        ui->textBrowser->append(data_2.t_val_9_16);
    }
    if(ui->checkBox_10->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_10);
    }
    if(ui->checkBox_11->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_11_f);
        ui->textBrowser->append(data_2.t_val_11_b);
    }
    if(ui->checkBox_12->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_12_2);
        ui->textBrowser->append(data_2.t_val_12_5);
    }
    if(ui->checkBox_13->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_13);
    }
    if(ui->checkBox_14->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_14);
    }
    if(ui->checkBox_15->isChecked())
    {
        ui->textBrowser->append(data_2.t_val_15);
    }
    ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
    if(data_2.finish_2==true)
    {
        t_finish_2=true;
        //qDebug()<<t_finish;
        if(t_finish==true)
        {
            //
            double tmp=(0.01*(1-0.01))/m_bit_Len.count();
            //int pass_std=(int)(m_bit_Len.count()*(1-0.01-3*sqrt(tmp))+0.5);
            double pass_std = m_bit_Len.count()*(1-0.01-3*sqrt(tmp));
            double pass_t = floor((pass_std * pow(10 ,3) + 0.5)) / pow(10, 3);//保留三位小数
            QString pass_std_rate_per = torate_per(pass_t,m_bit_Len.count());
            is_pass = true;

            ui->textBrowser->append("随机性检测的统计结果如下：（阈值下限：" + QString::number(pass_t) +"，阈值下限百分比：" + pass_std_rate_per + "）");
            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
            ui->textBrowser->append("测试项                                  通过的样本数 （概率）                            单项测试结果");
            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");

            //ui->textBrowser->append("\r\n单项检测结果如下：");
            if(checkbox_ischeck==true){
                ui->textBrowser->append("01-单比特频数检测                "+QString::number(pass_num+data_2.pass_num)
                                        +" ("+torate_per(pass_num+data_2.pass_num,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num+data_2.pass_num,pass_t));
                QString res = single_stat(pass_num+data_2.pass_num,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox2_ischeck==true){
                ui->textBrowser->append("02-块内频数检测                   "+QString::number(pass_num_2+data_2.pass_num_2)
                                        +" ("+torate_per(pass_num_2+data_2.pass_num_2,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_2+data_2.pass_num_2,pass_t));
                QString res = single_stat(pass_num_2+data_2.pass_num_2,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox3_ischeck==true){
                ui->textBrowser->append("03-扑克检测 (4)                     "+QString::number(pass_num_3_4+data_2.pass_num_3_4)
                                        +" ("+torate_per(pass_num_3_4+data_2.pass_num_3_4,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_3_4+data_2.pass_num_3_4,pass_t));
                ui->textBrowser->append("03-扑克检测 (8)                     "+QString::number(pass_num_3_8+data_2.pass_num_3_8)
                                        +" ("+torate_per(pass_num_3_8+data_2.pass_num_3_8,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_3_8+data_2.pass_num_3_8,pass_t));
                QString res_4 = single_stat(pass_num_3_4+data_2.pass_num_3_4,pass_t);
                if(res_4.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_8 = single_stat(pass_num_3_8+data_2.pass_num_3_8,pass_t);
                if(res_8.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox4_ischeck==true){
                ui->textBrowser->append("04-重叠子序列检测 (2_1)        "+QString::number(pass_num_4_2_1+data_2.pass_num_4_2_1)
                                        +" ("+torate_per(pass_num_4_2_1+data_2.pass_num_4_2_1,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_4_2_1+data_2.pass_num_4_2_1,pass_t));
                ui->textBrowser->append("04-重叠子序列检测 (2_2)        "+QString::number(pass_num_4_2_2+data_2.pass_num_4_2_2)
                                        +" ("+torate_per(pass_num_4_2_2+data_2.pass_num_4_2_2,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_4_2_2+data_2.pass_num_4_2_2,pass_t));
                ui->textBrowser->append("04-重叠子序列检测 (5_1)        "+QString::number(pass_num_4_5_1+data_2.pass_num_4_5_1)
                                        +" ("+torate_per(pass_num_4_5_1+data_2.pass_num_4_5_1,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_4_5_1+data_2.pass_num_4_5_1,pass_t));
                ui->textBrowser->append("04-重叠子序列检测 (5_2)        "+QString::number(pass_num_4_5_2+data_2.pass_num_4_5_2)
                                        +" ("+torate_per(pass_num_4_5_2+data_2.pass_num_4_5_2,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_4_5_2+data_2.pass_num_4_5_2,pass_t));
                QString res_2_1 = single_stat(pass_num_4_2_1+data_2.pass_num_4_2_1,pass_t);
                if(res_2_1.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_2_2 = single_stat(pass_num_4_2_2+data_2.pass_num_4_2_2,pass_t);
                if(res_2_2.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_5_1 = single_stat(pass_num_4_5_1+data_2.pass_num_4_5_1,pass_t);
                if(res_5_1.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_5_2 = single_stat(pass_num_4_5_2+data_2.pass_num_4_5_2,pass_t);
                if(res_5_2.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox5_ischeck==true){
                ui->textBrowser->append("05-游程总数检测                   "+QString::number(pass_num_5+data_2.pass_num_5)
                                        +" ("+torate_per(pass_num_5+data_2.pass_num_5,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_5+data_2.pass_num_5,pass_t));
                QString res = single_stat(pass_num_5+data_2.pass_num_5,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox6_ischeck==true){
                ui->textBrowser->append("06-游程分布检测                   "+QString::number(pass_num_6+data_2.pass_num_6)
                                        +" ("+torate_per(pass_num_6+data_2.pass_num_6,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_6+data_2.pass_num_6,pass_t));
                QString res = single_stat(pass_num_6+data_2.pass_num_6,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox7_ischeck==true){
                ui->textBrowser->append("07-块内最大'1'游程检测         "+QString::number(pass_num_7+data_2.pass_num_7)
                                        +" ("+torate_per(pass_num_7+data_2.pass_num_7,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_7+data_2.pass_num_7,pass_t));
                QString res = single_stat(pass_num_7+data_2.pass_num_7,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox8_ischeck==true){
                ui->textBrowser->append("08-二元推导检测 (3)              "+QString::number(pass_num_8_3+data_2.pass_num_8_3)
                                        +" ("+torate_per(pass_num_8_3+data_2.pass_num_8_3,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_8_3+data_2.pass_num_8_3,pass_t));
                ui->textBrowser->append("08-二元推导检测 (7)              "+QString::number(pass_num_8_7+data_2.pass_num_8_7)
                                        +" ("+torate_per(pass_num_8_7+data_2.pass_num_8_7,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_8_7+data_2.pass_num_8_7,pass_t));
                QString res_3 = single_stat(pass_num_8_3+data_2.pass_num_8_3,pass_t);
                if(res_3.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_7 = single_stat(pass_num_8_7+data_2.pass_num_8_7,pass_t);
                if(res_7.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox9_ischeck==true){
                ui->textBrowser->append("09-自相关检测 (1)                 "+QString::number(pass_num_9_1+data_2.pass_num_9_1)
                                        +" ("+torate_per(pass_num_9_1+data_2.pass_num_9_1,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_9_1+data_2.pass_num_9_1,pass_t));
                ui->textBrowser->append("09-自相关检测 (2)                 "+QString::number(pass_num_9_2+data_2.pass_num_9_2)
                                        +" ("+torate_per(pass_num_9_2+data_2.pass_num_9_2,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_9_2+data_2.pass_num_9_2,pass_t));
                ui->textBrowser->append("09-自相关检测 (8)                 "+QString::number(pass_num_9_8+data_2.pass_num_9_8)
                                        +" ("+torate_per(pass_num_9_8+data_2.pass_num_9_8,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_9_8+data_2.pass_num_9_8,pass_t));
                ui->textBrowser->append("09-自相关检测 (16)               "+QString::number(pass_num_9_16+data_2.pass_num_9_16)
                                        +" ("+torate_per(pass_num_9_16+data_2.pass_num_9_16,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_9_16+data_2.pass_num_9_16,pass_t));
                QString res_1 = single_stat(pass_num_9_1+data_2.pass_num_9_1,pass_t);
                if(res_1.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_2 = single_stat(pass_num_9_2+data_2.pass_num_9_2,pass_t);
                if(res_2.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_8 = single_stat(pass_num_9_8+data_2.pass_num_9_8,pass_t);
                if(res_8.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_16 = single_stat(pass_num_9_16+data_2.pass_num_9_16,pass_t);
                if(res_16.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox10_ischeck==true){
                ui->textBrowser->append("10-矩阵秩检测                      "+QString::number(pass_num_10+data_2.pass_num_10)
                                        +" ("+torate_per(pass_num_10+data_2.pass_num_10,m_bit_Len.count())
                                        +")                                   "+single_stat(pass_num_10+data_2.pass_num_10,pass_t));
                QString res = single_stat(pass_num_10+data_2.pass_num_10,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox11_ischeck==true){
                ui->textBrowser->append("11-累加和检测 (forward)       "+QString::number(pass_num_11_f+data_2.pass_num_11_f)
                                        +" ("+torate_per(pass_num_11_f+data_2.pass_num_11_f,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_11_f+data_2.pass_num_11_f,pass_t));
                ui->textBrowser->append("11-累加和检测 (backward)    "+QString::number(pass_num_11_b+data_2.pass_num_11_b)
                                        +" ("+torate_per(pass_num_11_b+data_2.pass_num_11_b,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_11_b+data_2.pass_num_11_b,pass_t));
                QString res_f = single_stat(pass_num_11_f+data_2.pass_num_11_f,pass_t);
                if(res_f.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_b = single_stat(pass_num_11_b+data_2.pass_num_11_b,pass_t);
                if(res_b.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox12_ischeck==true){
                ui->textBrowser->append("12-近似熵检测 (2)                 "+QString::number(pass_num_12_2+data_2.pass_num_12_2)
                                        +" ("+torate_per(pass_num_12_2+data_2.pass_num_12_2,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_12_2+data_2.pass_num_12_2,pass_t));
                ui->textBrowser->append("12-近似熵检测 (5)                 "+QString::number(pass_num_12_5+data_2.pass_num_12_5)
                                        +" ("+torate_per(pass_num_12_5+data_2.pass_num_12_5,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_12_5+data_2.pass_num_12_5,pass_t));
                QString res_2 = single_stat(pass_num_12_2+data_2.pass_num_12_2,pass_t);
                if(res_2.compare("不通过") == 0)
                {
                    is_pass = false;
                }
                QString res_5 = single_stat(pass_num_12_5+data_2.pass_num_12_5,pass_t);
                if(res_5.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox13_ischeck==true){
                ui->textBrowser->append("13-线性复杂度检测               "+QString::number(pass_num_13+data_2.pass_num_13)
                                        +" ("+torate_per(pass_num_13+data_2.pass_num_13,m_bit_Len.count())
                                        +")                                    "+single_stat(pass_num_13+data_2.pass_num_13,pass_t));
                QString res = single_stat(pass_num_13+data_2.pass_num_13,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox14_ischeck==true){
                ui->textBrowser->append("14-Maurer通用统计检测       "+QString::number(pass_num_14+data_2.pass_num_14)
                                        +" ("+torate_per(pass_num_14+data_2.pass_num_14,m_bit_Len.count())
                                        +")                                     "+single_stat(pass_num_14+data_2.pass_num_14,pass_t));
                QString res = single_stat(pass_num_14+data_2.pass_num_14,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            if(checkbox15_ischeck==true){
                ui->textBrowser->append("15-离散傅立叶检测               "+QString::number(pass_num_15+data_2.pass_num_15)
                                        +" ("+torate_per(pass_num_15+data_2.pass_num_15,m_bit_Len.count())
                                        +")                                     "+single_stat(pass_num_15+data_2.pass_num_15,pass_t));
                QString res = single_stat(pass_num_15+data_2.pass_num_15,pass_t);
                if(res.compare("不通过") == 0)
                {
                    is_pass = false;
                }
            }
            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");
            if(is_pass == false)
            {
                ui->textBrowser->append("检测结论：不通过");
            }
            else
            {
                ui->textBrowser->append("检测结论：通过");
            }
            ui->textBrowser->append("----------------------------------------------------------------------------------------------------");

            pthread_2->quit();
        }
    }
}

//保存检测日志
void MainWindow::on_pushButton_4_clicked()
{
    //
    if(ui->textBrowser->toPlainText()=="")
    {
        QMessageBox::information(this,"提示","无检测内容，请先检测！");
        return;
    }
    QFileDialog fileDialog;
    QString filename=fileDialog.getSaveFileName(this,QString("保存文件"),QString("/"),QString("文本文件(*.txt)"));
    if(filename.isEmpty())
    {
        QMessageBox::information(this,"提示","请选择保存的文件名！");
        return;
    }
    QFile s_file;
    s_file.setFileName(filename);
    if(!s_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"错误","无法打开文件！");
        return;
    }
    else
    {
        QTextStream textStream(&s_file);
        textStream.setCodec("UTF-8");
        QString text=ui->textBrowser->toPlainText();
        //qDebug()<<text;
        textStream<<text;//写入文本流
        s_file.close();
        QMessageBox::information(this,"提示","已成功保存文件！");
    }
}

//清空检测结果
void MainWindow::on_pushButton_5_clicked()
{
    ui->textBrowser->clear();
    ui->pushButton_2->setEnabled(true);
    t_pass_num=0;
    t_pass_num_2=0;
    flag=false;

    //    pthread->exit();
    //    pthread_2->exit();
    if(pthread != nullptr)
        pthread->quit();
    if(pthread_2 != nullptr)
        pthread_2->quit();
}



//转百分比
QString MainWindow::torate_per(double m, int n)
{
    double rate= (m / (double)n)*100.0;
    QString rate_str = QString::number(rate,'f', 6);
    QString rate_per=rate_str+"%";
    return rate_per;
}

//单项检测状态
QString MainWindow::single_stat(int m, double n)
{
    double pass_num = (double) m;
    const double EPS = 1e-6;
    double sub = pass_num - n;
    if((pass_num > n + EPS) || (fabs(sub) < EPS)){
        return "通过";
    }
    else{
        return "不通过";
    }
}

//检测函数
void MainWindow::detect_fun(BitSequence* bits,int len)
{
    //统计0和1的个数
    int num_0=0;
    int num_1=0;
    for(int i=0;i<len;i++)
    {
        if(bits[i]==0){
            num_0++;
        }
        else{
            num_1++;
        }
    }
    QString str_bit_len=QString::number(len);
    QString str_num_0=QString::number(num_0);
    QString str_num_1=QString::number(num_1);
    QString bit_data;
    bit_data="Count Of Bits: "+str_bit_len;
    bit_data+=", 0_num: ";
    bit_data+=str_num_0;
    bit_data+=", 1_num: ";
    bit_data+=str_num_1;
    if(flag==false ||is_multi==true)
    {
        ui->textBrowser->append(bit_data);
        flag=true;
    }

    //单比特频数检测
    if(ui->checkBox->isChecked()==true)
    {
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=Frequency(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(Frequency_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Frequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Frequency Test";
            ui->textBrowser->append(val);
            //QMessageBox::information(nullptr,"提示","Frequency failed");
        }
        else
        {
            QString p_val=QString::number(Frequency_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Frequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Frequency Test";
            ui->textBrowser->append(val);
            //QMessageBox::information(nullptr,"提示","Frequency pass");
        }
        free(bits_cp);
    }

    //块内频数检测
    if(ui->checkBox_2->isChecked()==true)
    {
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=BlockFrequency(100,len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(BlockFrequency_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BlockFrequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --BlockFrequency100 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(BlockFrequency_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BlockFrequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --BlockFrequency100 Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //扑克检测
    if(ui->checkBox_3->isChecked()==true)
    {
        BitSequence* bits_cp_4=new BitSequence[(unsigned int)len];
        BitSequence* bits_cp_8=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_4,bits,(unsigned int)len*sizeof(BitSequence));
        memcpy(bits_cp_8,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_4=0,end_4=0,begin_8=0,end_8=0;
        int ret_4,ret_8;
        begin_4=clock();
        ret_4=PokerDetect(4,len,bits_cp_4);//m=4
        end_4=clock();
        double time_Costs_4=end_4-begin_4;
        QString time_C_4=QString::number(time_Costs_4,10,3);
        if(ret_4==0)
        {
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_4;
            val+="ms";
            val+=" --Poker4 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_4;
            val+="ms";
            val+=" --Poker4 Test";
            ui->textBrowser->append(val);
        }
        begin_8=clock();
        ret_8=PokerDetect(8,len,bits_cp_8);//m=8
        end_8=clock();
        double time_Costs_8=end_8-begin_8;
        QString time_C_8=QString::number(time_Costs_8,10,3);
        if(ret_8==0)
        {
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --Poker8 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --Poker8 Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp_4);
        free(bits_cp_8);
    }

    //重叠子序列检测
    if(ui->checkBox_4->isChecked()==true)
    {
        //
        BitSequence* bits_cp_2=new BitSequence[(unsigned int)len];
        BitSequence* bits_cp_5=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_2,bits,(unsigned int)len*sizeof(BitSequence));
        memcpy(bits_cp_5,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_2=0,end_2=0,begin_5=0,end_5=0;
        int ret_2,ret_5;
        begin_2=clock();
        ret_2=Serial(2,len,bits_cp_2);//m=2
        end_2=clock();
        double time_Costs_2=end_2-begin_2;
        QString time_C_2=QString::number(time_Costs_2,10,3);
        if(ret_2==0)
        {
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="FAIL  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_2;
            val_1+="ms";
            val_1+=" --Serial2_1 Test";
            ui->textBrowser->append(val_1);

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="FAIL  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_2;
            val_2+="ms";
            val_2+=" --Serial2_2 Test";
            ui->textBrowser->append(val_2);
        }
        else
        {
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="PASS  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_2;
            val_1+="ms";
            val_1+=" --Serial2_1 Test";
            ui->textBrowser->append(val_1);

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="PASS  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_2;
            val_2+="ms";
            val_2+=" --Serial2_2 Test";
            ui->textBrowser->append(val_2);
        }
        begin_5=clock();
        ret_5=Serial(5,len,bits_cp_5);//m=5
        end_5=clock();
        double time_Costs_5=end_5-begin_5;
        QString time_C_5=QString::number(time_Costs_5,10,3);
        if(ret_5==0)
        {
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="FAIL  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_5;
            val_1+="ms";
            val_1+=" --Serial5_1 Test";
            ui->textBrowser->append(val_1);

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="FAIL  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_5;
            val_2+="ms";
            val_2+=" --Serial5_2 Test";
            ui->textBrowser->append(val_2);
        }
        else
        {
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="PASS  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_5;
            val_1+="ms";
            val_1+=" --Serial5_1 Test";
            ui->textBrowser->append(val_1);

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="PASS  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_5;
            val_2+="ms";
            val_2+=" --Serial5_2 Test";
            ui->textBrowser->append(val_2);
        }
        free(bits_cp_2);
        free(bits_cp_5);
    }

    //游程总数检测
    if(ui->checkBox_5->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=Runs(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(Runs_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Runs_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Runs Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(Runs_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Runs_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Runs Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //游程分布检测
    if(ui->checkBox_6->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=RunsDistribution(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(RunsDistribution_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(RunsDistribution_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --RunsDistribution Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(RunsDistribution_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(RunsDistribution_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --RunsDistribution Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //块内最大"1"游程检测
    if(ui->checkBox_7->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=LongestRunOfOnes(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(LongestRunOfOnes_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(LongestRunOfOnes_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LongestRunOfOnes Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(LongestRunOfOnes_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(LongestRunOfOnes_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LongestRunOfOnes Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //二元推导检测
    if(ui->checkBox_8->isChecked()==true)
    {
        //
        BitSequence* bits_cp_3=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_3,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_7=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_7,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_3=0,end_3=0,begin_7=0,end_7=0;
        int ret_3,ret_7;
        begin_3=clock();
        ret_3=BinaryDerivate(3,len,bits_cp_3);//k=3
        end_3=clock();
        double time_Costs_3=end_3-begin_3;
        QString time_C_3=QString::number(time_Costs_3,10,3);
        if(ret_3==0)
        {
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_3;
            val+="ms";
            val+=" --BinaryDerivate3 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_3;
            val+="ms";
            val+=" --BinaryDerivate3 Test";
            ui->textBrowser->append(val);
        }
        begin_7=clock();
        ret_7=BinaryDerivate(7,len,bits_cp_7);//k=7
        end_7=clock();
        double time_Costs_7=end_7-begin_7;
        QString time_C_7=QString::number(time_Costs_7,10,3);
        if(ret_7==0)
        {
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_7;
            val+="ms";
            val+=" --BinaryDerivate7 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_7;
            val+="ms";
            val+=" --BinaryDerivate7 Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp_3);
        free(bits_cp_7);
    }

    //自相关检测
    if(ui->checkBox_9->isChecked()==true)
    {
        //
        BitSequence* bits_cp_1=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_1,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_2=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_2,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_8=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_8,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_16=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_16,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_1=0,end_1=0,begin_2=0,end_2=0,begin_8=0,end_8=0,begin_16,end_16;
        int ret_1,ret_2,ret_8,ret_16;

        begin_1=clock();
        ret_1=SelfCorrelation(1,len,bits_cp_1);//d=1
        end_1=clock();
        double time_Costs_1=end_1-begin_1;
        QString time_C_1=QString::number(time_Costs_1,10,3);
        if(ret_1==0)
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_1;
            val+="ms";
            val+=" --SelfCorrelation1 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_1;
            val+="ms";
            val+=" --SelfCorrelation1 Test";
            ui->textBrowser->append(val);
        }

        begin_2=clock();
        ret_2=SelfCorrelation(2,len,bits_cp_2);//d=2
        end_2=clock();
        double time_Costs_2=end_2-begin_2;
        QString time_C_2=QString::number(time_Costs_2,10,3);
        if(ret_2==0)
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --SelfCorrelation2 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --SelfCorrelation2 Test";
            ui->textBrowser->append(val);
        }

        begin_8=clock();
        ret_8=SelfCorrelation(8,len,bits_cp_8);//d=8
        end_8=clock();
        double time_Costs_8=end_8-begin_8;
        QString time_C_8=QString::number(time_Costs_8,10,3);
        if(ret_8==0)
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --SelfCorrelation8 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --SelfCorrelation8 Test";
            ui->textBrowser->append(val);
        }

        begin_16=clock();
        ret_16=SelfCorrelation(16,len,bits_cp_16);//d=16
        end_16=clock();
        double time_Costs_16=end_16-begin_16;
        QString time_C_16=QString::number(time_Costs_16,10,3);
        if(ret_16==0)
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_16;
            val+="ms";
            val+=" --SelfCorrelation16 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_16;
            val+="ms";
            val+=" --SelfCorrelation16 Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp_1);
        free(bits_cp_2);
        free(bits_cp_8);
        free(bits_cp_16);
    }

    //矩阵秩检测
    if(ui->checkBox_10->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=Rank(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(Rank_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Rank_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Rank Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(Rank_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Rank_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Rank Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //累加和检测
    if(ui->checkBox_11->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=CumulativeSums(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val_f=QString::number(CumulativeSums_p_Value_forward,'f',6);
            p_val_f="FAIL  p_value = "+p_val_f;
            //QString v_val=QString::number(CumulativeSums_v_Value);
            //v_val="  统计值 = "+v_val;
            //QString val=p_val+v_val;
            QString val=p_val_f;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --CumulativeSums（forward） Test";
            ui->textBrowser->append(val);

            QString p_val_b=QString::number(CumulativeSums_p_Value_backward,'f',6);
            p_val_b="FAIL  p_value = "+p_val_b;
            //QString v_val=QString::number(CumulativeSums_v_Value);
            //v_val="  统计值 = "+v_val;
            //QString val=p_val+v_val;
            val=p_val_b;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --CumulativeSums（backward） Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val_f=QString::number(CumulativeSums_p_Value_forward,'f',6);
            p_val_f="PASS  p_value = "+p_val_f;
            //QString v_val=QString::number(CumulativeSums_v_Value);
            //v_val="  统计值 = "+v_val;
            //QString val=p_val+v_val;
            QString val=p_val_f;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --CumulativeSums（forward） Test";
            ui->textBrowser->append(val);

            QString p_val_b=QString::number(CumulativeSums_p_Value_backward,'f',6);
            p_val_b="PASS  p_value = "+p_val_b;
            //QString v_val=QString::number(CumulativeSums_v_Value);
            //v_val="  统计值 = "+v_val;
            //QString val=p_val+v_val;
            val=p_val_b;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --CumulativeSums（backward） Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //近似熵检测
    if(ui->checkBox_12->isChecked()==true)
    {
        //
        BitSequence* bits_cp_2=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_2,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_5=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_5,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_2=0,end_2=0,begin_5=0,end_5=0;
        int ret_2,ret_5;
        begin_2=clock();
        ret_2=ApproximateEntropy(2,len,bits_cp_2);//2
        end_2=clock();
        double time_Costs_2=end_2-begin_2;
        QString time_C_2=QString::number(time_Costs_2,10,3);
        if(ret_2==0)
        {
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --ApproximateEntropy2 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --ApproximateEntropy2 Test";
            ui->textBrowser->append(val);
        }
        begin_5=clock();
        ret_5=ApproximateEntropy(5,len,bits_cp_5);//5
        end_5=clock();
        double time_Costs_5=end_5-begin_5;
        QString time_C_5=QString::number(time_Costs_5,10,3);
        if(ret_5==0)
        {
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_5;
            val+="ms";
            val+=" --ApproximateEntropy5 Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_5;
            val+="ms";
            val+=" --ApproximateEntropy5 Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp_2);
        free(bits_cp_5);
    }

    //线性复杂度检测
    if(ui->checkBox_13->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=LinearComplexity(500,len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(LinearComplexity_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(LinearComplexity_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LinearComplexity Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(LinearComplexity_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(LinearComplexity_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LinearComplexity Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //通用统计检测
    if(ui->checkBox_14->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=Universal(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(Universal_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Universal_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Universal Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(Universal_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Universal_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Universal Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }

    //离散傅立叶检测
    if(ui->checkBox_15->isChecked()==true)
    {
        //
        BitSequence* bits_cp=new BitSequence[(unsigned int)len];
        memcpy(bits_cp,bits,(unsigned int)len*sizeof(BitSequence));

        double begin=0,end=0;
        int ret;
        begin=clock();
        ret=DiscreteFourierTransform(len,bits_cp);
        end=clock();
        double time_Costs=end-begin;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret==0)
        {
            QString p_val=QString::number(DiscreteFourierTransform_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(DiscreteFourierTransform_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --DiscreteFourierTransform Test";
            ui->textBrowser->append(val);
        }
        else
        {
            QString p_val=QString::number(DiscreteFourierTransform_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(DiscreteFourierTransform_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --DiscreteFourierTransform Test";
            ui->textBrowser->append(val);
        }
        free(bits_cp);
    }
}

//关闭事件，释放内存空间
void MainWindow::closeEvent(QCloseEvent *event)
{
    free(rand_bits);
    event->accept();
}

//全选
void MainWindow::on_checkBox_16_clicked()
{
    //
    ui->checkBox_17->setCheckState(Qt::Unchecked);
    if(ui->checkBox_16->isChecked()&&!ui->checkBox_17->isChecked())
    {
        ui->checkBox->setCheckState(Qt::Checked);
        ui->checkBox_2->setCheckState(Qt::Checked);
        ui->checkBox_3->setCheckState(Qt::Checked);
        ui->checkBox_4->setCheckState(Qt::Checked);
        ui->checkBox_5->setCheckState(Qt::Checked);
        ui->checkBox_6->setCheckState(Qt::Checked);
        ui->checkBox_7->setCheckState(Qt::Checked);
        ui->checkBox_8->setCheckState(Qt::Checked);
        ui->checkBox_9->setCheckState(Qt::Checked);
        ui->checkBox_10->setCheckState(Qt::Checked);
        ui->checkBox_11->setCheckState(Qt::Checked);
        ui->checkBox_12->setCheckState(Qt::Checked);
        ui->checkBox_13->setCheckState(Qt::Checked);
        ui->checkBox_14->setCheckState(Qt::Checked);
        ui->checkBox_15->setCheckState(Qt::Checked);
    }
}

//全不选
void MainWindow::on_checkBox_17_clicked()
{
    //
    ui->checkBox_16->setCheckState(Qt::Unchecked);
    if(!ui->checkBox_16->isChecked()&&ui->checkBox_17->isChecked())
    {
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_3->setCheckState(Qt::Unchecked);
        ui->checkBox_4->setCheckState(Qt::Unchecked);
        ui->checkBox_5->setCheckState(Qt::Unchecked);
        ui->checkBox_6->setCheckState(Qt::Unchecked);
        ui->checkBox_7->setCheckState(Qt::Unchecked);
        ui->checkBox_8->setCheckState(Qt::Unchecked);
        ui->checkBox_9->setCheckState(Qt::Unchecked);
        ui->checkBox_10->setCheckState(Qt::Unchecked);
        ui->checkBox_11->setCheckState(Qt::Unchecked);
        ui->checkBox_12->setCheckState(Qt::Unchecked);
        ui->checkBox_13->setCheckState(Qt::Unchecked);
        ui->checkBox_14->setCheckState(Qt::Unchecked);
        ui->checkBox_15->setCheckState(Qt::Unchecked);
    }
}
