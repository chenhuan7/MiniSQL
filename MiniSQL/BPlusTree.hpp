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
};

template <typename T>
class BPlusTree {
public:
    BPlusTree(std::string Name, int Size, int Degree);
    ~BPlusTree();
    void initBPlusTree();
    void readValues();
    void insertKey(T &key);
    void deleteKey(T &key);
private:
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
};

#endif /* BPlusTree_hpp */
