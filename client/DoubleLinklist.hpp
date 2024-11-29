/*
实现双向循环链表
实现的运算操作有
 增 ,删 ,改 ,查
 初始化, 清空 ,求线性表长度
*/

#ifndef __DOULBELINKLIST_HPP__
#define __DOULBELINKLIST_HPP__

#include "node.h"

using namespace std;

template <typename DataType>
class DoubleLinkList
{
public:
    int length;
    Node<DataType> *head;

    DoubleLinkList();                              // 初始化————初始化一个空表，
    DoubleLinkList(const DataType data_[], int n); // 初始化————初始化一个含有数据的表
    void Insert(const DataType data_, int pos);    // 增————向某个位置pos之前插入一个数据data_
    void Delete(int pos);                          // 删————删除某个位置pos处的结点
    void Change(const DataType data_, int pos);    // 改————改动某个位置pos处的结点
    DataType Search1(int pos);                     // 查————根据下标查数据
    int Search2(const DataType &e);                // 查————根据数据查下标
    void Clear();                                  // 清空————仅保留头节点
    int GetLength();                               // 得到长度
    void PrintAll();                               // 遍历链表输出各节点数值
    void SaleRecords();                            // 销售记录————输出销售记录（额外添加）
    ~DoubleLinkList();    // 析构————删除所有结点,释放所有指针
};

// 清屏
void clearorders();
// 功能选择
void choose();
// 链表总节点
void* list_start(void*arg);

#endif
