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
    if (!root) {
        root = new TreeNode<T>;
    }
    TreeNode<T> *p=root;
    if (root->findKey(key, p)) {
        return false;
    }
    if (root->keys.size() >= degree) {
        TreeNode<T> *newNode = new TreeNode<T>(root);
        root->parent = newNode;
        newNode->childs.push_back(root);
        root->split(0);
        root=newNode;
    }
    p->insertKey(key, value);
    return true;
}

template <typename T>
TreeNode<T>::TreeNode(TreeNode *Parent, bool Leaf): parent(Parent), isLeaf(Leaf), degree(0), nextLeaf(NULL)
{
    keys.clear();
    values.clear();
    childs.clear();
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
bool TreeNode<T>::findKey(const T &key, TreeNode *p) {
    if (!keys.size()) {
        return false;
    }
    if (keys[0]>key || keys[keys.size()-1]<key){
        return false;
    }
    int index = getKeyIndex(key);
    if (keys[index] == key) {
        return true;
    }
    if (isLeaf)
        return false;
    return childs[index]->findKey(key, p);
}

template <typename T>
void TreeNode<T>::split(int childIndex) {
    TreeNode *newNode = new TreeNode(this);
    int min=(degree-1)/2;
    for (int i=0; i<min; ++i)
        newNode->insertKey(keys[i+min], childs[i+min]);
    for (int i=0; i<min; ++i) {
        keys.pop_back();
        childs.pop_back();
    }
    parent->insert(childIndex+1, newNode->keys[0], newNode);
}

template <typename T>
void TreeNode<T>::insert(int childIndex,const T &key, TreeNode *childNode) {
    keys.insert(keys.begin()+childIndex, key);
    childs.insert(childs.begin()+childIndex, childNode);
}

//template <typename T>
//void TreeNode<T>::mergeChild() {
//    insert(<#int childIndex#>, <#const T &key#>, <#TreeNode<T> *childNode#>)
//}

template <typename T>
bool TreeNode<T>::insertKey(const T &key, int value) {
    if (isLeaf){
        if (!keys.size()){
            keys.push_back(key);
            values.push_back(value);
            return true;
        }
        if (key>keys[keys.size()-1]) {
            keys.push_back(key);
            values.push_back(value);
            return true;
        }
        int index = getKeyIndex(key);
        keys.insert(keys.begin()+index, key);
        values.insert(keys.begin()+index, value);
        return true;
    }
    return false;
}

template <typename T>
bool TreeNode<T>::insertKey(const T &key) {
    if (!keys.size()) {
        keys.push_back(key);
        return true;
    }
    index = getKeyIndex(key);
    keys.insert(keys.begin()+index, key);
    childs.insert(childs.begin()+index+1, NULL);
}

template <typename T>
bool BPlusTree<T>::adjustAfterinsert(TreeNode<T> *pNode) {
    T key;
    TreeNode<T> *newNode = pNode->split(0);
}

template <typename T>
bool TreeNode<T>::deleteKey(const T &key) {
    
}
