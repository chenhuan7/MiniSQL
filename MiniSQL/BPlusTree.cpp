//
//  BPlusTree.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "BPlusTree.hpp"

template <typename T>
BPlusTree<T>::BPlusTree(std::string Name, int Size, int Degree):
fileName(Name), keySize(Size), degree(Degree), root(NULL)
{
    initBPlusTree();
    readValues();
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
}

template <typename T>
bool BPlusTree<T>::insertKey(const T &key, int value) {
    Tree p = NULL;
    if (root->findKey(key, p)) {
        return false;
    }
    p->insertKey(key, value);
    return true;
}

template <typename T>
TreeNode<T>::TreeNode(){
    
}

template <typename T>
bool TreeNode<T>::findKey(const T &key, Tree &p) {
    if (!keys.size()) {
        return false;
    }
    if (keys[0]>key || keys[keys.size()-1]<key){
        return false;
    }
    
    int left=0, right=keys.size()-1;
    int pnow = (left+right)/2;
    while (left <= right) {
        pnow = (left+right)/2;
        if (keys[pnow] < key)
            left = pnow+1;
        else
            right = pnow-1;
    }
    if (keys[pnow] == key) {
        p = childs[pnow+1];
        return true;
    }
    else {
        p = childs[pnow];
        return p->findKey(key, p);
    }
}

template <typename T>
void TreeNode<T>::insertKey(const T &key, int value) {
    
}
