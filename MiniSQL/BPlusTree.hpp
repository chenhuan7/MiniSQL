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
    TreeNode *parent;
    std::vector<T> keys;
    std::vector<TreeNode*> childs;
    
    TreeNode(TreeNode *Parent=NULL, bool Leaf=false);
    void clear();
    bool insertKey(const T &key, int value);
    bool deleteKey(const T &key);
    void split(int childIndex);
    
private:
    bool isLeaf;
    int degree;
    std::vector<int> values;
    TreeNode *nextLeaf;
    
    int getKeyIndex(const T &key);
    void insert(int childIndex,const T &key, TreeNode *childNode);
    bool findKey(const T &key);
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
