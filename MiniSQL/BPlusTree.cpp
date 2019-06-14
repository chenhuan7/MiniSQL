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
    p->insertKey(key, value);
    if (p->keys.size() == degree)
        adjustAfterinsert(p, key);
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
    if(keys[left]<key)
        return left+1;
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
TreeNode<T>* TreeNode<T>::split(T key) {
    TreeNode *newNode = new TreeNode(this->parent, this->isLeaf);
    int Min=(degree-1)/2;
    if (!isLeaf) {
        key = keys[Min];
        for (int i=0; i<degree-Min; ++i) {
            newNode->childs.push_back(this->childs[i+Min+1]);
            newNode->childs[i]->parent = newNode;
        }
        for (int i=0; i<degree-Min-1; ++i)
            newNode->keys.push_back(this->keys[i+Min+1]);
        for (int i=0; i<degree-Min; ++i) {
            this->childs.pop_back();
            this->keys.pop_back();
        }
    }
    else {
        key = keys[Min + 1];
        for (int i=0; i<degree-Min-1; ++i) {
            newNode->keys.push_back(keys[i-Min-1]);
            newNode->values.push_back(values[i-Min-1]);
        }
        for (int i=0; i<degree-Min-1; ++i) {
            this->keys.pop_back();
            this->values.pop_back();
        }
        newNode->nextLeafNode = this->nextLeafNode;
        this->nextLeafNode = newNode;
    }
    return newNode;
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
    TreeNode<T> *newNode = pNode->split(key);
    if (pNode == root) {
        TreeNode<T> *newRoot = new TreeNode<T>(NULL);
        this->root = newRoot;
        pNode->parent=newRoot;
        newNode->parent=newRoot;
        newRoot->insertKey(key);
        newRoot->childs.push_back(pNode);
        newRoot->childs.push_back(newNode);
        return true;
    }
    TreeNode<T> *parent = pNode->parent;
    int index = parent->getKeyIndex(key);
    parent->insert(index, key, newNode);
    if (parent->keys.size() == degree) {
        return adjustAfterinsert(parent);
    }
    return true;
}

template <typename T>
bool TreeNode<T>::deleteKey(const T &key) {
    
}
