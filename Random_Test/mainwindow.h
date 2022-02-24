#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <math.h>
#include "include/stat_fncs.h"
#include <QVector>
#include <QCloseEvent>
#include <QEventLoop>
#include <QSettings>
#include "detect_thread.h"
#include "detect_thread_2.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static QByteArray byteData;
    static int byteData_Len;
    static int bit_Len;
    static BitSequence* rand_bits;
    static QVector<QByteArray>m_byteData;
    static QVector<int>m_byteData_Len;
    static QVector<int>m_bit_Len;
    static QVector<BitSequence*>m_rand_bits;
    static bool flag;
    static bool is_multi;
    static QStringList file_list;
    static QStringList f_name;
    void detect_fun(BitSequence* bits,int len);
    QString torate_per(double m,int n);
    QString single_stat(int m,double n);
    //int detect_fun_thread(BitSequence* bits,int len);

    static detect_thread *pthread;
    static detect_thread_2 *pthread_2;

    static int t_pass_num;
    static int t_pass_num_2;

    static bool t_finish;
    static bool t_finish_2;

    static bool is_pass;

    //统计线程1单项通过数
    static int pass_num;//单比特频数

    static int pass_num_2;//块内频数

    static int pass_num_3_4;//扑克
    static int pass_num_3_8;

    static int pass_num_4_2_1;//重叠子序列
    static int pass_num_4_2_2;
    static int pass_num_4_5_1;
    static int pass_num_4_5_2;

    static int pass_num_5;//游程总数

    static int pass_num_6;//游程分布

    static int pass_num_7;//块内最大"1"游程

    static int pass_num_8_3;//二元推导
    static int pass_num_8_7;

    static int pass_num_9_1;//自相关
    static int pass_num_9_2;
    static int pass_num_9_8;
    static int pass_num_9_16;

    static int pass_num_10;//矩阵秩

    static int pass_num_11_f;//累加和
    static int pass_num_11_b;

    static int pass_num_12_2;//近似熵
    static int pass_num_12_5;

    static int pass_num_13;//线性复杂度

    static int pass_num_14;//通用

    static int pass_num_15;//离散傅立叶

protected:
    void closeEvent(QCloseEvent*event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_checkBox_16_clicked();

    void on_checkBox_17_clicked();

    void show_thread_info(Data data);

    void show_thread_2_info(Data_2 data_2);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
