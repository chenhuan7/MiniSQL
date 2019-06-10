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
typedef TreeNode<T>* Tree;

template <typename T>
class TreeNode {
public:
    TreeNode();
    void insertKey(const T &key, int value);
    
private:
    Tree parent;
    bool isLeaf;
    int degree;
    int keyNum;
    std::vector<T> keys;
    std::vector<Tree> childs;
    Tree nextLeaf;
    
    bool findKey(const T &key, Tree &p);
};

template <typename T>
class BPlusTree {
public:
    BPlusTree(std::string Name, int Size, int Degree);
    ~BPlusTree();
    void insertKey(const T &key, int value);
    void deleteKey(const T &key);
    
private:
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
    
    void initBPlusTree();
    void readValues();
};

#endif /* BPlusTree_hpp */
