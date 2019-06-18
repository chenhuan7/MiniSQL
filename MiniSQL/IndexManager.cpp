//
//  IndexManager.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//
#include "IndexManager.hpp"

bool isExist(const std::string &fileName) {
    std::ifstream f(fileName.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

void IndexManager::createIndex(const std::string &indexName, int type){
    if (type == TYPE_INT) {
        BPlusTree<int> *tree = new BPlusTree<int>(indexName, sizeof(int), 11);
        indexIntMap.insert(intMap::value_type(indexName, tree));
        return ;
    }
    if (type == TYPE_FLOAT) {
        BPlusTree<float> *tree = new BPlusTree<float>(indexName, sizeof(float), 13);
        indexFloatMap.insert(floatMap::value_type(indexName, tree));
        return ;
    }
    if (type > 0) {
        BPlusTree<std::string> *tree = new BPlusTree<std::string>(indexName, sizeof(char)*type, 15);
        indexStringMap.insert(stringMap::value_type(indexName, tree));
        return ;
    }
    return;
}

void IndexManager::dropIndex(const std::string &indexName){
    intMap::iterator itInt = indexIntMap.find(indexName);
    if (itInt != indexIntMap.end()) {
        delete itInt->second;
        indexIntMap.erase(itInt);
        return;
    }
    floatMap::iterator itFloat = indexFloatMap.find(indexName);
    if (itFloat != indexFloatMap.end()) {
        delete itFloat->second;
        indexFloatMap.erase(itFloat);
        return;
    }
    stringMap::iterator itString = indexStringMap.find(indexName);
    if (itString != indexStringMap.end()) {
        delete itString->second;
        indexStringMap.erase(itString);
        return;
    }
    std::cout << "Error: no such index" << std::endl;
    return;
}

void IndexManager::insertIndex(const std::string &indexName, const element &e, long offset) {
    if(e.type == TYPE_INT) {
        
    }
    return;
}
