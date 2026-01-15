#include"ChannelMap.h"

struct ChannelMap* channelMapInit(int size)
{
        struct ChannelMap* map=(struct ChannelMap*)malloc(sizeof(struct ChannelMap));
        map->size=size;
        map->list=(struct Channel**)malloc(size*sizeof(struct Channel*));
        return map;
}
void  ChannelMapClear(struct ChannelMap* map)
{
    if(map!=NULL)
    {
        for(int i=0;i<map->size;i++)
        {
            if(map->list[i]!=NULL)  //如果数组里面的元素指向的内存不为空
            {
                free(map->list[i]);
            }
        }
    free(map->list); //释放这个数组对应的那块内存地址
    map->list=NULL; 
    }
    map->size=0;
}
bool makeMapRoom(struct ChannelMap* map,int newSize,int unitSize)
{
    if(map->size<newSize)
    {
        int curSize=map->size;
        //这里我们指定容量每次扩大到原来的一倍
        while(curSize<newSize)
        {
            curSize*=2;
            //扩容 realloc 这里有一个问题，扩容会出现两种情况 第一种直接在后面追加成功
            //第二种是后面内存不够用 realloc会重新找一块内存地址把前面已有的进行拷贝然后再追加
            //此时list的值会变化   realloc的返回值就是函数调用成功之后指向的那个数据块的起始地址 
            //这个起始地址有可能是原来的地址，也有可能是一个新的内存地址所有要进行一下判断
            struct Channel** temp=realloc(map->list,curSize*unitSize);
            if(temp==NULL)
            {
                return false;
            }
            map->list=temp;
            //给扩展出的那块地址进行初始化
            // 初始化的起始位置是元素的总个数+1 也就是map->size
            //第三个参数是填充的字节数：就是（扩展出来的元素个数-旧的元素个数）*每个元素的字节数
            memset(map->list[map->size],0,(curSize-map->size)*unitSize);
            map->size=curSize;
        }
    }
    return true;
}   