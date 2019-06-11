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
    root->clear();
}

template <typename T>
bool BPlusTree<T>::insertKey(const T &key, int value) {
    Tree p = NULL;
    if (root->findKey(key, p)) {
        return false;
    }
    p->insertKey(key, value);
    if (p->keyNum == degree) {
        
    }
    return true;
}

template <typename T>
void TreeNode<T>::clear() {
    while (childs.size()) {
        childs.back()->clear();
        delete childs.back();
        childs.pop_back();
    }
}

template <typename T>
int TreeNode<T>::getKeyIndex(const T &key) {
    int left=0, right=keys.size()-1;
    while (left < right) {
        int pnow = (left+right)/2;
        if (keys[pnow] < key)
            left = pnow+1;
        else
            right = pnow-1;
    }
    return left;
}

template <typename T>
TreeNode<T>::TreeNode(bool Leaf): keyNum(0), parent(NULL), isLeaf(Leaf), degree(0), nextLeaf(NULL)
{
    keys.clear();
    values.clear();
    childs.clear();
}

template <typename T>
bool TreeNode<T>::findKey(const T &key, TreeNode *p) {
    if (!keyNum) {
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
        return true;
    }
    else {
        p = childs[pnow];
        return p->findKey(key, p);
    }
}

template <typename T>
bool TreeNode<T>::insertKey(const T &key, int value) {
    if (!isLeaf)
        return false;
    if (!keys.size()){
        keys.push_back(key);
        values.push_back(value);
        keyNum = keys.size();
        return true;
    }
    
    if (key>keys[keyNum-1]) {
        keys.push_back(key);
        values.push_back(value);
        keyNum = keys.size();
        return true;
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
    keys.insert(keys.begin()+pnow, key);
    values.insert(keys.begin()+pnow, value);
    keyNum = keys.size();
    return true;
}
