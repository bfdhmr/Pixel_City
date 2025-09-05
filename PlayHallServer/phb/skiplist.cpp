#include "skiplist.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

// SkipListNode 实现

SkipListNode::SkipListNode(int level, const CPlayer& player) : data(player) {
    forward = new SkipListNode * [level];
    for (int i = 0; i < level; ++i) {
        forward[i] = nullptr;
    }
}

SkipListNode::~SkipListNode() {
    delete[] forward;
}

// SkipList 实现

SkipList::SkipList(int maxLevel, double probability)
    : maxLevel(maxLevel), probability(probability), level(1) {
    // 初始化头节点
    CPlayer dummyPlayer;
    dummyPlayer.score = 0; // 头节点分数不影响排序
    header = new SkipListNode(maxLevel, dummyPlayer);

    // 初始化随机数生成器
    std::srand(std::time(nullptr));
}

SkipList::~SkipList() {
    // 遍历删除所有节点
    SkipListNode* current = header->forward[0];
    SkipListNode* next;

    while (current != nullptr) {
        next = current->forward[0];
        delete current;
        current = next;
    }

    delete header;
}

int SkipList::randomLevel() {
    int level = 1;
    while ((std::rand() % 100) < (probability * 100) && level < maxLevel) {
        level++;
    }
    return level;
}

SkipListNode* SkipList::insert(CPlayer& player) {
    SkipListNode* update[16];
    SkipListNode* current = header;

    // 从最高层开始查找插入位置
    for (int i = level - 1; i >= 0; --i) {
        // 找到当前层中最后一个分数大于插入分数的节点
        while (current->forward[i] != nullptr &&
            current->forward[i]->data.score > player.score) {
            current = current->forward[i];
        }
        // 对于相同分数，找到最后一个相同分数的节点
        while (current->forward[i] != nullptr &&
            current->forward[i]->data.score == player.score) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 生成新节点的随机层数
    int newLevel = randomLevel();

    // 如果新节点的层数高于当前跳表的最高层数
    if (newLevel > level) {
        for (int i = level; i < newLevel; ++i) {
            update[i] = header;
        }
        level = newLevel;
    }

    // 创建新节点并插入
    SkipListNode* newNode = new SkipListNode(newLevel, player);
    for (int i = 0; i < newLevel; ++i) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }

    return newNode;
}

bool SkipList::deleteNode(CPlayer& targetPlayer) {
    if (/* 可根据实际情况补充对 targetPlayer 合法性判断，比如 score 等字段有效 */ true) {
        SkipListNode* update[16];
        SkipListNode* current = header;

        // 查找要删除节点的前驱节点，类似查找逻辑，依据 score 定位
        for (int i = level - 1; i >= 0; --i) {
            while (current->forward[i] != nullptr &&
                   current->forward[i]->data.score > targetPlayer.score) {
                current = current->forward[i];
            }
            // 处理相同分数的情况，找到对应节点
            while (current->forward[i] != nullptr &&
                   current->forward[i]->data.score == targetPlayer.score &&
                   !(current->forward[i]->data.id == targetPlayer.id)) {  // 假设 CPlayer 实现了 == 运算符重载，用于精确匹配
                current = current->forward[i];
            }
            update[i] = current;
        }

        // 定位到待删除节点
        SkipListNode* nodeToDelete = current->forward[0];
        if (nodeToDelete == nullptr || !(nodeToDelete->data.id == targetPlayer.id)) {
            return false; // 未找到对应节点
        }

        // 更新跳表指针，移除节点
        for (int i = 0; i < level; ++i) {
            if (update[i]->forward[i] != nodeToDelete) {
                break;
            }
            update[i]->forward[i] = nodeToDelete->forward[i];
        }

        // 更新跳表的最高层数
        while (level > 1 && header->forward[level - 1] == nullptr) {
            level--;
        }

        // 删除节点
        delete nodeToDelete;
        return true;
    }
    return false;
}

void SkipList::getTop10(std::vector<CPlayer>& resultVector) {
    // 清空向量，确保结果正确
    resultVector.clear();

    SkipListNode* current = header->forward[0];
    int count = 0;

    // 最多取10个节点
    while (current != nullptr && count < 10) {
        resultVector.push_back(current->data);
        current = current->forward[0];
        count++;
    }
}
