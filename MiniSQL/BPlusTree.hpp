//
//  BPlusTree.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef BPlusTree_hpp
#define BPlusTree_hpp
#include "MiniSQL.h"

template <typename T>
class TreeNode {
public:
    int keyNum;
    
    TreeNode(bool Leaf);
    void clear();
    bool insertKey(const T &key, int value);
    void split(int childIndex);
    
private:
    TreeNode *parent;
    bool isLeaf;
    int degree;
    std::vector<T> keys;
    std::vector<int> values;
    std::vector<TreeNode*> childs;
    TreeNode *nextLeaf;
    
    int getKeyIndex(const T &key);
    bool findKey(const T &key, TreeNode *p);
};



template <typename T>
class BPlusTree {
public:
    BPlusTree(std::string Name, int Size, int Degree);
    ~BPlusTree();
    bool insertKey(const T &key, int value);
    bool deleteKey(const T &key);
    
private:
    typedef TreeNode<T>* Tree;
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
    
    void initBPlusTree();
    void readValues();
};

#endif /* BPlusTree_hpp */
