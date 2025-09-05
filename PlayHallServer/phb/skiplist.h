#pragma once
#include <vector>
#include<packdef.h>

// 玩家结构体，包含分数
struct CPlayer {
    int id;
    int score; // 用于排序的分数
    char name[_MAX_SIZE];// 可以添加其他玩家信息
};

// 跳表节点类
class SkipListNode {
public:
    CPlayer data; // 存储玩家数据
    SkipListNode** forward; // 指向各层下一个节点的指针数组

    // 构造函数
    SkipListNode(int level, const CPlayer& player);
    // 析构函数
    ~SkipListNode();

};

// 跳表类
class SkipList {
private:
    int maxLevel; // 最大层数
    double probability; // 提升层数的概率
    int level; // 当前最高层数
    SkipListNode* header; // 头节点

    // 生成随机层数
    int randomLevel();

public:
    // 构造函数
    SkipList(int maxLevel = 16, double probability = 0.5);
    // 析构函数
    ~SkipList();

    // 插入节点，返回新插入节点的指针
    SkipListNode* insert(CPlayer& player);

    // 删除指定节点，返回是否删除成功
    bool deleteNode(CPlayer& targetPlayer);

    // 将跳表前10个节点内容拷贝到vector中，通过引用传出
    void getTop10(std::vector<CPlayer>& resultVector);
};
