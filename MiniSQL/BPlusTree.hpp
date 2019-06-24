//
//  BPlusTree.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "MiniSQL.h"
#include "BufferManager.hpp"

// 每一页的大小为4KB
#define PAGESIZE 4096
// 最大页数为100
#define MAXFRAMESIZE 100

extern BufferManager buffer_manager;

template <typename T>
class TreeNode {
public:
    TreeNode *parent;
    std::vector<T> keys;
    std::vector<TreeNode*> childs;
    std::vector<int> values;
    TreeNode *nextLeaf;
    bool isLeaf;
    
    TreeNode(TreeNode *Parent=NULL, bool Leaf=false);
    void clear();
    bool insertKey(const T &key, int value);
    bool insertKey(const T &key);
    void insert(int childIndex,const T &key, TreeNode *childNode);
    TreeNode<T>* findKey(const T &key, int &index);
    TreeNode<T>* split(T key);
    int getKeyIndex(const T &key);
    
private:
    int degree;
};

template <typename T>
class BPlusTree {
public:
    BPlusTree(std::string Name, int Size, int Degree);
    ~BPlusTree();
    bool insertKey(const T key, int value);
    bool deleteKey(const T key);
    int searchVal(const T key);
    void searchRange(T data1, T data2, std::vector<int> &vals);
    
private:
    typedef TreeNode<T>* Tree;
    std::string fileName;
    int keySize;
    int degree;
    Tree root;
    
    void readValues();
    bool adjustAfterinsert(TreeNode<T> *pNode);
    int getBlockNum(std::string tableName);
};

//
//  BPlusTree.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//


template <typename T>
BPlusTree<T>::BPlusTree(std::string Name, int Size, int Degree):
fileName(Name), keySize(Size), degree(Degree), root(NULL)
{
    root = new TreeNode<T>(NULL);
    readValues();
}

template <typename T>
bool BPlusTree<T>::insertKey(const T key, int value) {
    if (!root) {
        root = new TreeNode<T>;
    }
    int index;
    TreeNode<T> *p = root->findKey(key, index);
    if (!p) {
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
TreeNode<T>* TreeNode<T>::findKey(const T &key, int &index) {
    if (!keys.size()) {
        return this;
    }
    if (keys[0]>key || keys[keys.size()-1]<key){
        return this;
    }
    index = getKeyIndex(key);
    if (keys[index] == key) {
        return this;
    }
    if (isLeaf)
        return this;
    return childs[index]->findKey(key, index);
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
bool BPlusTree<T>::deleteKey(const T key) {
    int index;
    TreeNode<T> *p=root->findKey(key, index);
    if (!p)
        return false;
    if (p->keys[index] != key)
        return false;
    p->keys[index] = -1;
    return true;
}

template <typename T>
int BPlusTree<T>::searchVal(const T key) {
    int index;
    TreeNode<T> *p=root->findKey(key, index);
    if (!p) {
        return -1;
    }
    if (p->keys[index] != key) {
        return -1;
    }
    return p->values[index];
}

template <typename T>
void BPlusTree<T>::searchRange(T data1, T data2, std::vector<int> &vals) {
    int stIndex, edIndex;
    TreeNode<T> *pNode=root->findKey(data1, stIndex);
    TreeNode<T> *edNode=root->findKey(data2, edIndex);
    while (pNode!=edNode) {
        for (int i=stIndex; i<pNode->values.size(); ++i)
            vals.push_back(pNode->values[i]);
        pNode = pNode->nextLeaf;
        stIndex=0;
    }
    for (int i=stIndex; i<=edIndex; ++i)
        vals.push_back(pNode->values[i]);
    return;
}

template <typename T>
void BPlusTree<T>::readValues() {
    std::string fname = "./database/index/" + fileName;
    int block_num = getBlockNum(fname);
    
    if (block_num <= 0)
        block_num = 1;
    
    for (int i = 0; i < block_num; i++) {
        char* p = buffer_manager.getPage(fname, i);
        readValues();
    }
}

template <typename T>
int BPlusTree<T>::getBlockNum(std::string tableName) {
    char* p;
    int block_num = -1;
    p = buffer_manager.getPage(tableName , block_num + 1);
    while(p[0] != '\0') {
        block_num++;
        p = buffer_manager.getPage(tableName , block_num + 1);
    };
    return block_num;
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
    std::string fname = "./database/index/" + fileName;
    int block_num = getBlockNum(fname);
    
    Tree ntmp = root;
    while (!ntmp->isLeaf)
        ntmp = ntmp->childs[0];
    int i, j;
    
    for (j = 0, i = 0; ntmp != NULL; j++) {
        char* p = buffer_manager.getPage(fname, j);
        int offset = 0;
        
        memset(p, 0, PAGESIZE);
        
        for (i = 0; i < ntmp->keys.size(); i++) {
            p[offset++] = '#';
            p[offset++] = ' ';
            
            std::stringstream stream;
            stream << ntmp->keys[i];
            std::string s1 = stream.str();
            for (int i = 0;i < s1.length();i++,offset++)
                p[offset] = s1[i];
            p[offset++] = ' ';
            
            stream << ntmp->values[i];
            s1 = stream.str();
            for (int i = 0;i < s1.length();i++,offset++)
                p[offset] = s1[i];
            p[offset++] = ' ';
        }
        
        p[offset] = '\0';
        
        int page_id = buffer_manager.getPageId(fname, j);
        buffer_manager.modifyPage(page_id);
        
        ntmp = ntmp->nextLeaf;
    }
    
    while (j < block_num) {
        char* p = buffer_manager.getPage(fname, j);
        memset(p, 0, PAGESIZE);
        
        int page_id = buffer_manager.getPageId(fname, j);
        buffer_manager.modifyPage(page_id);
        
        j++;
    }
    
    return;
}
