//
//  BPlusTree.h
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef BPlusTree_h
#define BPlusTree_h
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
private:
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
};

BPlusTree<T>::BPlusTree(std::string Name, int Size, int Degree):
    fileName(Name), keySize(Size), degree(Degree), root(NULL)
{
    initBPlusTree();
    read();
}

#endif /* BPlusTree_h */
