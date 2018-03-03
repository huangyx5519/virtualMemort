#include <iostream>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <iomanip>
#include<fstream>

using namespace std;

#define TOTALNUMBER 512*1024
#define MEMORYNUMBER 8192          //主存pageframe number数
#define JOBNUMBER  5               //进程数5个
#define WorkObject 500             //单个进程产生的指令序列


// 总访问次数
double TotalCount = 0;
// 缺页次数
double LackCount = 0;

// 指针
int Point = 0;
// LRU作业数组，每个元素记录页表使用情况信息
int WorkRecord[MEMORYNUMBER];


string dec2hex(long i){
    stringstream ioss; //定义字符串流
    string s_temp; //存放转化后字符
    ioss << setiosflags(ios::uppercase) << hex << i; //以十六制(大写)形式输出
    ioss >> s_temp;
    int zero_num =0;
    zero_num = 8 - s_temp.length();
    for(int i=0;i<zero_num;i++){
        s_temp = "0" + s_temp;
    }
    s_temp = "0x" + s_temp;
//    cout<<s_temp;
    return s_temp;
}

//16进制转int
long hex2dec(string s){
    s.operator[](2);
    const char *c = s.data();
    long i;
    sscanf(c,"%lx",&i);
//    cout<<i<<endl;
    return i;
}


//--------------------------------------------
//    页表结构体
//--------------------------------------------
typedef struct table
{
    int TableNum;                    // 页号          第几页 ，与main中的tabNum区别，后者是总数
    int mark;                       // 标志
    int BlockNumX;                   // 主存块号       第几块
    int VirMemX;                     // 虚拟内存块号
}TABLE;

class PROCESS {

public:
    TABLE* PT[TOTALNUMBER];
    int LackCount=0;
    int process_size;
    string ac[3][500];
    PROCESS(int cur_p,int proce){
        process_size = proce;
        char buf[50];
        sprintf(buf,"./addr_seq_%d.txt",cur_p);
        ofstream ofile;               //定义输入文件
        ofile.open(buf);

        int temple;
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, process_size);
        for(int k=0;k<3;k++) {
            for (int i = 0; i < 50; i++) {
                temple = dist(rd);
                for (int j = 0; j < 10; j++) {
                    //处理上下半区//处理 temple+j 的进制转化
                    if (temple + j < process_size / 2) {
                        ac[k][i * 10 + j] = dec2hex(temple + j);
                    } else {
                        ac[k][i * 10 + j] = dec2hex(4294967295 - process_size + temple + j + 1);
                    }
                    ofile<<ac[i * 10 + j]<<endl;
                }
            }
        }


        for(int j=0; j<1024*512; j++)    //初始化page table
        {
            TABLE *tab = new TABLE();  //表项
            tab->mark = 0;
            tab->TableNum = j+1;
            tab->BlockNumX = -1;
            tab->VirMemX = 0;
            PT[j]= tab;
        }

    }
};

PROCESS * processes[JOBNUMBER];


long TLB[16][0];


void TLBReset(){
    for(int i=0;i<16;i++) {
        *TLB[i]=-1;
    }
}


//-------------------------------------------
//
//       全局函数定义模块
//
//-------------------------------------------
// 作业工作
void ApplicationWork(int num, int m)
{
    printf("作业%d访问%d页\n",num,m);
}
//*--------------------------------------------------------------------
// 初始化作业数组
void InitWorkRecord()
{
    for(int i=0; i<MEMORYNUMBER; i++)
    {
        WorkRecord[i] = -1;
    }
}

// 计算显示缺页率
void Compute(double LackCount)
{
    double V;
    V = LackCount / WorkObject ;
    printf("此次访问的缺页率为:%.2f\n",V);
}


// 虚拟内存中的数据加载到内存中
// 这一模块使用替换算法实现
void LRU(int JobSign,int TableSign,PROCESS *  c_process)
{
    int Last = WorkRecord[MEMORYNUMBER-1];

    TABLE *virtable =  c_process->PT[TableSign];//调入页表   LRU调入页面号作为参数传入，：LRU换出算法 考虑tlb参数，传工作数组参数

    if(Point >= MEMORYNUMBER)
    {
        TABLE *tableout =  c_process->PT[Last];/*调出页表*/
        //分页缺失之后的操作，p108  页表项，virtual page
        //-------------------------------------------------------------------
        tableout->mark = 0;
        printf("作业%d第%d页调出\n",JobSign+1, tableout->TableNum);

        virtable->mark = 1;
        printf("作业%d第%d页调入\n",JobSign+1, virtable->TableNum);
        virtable->BlockNumX =  tableout->BlockNumX;  //页表和memory，vitrual
        tableout->BlockNumX = -1;
    }
    else
    {
        virtable->mark = 1;
        virtable->BlockNumX=Point;
        Point++;
    }
}

// 调整页表数组   排序方便之后的跳出
void AdjustWorkRecord(int JobSign, int TableSign)  //传入参数 调整的数组
{
    if(Point <= 0)
    {
        printf("页表数组为空!\n");
    }

    else
    {
        int i;
        for(i=0; i<MEMORYNUMBER; i++)
        {
            if(WorkRecord[i] == TableSign)
            {
                break;
            }
        }


        for(int j=i; j>0; j--)
        {
            WorkRecord[j] = WorkRecord[j-1];
        }
        WorkRecord[0] = TableSign;    //存放最新的列表项，最新的放在最前面

    }
}





//将int转成16进制字符串


