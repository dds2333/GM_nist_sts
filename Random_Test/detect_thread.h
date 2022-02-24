#ifndef DETECT_THREAD_H
#define DETECT_THREAD_H
#include <QThread>
#include <string>
#include <QString>
#include "global.h"
#include"include/stat_fncs.h"
#include <QMetaType>
#include <QObject>

//自定义消息结构
typedef struct
{
    QString t_bit_data;
    QString t_val;//单比特频数
    QString t_val_2;//块内频数

    QString t_val_3_4;//扑克
    QString t_val_3_8;

    QString t_val_4_2_1;//重叠子序列
    QString t_val_4_2_2;
    QString t_val_4_5_1;
    QString t_val_4_5_2;

    QString t_val_5;//游程总数
    QString t_val_6;//游程分布
    QString t_val_7;//块内最大"1"游程

    QString t_val_8_3;//二元推导
    QString t_val_8_7;

    QString t_val_9_1;//自相关
    QString t_val_9_2;
    QString t_val_9_8;
    QString t_val_9_16;

    QString t_val_10;//矩阵秩

    QString t_val_11_f;//累加和
    QString t_val_11_b;

    QString t_val_12_2;//近似熵
    QString t_val_12_5;

    QString t_val_13;//线性复杂度
    QString t_val_14;//通用
    QString t_val_15;//离散傅立叶

    QString f_Name;
    bool finish=false;
    int ret;

    int pass_num_th1=0;

    //统计多文件检测时单项通过的个数
    int pass_num=0;//单比特频数

    int pass_num_2=0;//块内频数

    int pass_num_3_4=0;//扑克
    int pass_num_3_8=0;

    int pass_num_4_2_1=0;//重叠子序列
    int pass_num_4_2_2=0;
    int pass_num_4_5_1=0;
    int pass_num_4_5_2=0;

    int pass_num_5=0;//游程总数

    int pass_num_6=0;//游程分布

    int pass_num_7=0;//块内最大"1"游程

    int pass_num_8_3=0;//二元推导
    int pass_num_8_7=0;

    int pass_num_9_1=0;//自相关
    int pass_num_9_2=0;
    int pass_num_9_8=0;
    int pass_num_9_16=0;

    int pass_num_10=0;//矩阵秩

    int pass_num_11_f=0;//累加和
    int pass_num_11_b=0;

    int pass_num_12_2=0;//近似熵
    int pass_num_12_5=0;

    int pass_num_13=0;//线性复杂度

    int pass_num_14=0;//通用

    int pass_num_15=0;//离散傅立叶
}Data;
Q_DECLARE_METATYPE(Data);

class detect_thread : public QThread
{
    Q_OBJECT

public:
    detect_thread();
    ~detect_thread();

    void run();
    int detect_fun_thread(BitSequence* bits,int len);

private:
    Data data;
signals:
    void Send2UI(Data data);
};

#endif // DETECT_THREAD_H
