#pragma execution_character_set("utf-8")
#include "detect_thread_2.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "QDebug"

detect_thread_2::detect_thread_2()
{

}
detect_thread_2::~detect_thread_2()
{
    requestInterruption();
    quit();
    wait();
}

void detect_thread_2::run()
{
    //

    for(int i=MainWindow::m_bit_Len.count()/2;i<MainWindow::m_bit_Len.count();i++)
    {
        if(QThread::currentThread()-> isInterruptionRequested())
        {
            break;
        }
        //qDebug()<<QThread::currentThread();
        data_2.ret=detect_fun_thread_2(MainWindow::m_rand_bits[i],MainWindow::m_bit_Len[i]);
        data_2.f_Name=MainWindow::f_name[i];

        if(i==MainWindow::m_bit_Len.count()-1)
        {
            data_2.finish_2=true;
            msleep(1000);//当计算到最后一个文件时休眠一秒，等待线程1完成
        }
        emit Send2UI_2(data_2);
    }   
}

//检测函数
int detect_thread_2::detect_fun_thread_2(BitSequence* bits,int len)
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
    if(MainWindow::flag==false ||MainWindow::is_multi==true)
    {
        //ui->textBrowser->append(bit_data);
        data_2.t_bit_data=bit_data;
        MainWindow::flag=true;
    }
    bool result=false,result_2=false,result_3_4=false,result_3_8=false,result_4_2=false,result_4_5=false,result_5=false,result_6=false,
            result_7=false,result_8_3=false,result_8_7=false,result_9_1=false,result_9_2=false,result_9_8=false,result_9_16=false,result_10=false,
            result_11=false,result_12_2=false,result_12_5=false,result_13=false,result_14=false,result_15=false;   

    //单比特频数检测
    if(checkbox_ischeck==true)
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
            result=false;
            const double EPS = 1e-6;
            double sub = Frequency_p_Value - ALPHA;
            if((Frequency_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num++;
            }
            QString p_val=QString::number(Frequency_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Frequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Frequency Test";
            //ui->textBrowser->append(val);
            data_2.t_val=val;
            //QMessageBox::information(nullptr,"提示","Frequency failed");
        }
        else
        {
            result=true;
            data_2.pass_num++;
            QString p_val=QString::number(Frequency_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Frequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Frequency Test";
            //ui->textBrowser->append(val);
            data_2.t_val=val;
            //QMessageBox::information(nullptr,"提示","Frequency pass");
        }
        free(bits_cp);
    }

    //块内频数检测
    if(checkbox2_ischeck==true)
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
            result_2=false;
            const double EPS = 1e-6;
            double sub = BlockFrequency_p_Value - ALPHA;
            if((BlockFrequency_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_2++;
            }
            QString p_val=QString::number(BlockFrequency_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BlockFrequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --BlockFrequency100 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_2=val;
        }
        else
        {
            result_2=true;
            data_2.pass_num_2++;
            QString p_val=QString::number(BlockFrequency_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BlockFrequency_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --BlockFrequency100 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_2=val;
        }
        free(bits_cp);
    }

    //扑克检测
    if(checkbox3_ischeck==true)
    {
        BitSequence* bits_cp_4=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_4,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_8=new BitSequence[(unsigned int)len];
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
            result_3_4=false;
            const double EPS = 1e-6;
            double sub = PokerDetect_p_Value - ALPHA;
            if((PokerDetect_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_3_4++;
            }
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_4;
            val+="ms";
            val+=" --Poker4 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_3_4=val;
        }
        else
        {
            result_3_4=true;
            data_2.pass_num_3_4++;
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_4;
            val+="ms";
            val+=" --Poker4 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_3_4=val;
        }
        begin_8=clock();
        ret_8=PokerDetect(8,len,bits_cp_8);//m=8
        end_8=clock();
        double time_Costs_8=end_8-begin_8;
        QString time_C_8=QString::number(time_Costs_8,10,3);
        if(ret_8==0)
        {
            result_3_8=false;
            const double EPS = 1e-6;
            double sub = PokerDetect_p_Value - ALPHA;
            if((PokerDetect_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_3_8++;
            }
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --Poker8 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_3_8=val;
        }
        else
        {
            result_3_8=true;
            data_2.pass_num_3_8++;
            QString p_val=QString::number(PokerDetect_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(PokerDetect_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --Poker8 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_3_8=val;
        }
        free(bits_cp_4);
        free(bits_cp_8);
    }

    //重叠子序列检测
    if(checkbox4_ischeck==true)
    {
        //
        BitSequence* bits_cp_2=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_2,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_5=new BitSequence[(unsigned int)len];
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
            result_4_2=false;
            const double EPS = 1e-6;
            double sub_1 = Serial_p_Value_1 - ALPHA;
            double sub_2 = Serial_p_Value_2 - ALPHA;
            if((Serial_p_Value_1 > ALPHA + EPS) ||(fabs(sub_1) < EPS))
            {
                data_2.pass_num_4_2_1++;
            }
            if((Serial_p_Value_2 > ALPHA + EPS) ||(fabs(sub_2) < EPS))
            {
                data_2.pass_num_4_2_2++;
            }

            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="FAIL  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_2;
            val_1+="ms";
            val_1+=" --Serial2_1 Test";
            //ui->textBrowser->append(val_1);
            data_2.t_val_4_2_1=val_1;

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="FAIL  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_2;
            val_2+="ms";
            val_2+=" --Serial2_2 Test";
            //ui->textBrowser->append(val_2);
            data_2.t_val_4_2_2=val_2;
        }
        else
        {
            result_4_2=true;
            data_2.pass_num_4_2_1++;
            data_2.pass_num_4_2_2++;
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="PASS  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_2;
            val_1+="ms";
            val_1+=" --Serial2_1 Test";
            //ui->textBrowser->append(val_1);
            data_2.t_val_4_2_1=val_1;

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="PASS  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_2;
            val_2+="ms";
            val_2+=" --Serial2_2 Test";
            //ui->textBrowser->append(val_2);
            data_2.t_val_4_2_2=val_2;
        }
        begin_5=clock();
        ret_5=Serial(5,len,bits_cp_5);//m=5
        end_5=clock();
        double time_Costs_5=end_5-begin_5;
        QString time_C_5=QString::number(time_Costs_5,10,3);
        if(ret_5==0)
        {
            result_4_5=false;
            const double EPS = 1e-6;
            double sub_1 = Serial_p_Value_1 - ALPHA;
            double sub_2 = Serial_p_Value_2 - ALPHA;
            if((Serial_p_Value_1 > ALPHA + EPS) ||(fabs(sub_1) < EPS))
            {
                data_2.pass_num_4_5_1++;
            }
            if((Serial_p_Value_2 > ALPHA + EPS) ||(fabs(sub_2) < EPS))
            {
                data_2.pass_num_4_5_2++;
            }

            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="FAIL  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_5;
            val_1+="ms";
            val_1+=" --Serial5_1 Test";
            //ui->textBrowser->append(val_1);
            data_2.t_val_4_5_1=val_1;

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="FAIL  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_5;
            val_2+="ms";
            val_2+=" --Serial5_2 Test";
            //ui->textBrowser->append(val_2);
            data_2.t_val_4_5_2=val_2;
        }
        else
        {
            result_4_5=true;
            data_2.pass_num_4_5_1++;
            data_2.pass_num_4_5_2++;
            QString p_val_1=QString::number(Serial_p_Value_1,'f',6);
            p_val_1="PASS  p_value1 = "+p_val_1;
            QString v_val_1=QString::number(Serial_v_Value_1,'f',6);
            v_val_1="  统计值 = "+v_val_1;
            QString val_1=p_val_1+v_val_1;
            val_1+="  耗时:  ";
            val_1+=time_C_5;
            val_1+="ms";
            val_1+=" --Serial5_1 Test";
            //ui->textBrowser->append(val_1);
            data_2.t_val_4_5_1=val_1;

            QString p_val_2=QString::number(Serial_p_Value_2,'f',6);
            p_val_2="PASS  p_value2 = "+p_val_2;
            QString v_val_2=QString::number(Serial_v_Value_2,'f',6);
            v_val_2="  统计值 = "+v_val_2;
            QString val_2=p_val_2+v_val_2;
            val_2+="  耗时:  ";
            val_2+=time_C_5;
            val_2+="ms";
            val_2+=" --Serial5_2 Test";
            //ui->textBrowser->append(val_2);
            data_2.t_val_4_5_2=val_2;
        }
        free(bits_cp_2);
        free(bits_cp_5);
    }

    //游程总数检测
    if(checkbox5_ischeck==true)
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
            result_5=false;
            const double EPS = 1e-6;
            double sub = Runs_p_Value - ALPHA;
            if((Runs_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_5++;
            }
            QString p_val=QString::number(Runs_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Runs_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Runs Test";
            //ui->textBrowser->append(val);
            data_2.t_val_5=val;
        }
        else
        {
            result_5=true;
            data_2.pass_num_5++;
            QString p_val=QString::number(Runs_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Runs_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Runs Test";
            //ui->textBrowser->append(val);
            data_2.t_val_5=val;
        }
        free(bits_cp);
    }

    //游程分布检测
    if(checkbox6_ischeck==true)
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
            result_6=false;
            const double EPS = 1e-6;
            double sub = RunsDistribution_p_Value - ALPHA;
            if((RunsDistribution_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_6++;
            }

            QString p_val=QString::number(RunsDistribution_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(RunsDistribution_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --RunsDistribution Test";
            //ui->textBrowser->append(val);
            data_2.t_val_6=val;
        }
        else
        {
            result_6=true;
            data_2.pass_num_6++;
            QString p_val=QString::number(RunsDistribution_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(RunsDistribution_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --RunsDistribution Test";
            //ui->textBrowser->append(val);
            data_2.t_val_6=val;
        }
        free(bits_cp);
    }

    //块内最大"1"游程检测
    if(checkbox7_ischeck==true)
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
            result_7=false;
            const double EPS = 1e-6;
            double sub = LongestRunOfOnes_p_Value - ALPHA;
            if((LongestRunOfOnes_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_7++;
            }
            QString p_val=QString::number(LongestRunOfOnes_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(LongestRunOfOnes_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LongestRunOfOnes Test";
            //ui->textBrowser->append(val);
            data_2.t_val_7=val;
        }
        else
        {
            result_7=true;
            data_2.pass_num_7++;
            QString p_val=QString::number(LongestRunOfOnes_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(LongestRunOfOnes_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LongestRunOfOnes Test";
            //ui->textBrowser->append(val);
            data_2.t_val_7=val;
        }
        free(bits_cp);
    }

    //二元推导检测
    if(checkbox8_ischeck==true)
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
            result_8_3=false;
            const double EPS = 1e-6;
            double sub = BinaryDerivate_p_Value - ALPHA;
            if((BinaryDerivate_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_8_3++;
            }
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_3;
            val+="ms";
            val+=" --BinaryDerivate3 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_8_3=val;
        }
        else
        {
            result_8_3=true;
            data_2.pass_num_8_3++;
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_3;
            val+="ms";
            val+=" --BinaryDerivate3 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_8_3=val;
        }
        begin_7=clock();
        ret_7=BinaryDerivate(7,len,bits_cp_7);//k=7
        end_7=clock();
        double time_Costs_7=end_7-begin_7;
        QString time_C_7=QString::number(time_Costs_7,10,3);
        if(ret_7==0)
        {
            result_8_7=false;
            const double EPS = 1e-6;
            double sub = BinaryDerivate_p_Value - ALPHA;
            if((BinaryDerivate_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_8_7++;
            }
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_7;
            val+="ms";
            val+=" --BinaryDerivate7 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_8_7=val;
        }
        else
        {
            result_8_7=true;
            data_2.pass_num_8_7++;
            QString p_val=QString::number(BinaryDerivate_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(BinaryDerivate_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_7;
            val+="ms";
            val+=" --BinaryDerivate7 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_8_7=val;
        }
        free(bits_cp_3);
        free(bits_cp_7);
    }

    //自相关检测
    if(checkbox9_ischeck==true)
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
            result_9_1=false;
            const double EPS = 1e-6;
            double sub = SelfCorrelation_p_Value - ALPHA;
            if((SelfCorrelation_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_9_1++;
            }
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_1;
            val+="ms";
            val+=" --SelfCorrelation1 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_1=val;
        }
        else
        {
            result_9_1=true;
            data_2.pass_num_9_1++;
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_1;
            val+="ms";
            val+=" --SelfCorrelation1 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_1=val;
        }

        begin_2=clock();
        ret_2=SelfCorrelation(2,len,bits_cp_2);//d=2
        end_2=clock();
        double time_Costs_2=end_2-begin_2;
        QString time_C_2=QString::number(time_Costs_2,10,3);
        if(ret_2==0)
        {
            result_9_2=false;
            const double EPS = 1e-6;
            double sub = SelfCorrelation_p_Value - ALPHA;
            if((SelfCorrelation_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_9_2++;
            }
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --SelfCorrelation2 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_2=val;
        }
        else
        {
            result_9_2=true;
            data_2.pass_num_9_2++;
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_2;
            val+="ms";
            val+=" --SelfCorrelation2 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_2=val;
        }

        begin_8=clock();
        ret_8=SelfCorrelation(8,len,bits_cp_8);//d=8
        end_8=clock();
        double time_Costs_8=end_8-begin_8;
        QString time_C_8=QString::number(time_Costs_8,10,3);
        if(ret_8==0)
        {
            result_9_8=false;
            const double EPS = 1e-6;
            double sub = SelfCorrelation_p_Value - ALPHA;
            if((SelfCorrelation_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_9_8++;
            }
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --SelfCorrelation8 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_8=val;
        }
        else
        {
            result_9_8=true;
            data_2.pass_num_9_8++;
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_8;
            val+="ms";
            val+=" --SelfCorrelation8 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_8=val;
        }

        begin_16=clock();
        ret_16=SelfCorrelation(16,len,bits_cp_16);//d=16
        end_16=clock();
        double time_Costs_16=end_16-begin_16;
        QString time_C_16=QString::number(time_Costs_16,10,3);
        if(ret_16==0)
        {
            result_9_16=false;
            const double EPS = 1e-6;
            double sub = SelfCorrelation_p_Value - ALPHA;
            if((SelfCorrelation_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_9_16++;
            }
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_16;
            val+="ms";
            val+=" --SelfCorrelation16 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_16=val;
        }
        else
        {
            result_9_16=true;
            data_2.pass_num_9_16++;
            QString p_val=QString::number(SelfCorrelation_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(SelfCorrelation_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_16;
            val+="ms";
            val+=" --SelfCorrelation16 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_9_16=val;
        }
        free(bits_cp_1);
        free(bits_cp_2);
        free(bits_cp_8);
        free(bits_cp_16);
    }

    //矩阵秩检测
    if(checkbox10_ischeck==true)
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
            result_10=false;
            const double EPS = 1e-6;
            double sub = Rank_p_Value - ALPHA;
            if((Rank_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_10++;
            }
            QString p_val=QString::number(Rank_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Rank_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Rank Test";
            //ui->textBrowser->append(val);
            data_2.t_val_10=val;
        }
        else
        {
            result_10=true;
            data_2.pass_num_10++;
            QString p_val=QString::number(Rank_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Rank_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Rank Test";
            //ui->textBrowser->append(val);
            data_2.t_val_10=val;
        }
        free(bits_cp);
    }

    //累加和检测
    if(checkbox11_ischeck==true)
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
            result_11=false;
            const double EPS = 1e-6;
            double sub_f = CumulativeSums_p_Value_forward - ALPHA;
            if((CumulativeSums_p_Value_forward > ALPHA + EPS) || (fabs(sub_f) < EPS))//forward
            {
                data_2.pass_num_11_f++;
            }
            double sub_b = CumulativeSums_p_Value_backward - ALPHA;
            if((CumulativeSums_p_Value_backward > ALPHA + EPS) || (fabs(sub_b) < EPS))//backward
            {
                data_2.pass_num_11_b++;
            }

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
            //ui->textBrowser->append(val);
            data_2.t_val_11_f=val;

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
            //ui->textBrowser->append(val);
            data_2.t_val_11_b=val;
        }
        else
        {
            result_11=true;
            data_2.pass_num_11_f++;
            data_2.pass_num_11_b++;
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
            //ui->textBrowser->append(val);
            data_2.t_val_11_f=val;

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
            //ui->textBrowser->append(val);
            data_2.t_val_11_b=val;
        }
        free(bits_cp);
    }

    //近似熵检测
    if(checkbox12_ischeck==true)
    {
        //
        BitSequence* bits_cp_2=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_2,bits,(unsigned int)len*sizeof(BitSequence));
        BitSequence* bits_cp_5=new BitSequence[(unsigned int)len];
        memcpy(bits_cp_5,bits,(unsigned int)len*sizeof(BitSequence));

        double begin_2=0,end_2=0,begin_5=0,end_5=0;
        int ret_2,ret_5;
        begin_2=clock();
        ret_2=ApproximateEntropy(2,len,bits_cp_2);
        end_2=clock();
        double time_Costs=end_2-begin_2;
        QString time_C=QString::number(time_Costs,10,3);
        if(ret_2==0)
        {
            result_12_2=false;
            const double EPS = 1e-6;
            double sub = ApproximateEntropy_p_Value - ALPHA;
            if((ApproximateEntropy_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_12_2++;
            }
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --ApproximateEntropy Test";
            //ui->textBrowser->append(val);
            data_2.t_val_12_2=val;
        }
        else
        {
            result_12_2=true;
            data_2.pass_num_12_2++;
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --ApproximateEntropy Test";
            //ui->textBrowser->append(val);
            data_2.t_val_12_2=val;
        }
        begin_5=clock();
        ret_5=ApproximateEntropy(5,len,bits_cp_5);
        end_5=clock();
        double time_Costs_5=end_5-begin_5;
        QString time_C_5=QString::number(time_Costs_5,10,3);
        if(ret_5==0)
        {
            result_12_5=false;
            const double EPS = 1e-6;
            double sub = ApproximateEntropy_p_Value - ALPHA;
            if((ApproximateEntropy_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_12_5++;
            }
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_5;
            val+="ms";
            val+=" --ApproximateEntropy5 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_12_5=val;
        }
        else
        {
            result_12_5=true;
            data_2.pass_num_12_5++;
            QString p_val=QString::number(ApproximateEntropy_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(ApproximateEntropy_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C_5;
            val+="ms";
            val+=" --ApproximateEntropy5 Test";
            //ui->textBrowser->append(val);
            data_2.t_val_12_5=val;
        }
        free(bits_cp_2);
        free(bits_cp_5);
    }

    //线性复杂度检测
    if(checkbox13_ischeck==true)
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
            result_13=false;
            const double EPS = 1e-6;
            double sub = LinearComplexity_p_Value - ALPHA;
            if((LinearComplexity_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_13++;
            }
            QString p_val=QString::number(LinearComplexity_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(LinearComplexity_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LinearComplexity Test";
            //ui->textBrowser->append(val);
            data_2.t_val_13=val;
        }
        else
        {
            result_13=true;
            data_2.pass_num_13++;
            QString p_val=QString::number(LinearComplexity_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(LinearComplexity_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --LinearComplexity Test";
            //ui->textBrowser->append(val);
            data_2.t_val_13=val;
        }
        free(bits_cp);
    }

    //通用统计检测
    if(checkbox14_ischeck==true)
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
            result_14=false;
            const double EPS = 1e-6;
            double sub = Universal_p_Value - ALPHA;
            if((Universal_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_14++;
            }
            QString p_val=QString::number(Universal_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(Universal_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Universal Test";
            //ui->textBrowser->append(val);
            data_2.t_val_14=val;
        }
        else
        {
            result_14=true;
            data_2.pass_num_14++;
            QString p_val=QString::number(Universal_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(Universal_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --Universal Test";
            //ui->textBrowser->append(val);
            data_2.t_val_14=val;
        }
        free(bits_cp);
    }

    //离散傅立叶检测
    if(checkbox15_ischeck==true)
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
            result_15=false;
            const double EPS = 1e-6;
            double sub = DiscreteFourierTransform_p_Value - ALPHA;
            if((DiscreteFourierTransform_p_Value > ALPHA + EPS) || (fabs(sub) < EPS))
            {
                data_2.pass_num_15++;
            }
            QString p_val=QString::number(DiscreteFourierTransform_p_Value,'f',6);
            p_val="FAIL  p_value = "+p_val;
            QString v_val=QString::number(DiscreteFourierTransform_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --DiscreteFourierTransform Test";
            //ui->textBrowser->append(val);
            data_2.t_val_15=val;
        }
        else
        {
            result_15=true;
            data_2.pass_num_15++;
            QString p_val=QString::number(DiscreteFourierTransform_p_Value,'f',6);
            p_val="PASS  p_value = "+p_val;
            QString v_val=QString::number(DiscreteFourierTransform_v_Value,'f',6);
            v_val="  统计值 = "+v_val;
            QString val=p_val+v_val;
            val+="  耗时:  ";
            val+=time_C;
            val+="ms";
            val+=" --DiscreteFourierTransform Test";
            //ui->textBrowser->append(val);
            data_2.t_val_15=val;
        }
        free(bits_cp);
    }

//    //全部检测项都通过
//    if (checkbox_ischeck==true&&checkbox2_ischeck==true
//            &&checkbox3_ischeck==true&&checkbox4_ischeck==true
//            &&checkbox5_ischeck==true&&checkbox6_ischeck==true
//            &&checkbox7_ischeck==true&&checkbox8_ischeck==true
//            &&checkbox9_ischeck==true&&checkbox10_ischeck==true
//            &&checkbox11_ischeck==true&&checkbox12_ischeck==true
//            &&checkbox13_ischeck==true&&checkbox14_ischeck==true
//            &&checkbox15_ischeck==true)
//    {
//        if(result==true&&result_2==true&&result_3_4==true&&result_3_8==true&&result_4_2==true&&result_4_5==true&&result_5==true
//                &&result_6==true&&result_7==true&&result_8_3==true&&result_8_7==true&&result_9_1==true&&result_9_2==true&&result_9_8==true
//                &&result_9_16==true&&result_10==true&&result_11==true&&result_12_2==true&&result_12_5==true&&result_13==true&&result_14==true&&result_15==true)
//        {
//            data_2.pass_num_th2++;
//        }
//    }

    //    else if(checkbox_ischeck==false||checkbox2_ischeck==false
    //            ||checkbox3_ischeck==false||checkbox4_ischeck==false
    //            ||checkbox5_ischeck==false||checkbox6_ischeck==false
    //            ||checkbox7_ischeck==false||checkbox8_ischeck==false
    //            ||checkbox9_ischeck==false||checkbox10_ischeck==false
    //            ||checkbox11_ischeck==false||checkbox12_ischeck==false
    //            ||checkbox13_ischeck==false||checkbox14_ischeck==false
    //            ||checkbox15_ischeck==false)
    //    {
    //        return -2;
    //    }
    return  0;
}
