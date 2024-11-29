#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <cassert>
#include <limits>


using namespace std;

// 定义菜品数据结构
class DataType
{
    public:
    string name;    // 菜名
    string taste;   // 口味
    int price;        // 价格
    int number;       // 售出份数
    int empty;        // 是否售罄（1表示已售罄，0表示未售罄）
    string picture; // 图片路径

    DataType(const char *name_="", const char *taste_="", int price_=0, int number_=0, int empty_=0, const char *picture_="default")
    : price(price_), number(number_), empty(empty_), name(name_), taste(taste_), picture(picture_)
    {};
};


template <class DataType>
class Node
{
    public:
    DataType data;
    Node<DataType> *prior;
    Node<DataType> *next;
    Node();
    Node(DataType data_, Node<DataType> *prior_, Node<DataType> *next_);
};
template <class DataType>
Node<DataType>::Node() // 定义一个空结点
{
    prior = NULL;
    next = NULL;
}

template <class DataType>
Node<DataType>::Node(DataType data_, Node<DataType> *prior_, Node<DataType> *next_) // 定义一个完整结点
{
    prior = prior_;
    next = next_;
    data = data_;
}


#endif