//string* new_sequence(int process_size,int cur_p) {
////生成500个随机访问序列
//    char buf[50];
//    sprintf(buf,"./addr_seq_%d.txt",cur_p+1);
//    ofstream ofile;               //定义输入文件
//    ofile.open(buf,ios::app);
//
//    int temple;
//    string* ac_sequence = new string[WorkObject];
//    std::random_device rd;
//    std::uniform_int_distribution<int> dist(0, process_size);
//    for (int i = 0; i < 50; i++) {
//        temple = dist(rd);
//        for (int j = 0; j < 10; j++) {
////处理上下半区//处理 temple+j 的进制转化
//            if (temple + j < process_size / 2) {
//                ac_sequence[i * 10 + j] = dec2hex(temple + j);
//            } else {
//                ac_sequence[i * 10 + j] = dec2hex(4294967295 - process_size + temple + j + 1);
//            }
//            ofile<<ac_sequence[i * 10 + j]<<endl;
//        }
//    }
//    ofile.close();
//
//    return ac_sequence;
//}


bool is_in_TLB(int TableSign){
    bool is_in =false;
    int h=-1;
    for(int i=0;i<16;i++) {
        if (TableSign == *TLB[i]) {
            is_in = true;
        }
    }
    if(is_in){
//        cout << "TLB hit" << "\t" << "don't visit PT" <<"\t"<< "VP number：" << TableSign << "\t"
//        << "PF number:" << TLB[h][0] << "\n";
    }
    return is_in;
}


void AdjustTLB(int TableSign)  //传入参数 调整的数组
{
    int i;
    for(i=0; i<16; i++)
    {
        if(*TLB[i] == TableSign)
        {
            break;
        }
    }

//        for(int j=i; j>0; j--)
    for(int j=i; j>0; j--)
    {
        *TLB[j] = *TLB[j-1];
    }
    *TLB[0] = TableSign;    //存放最新的列表项，最新的放在最前面
}


int main() {
    //初始化输出文本
    ofstream ofile[5];
    ofstream visit[5];

    //初始化进程
    processes[0] = new PROCESS(1,1 * 1024 * 1024);
    processes[1] = new PROCESS(2,32 * 1024 * 1024);
    processes[2] = new PROCESS(3,64 * 1024 * 1024);
    processes[3] = new PROCESS(4,128 * 1024 * 1024);
    processes[4] = new PROCESS(5,256 * 1024 * 1024);

    PROCESS *c_process;

    //执行循环
//    for(int turn =0;turn<3;turn++) {
    for (int i = 0; i < JOBNUMBER; i++) {
        c_process=processes[i];
        char buf2[50];
        sprintf(buf2, "./visit_seq_%d.txt", i + 1);
        visit[i].open(buf2);
        c_process = processes[i];
//        for (int i = 0; i < JOBNUMBER; i++) {
        for(int turn =0;turn<3;turn++) {
            TLBReset();
            for (int j = 0; j < 500; j++) {

                TotalCount++;
//                cout << c_process->ac[turn][j]<<"\t";
                long worknumber = hex2dec(c_process->ac[turn][j]);//*
//                cout<<worknumber<<"\t";
                int TableSign = worknumber / (8 * 1024);
//                cout<<TableSign<<"\n";
                if (is_in_TLB(TableSign)) {
                    visit[i] << "TLB hit" << "\t" << "don't visit PT" <<"\t"<< "VP number：" << TableSign << "\t"
                             << "PF number:" << c_process->PT[TableSign]->BlockNumX << "\n";
                    AdjustTLB(TableSign);
                } else {
                    AdjustTLB(TableSign);
                    visit[i] << "TLB no hit" << "\t";
                    if (c_process->PT[TableSign]->mark == 1) {
                        AdjustWorkRecord(i, TableSign);
                        visit[i] << "PT hit" << "\t" << "VP number：" << TableSign << "\t" << "PF number:"
                                 << c_process->PT[TableSign]->BlockNumX << "\n";
                    } else if (c_process->PT[TableSign]->mark == 0) {
                        c_process->LackCount++;    //命中缺失
                        LRU(i, TableSign, c_process);
                        //                    cout << Point;
                        AdjustWorkRecord(i, TableSign);
                        visit[i] << "PT no hit" << "\t" << "VP number：" << TableSign << "\t" << "PF number:"
                                 << c_process->PT[TableSign]->BlockNumX << "\n";
                    }
                }
            }
//        cout<<c_process->LackCount<<endl;
        }
//        cout<<"进程"<<i<<"页缺失率:"<<double(processes[i]->LackCount)/1500;
//        cout<<"进程"<<i<<"页缺失率:"<<double(c_process->LackCount)/1500<<endl;
        visit[i]<<"进程"<<i+1<<"页缺失率:"<<double(c_process->LackCount)/1500<<endl;
    }

//    for (int i = 0; i < JOBNUMBER; i++) {
//        cout<<"进程"<<i<<"页缺失率:"<<double(processes[i]->LackCount)/1500;
//    }

    for (int i = 0; i < JOBNUMBER; i++){
        char buf[50];
        sprintf(buf, "./page_table_%d.txt", i + 1);
        ofile[i].open(buf);
        ofile[i] << "    虚拟地址        mark位          物理地址" << endl;
        for (int TableSign = 0; TableSign < 1024 * 512; TableSign++)    //初始化page table
        {
            TABLE *tab = c_process->PT[TableSign];
            ofile[i] << setw(12) << tab->TableNum << setw(12) << tab->mark << setw(12) << tab->BlockNumX
                     << "\n";
        }
    }
}


