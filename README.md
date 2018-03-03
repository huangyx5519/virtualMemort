一.实验目的和实验内容

	实现虚拟内存的分页系统， 模拟进程调度过程中的内存管理

二.实验运行环境
	macOS  IDE：clion  编辑器：clang

三.数据结构及符号说明
数据结构：
1.page table 列表项：
typedef struct table
{
    int TableNum;                    // 页号        
    int mark;                        // 标志 0or1
    int BlockNumX;                   // 主存块号       
    int VirMemX;                     // 虚拟内存块号
}TABLE;
2.page table:
TABLE* PT[TOTALNUMBER];

3.TLB列表项：
typedef struct TLBlist
{
    int BlockNumX;                   // 主存块号       
    int VirMemX;                     // 虚拟内存块号
}TABLELIST;

4.TLB
TLBlist TLB[16]

5.进程
class PROCESS {
public:
    TABLE* PT[TOTALNUMBER];             //进程的pagetable
    int LackCount;						 //缺页计数
    int process_size;                  //进程大小
    PROCESS(){
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
四.其他重要函数和符号说明
1.生成随机访问地址数组
string* new_sequence(int process_size)   
2.模拟LRU
void LRU(int JobSign,int TableSign,PROCESS *  c_process)
3.调整WorkRecord，辅助LRU的替换
void AdjustWorkRecord(int JobSign, int TableSign) 
4.判断TLB是否命中
bool is_in_TLB(int TableSign)
5.TLB LRU替换操作
void AdjustTLB(int TableSign) 




五.程序流程图及流程分析


a)五个进程轮流执行，总共执行三轮。 每个进程在第一次执行时，页表内容为空，每次执行完毕出现进程切 换时，将其页表内容保存到外部文件。在该程序之后执行时，首先从 外部文件导入页表内容。 TLB 用来保存当前运行进程最近的页访问信息，因此在进程切换时， 需要清空 TLB 为新进程的运行做准备。 
b) 每个进程在一次执行过程中，利用随机函数生成 500 次虚拟地址的 访问序列。例如，进程 1 在执行过程中，生成 500 个随机虚拟地址 访问序列，每个虚拟地址均需落在 0x00000000~0x0007FFFF 或者 0xFFFF8000~0xFFFFFFFF 范围内。 生成随机虚拟地址序列的方式为：首先随机生成一个地址，然后以该 地址为起始点生成九个连续的地址。按此方法循环 50 次，总共生成 500 次随机虚拟地址访问。
c) 若访问某个虚拟地址，首先生成对应的虚拟页号（virtual page number），再查找该虚拟页对应的物理页框号（page frame number） 是否在 TLB；若不在 TLB，进一步查找是否在 page table；若也不在， 则分配一个物理页框给该页面，并将虚拟页到物理页的映射信息保 存在 page table 和 TLB 中。 若 TLB 已满，按照 LRU 的置换策略置换掉最早被访问的 entry。
d) 物理页面在分配的过程中，若仍然有可用的页框，按照页框编号从小 到大依次分配。若没有可用页框，则利用 LRU 置换策略，将最早分 配的页框收回重新分配给当前的虚拟页。被收回的页框若是已经分 配给其他进程，则需要更新该进程的 page table。



六.程序运行时的参数设置及运行结果说明

内存大小64MB，进程大小分别为为 1MB， 32MB，64MB，128MB，和 256MB，每轮每进程进行500次地址请求，页大小8KB。
