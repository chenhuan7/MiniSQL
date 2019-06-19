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
    std::vector<int> values;
    TreeNode *nextLeaf;
    
    TreeNode(TreeNode *Parent=NULL, bool Leaf=false);
    void clear();
    bool insertKey(const T &key, int value);
    bool insertKey(const T &key);
    void insert(int childIndex,const T &key, TreeNode *childNode);
    bool deleteKey(const T &key);
    TreeNode<T>* findKey(const T &key, int &index);
    TreeNode<T>* split(T key);
    int getKeyIndex(const T &key);
    
private:
    bool isLeaf;
    int degree;
};

template <typename T>
class BPlusTree {
public:
    BPlusTree(std::string Name, int Size, int Degree);
    ~BPlusTree();
    bool insertKey(const T &key, int value);
    bool deleteKey(const T &key);
    int searchVal(const T &key);
    void searchRange(T data1, T data2, std::vector<int> &vals);
    
private:
    typedef TreeNode<T>* Tree;
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
    
    void initBPlusTree();
    void readValues();
    bool adjustAfterinsert(TreeNode<T> *pNode);
};

#endif /* BPlusTree_hpp */
