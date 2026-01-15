#pragma once
#include<stdbool.h>
#include <stdlib.h>
#include<stdio.h>
struct ChannelMap
{   //struct Channel* list[]  这个二级指针相当于是定义了一个指针数组 数组的每个元素是指针
    struct Channel** list;
    int size; // 记录指针指向的数组的元素的总个数
};

//初始化  
struct ChannelMap* channelMapInit(int size);
//清空map
void  ChannelMapClear(struct ChannelMap* map);
//重新分配内存空间 第一个参数是要给哪一个map对应的内存进行扩容  
// 第二个参数是在扩容的时候，新的内存大小是多少也就是新的元素的个数
// 第三个参数是每个元素的最小单元是多少个字节 因为每个元素都是一个指针，而指针是4字节 所以这个值在这个数组里就是4
bool makeMapRoom(struct ChannelMap* map,int newSize,int unitSize); 