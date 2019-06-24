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
        BPlusTree<int> *tree = new BPlusTree<int>(indexName, sizeof(int), 15);
        indexIntMap.insert(intMap::value_type(indexName, tree));
        return ;
    }
    if (type == TYPE_FLOAT) {
        BPlusTree<float> *tree = new BPlusTree<float>(indexName, sizeof(float), 15);
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

bool IndexManager::insertIndex(const std::string &indexName, const element &e, long offset) {
    if (e.type<-1) {
        return false;
    }
    switch (e.type) {
        case TYPE_INT: {
            intMap::iterator itInt = indexIntMap.find(indexName);
            if (itInt == indexIntMap.end()) {
                return false;
            }
            delete itInt->second;
            indexIntMap.erase(itInt);
            break;
        }
        case TYPE_FLOAT: {
            floatMap::iterator itFloat = indexFloatMap.find(indexName);
            if (itFloat == indexFloatMap.end()) {
                return false;
            }
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
            break;
        }
        default: {
            stringMap::iterator itString = indexStringMap.find(indexName);
            if (itString == indexStringMap.end()) {
                return false;
            }
            delete itString->second;
            indexStringMap.erase(itString);
            break;
        }
    }
    return true;
}

void IndexManager::deleteIndex(const std::string &indexName, const element &e) {
    if (e.type < -1)
        return;
    switch (e.type) {
        case TYPE_INT: {
            intMap::iterator itInt = indexIntMap.find(indexName);
            itInt->second->deleteKey(e.intT);
            break;
        }
        case TYPE_FLOAT: {
            floatMap::iterator itFloat = indexFloatMap.find(indexName);
            itFloat->second->deleteKey(e.floatT);
            break;
        }
        default: {
            stringMap::iterator itString = indexStringMap.find(indexName);
            itString->second->deleteKey(e.stringT);
            break;
        }
    }
    return;
}

int IndexManager::findIndex(const std::string &indexName, element e) {
    if (e.type < -1) {
        return -1;
    }
    switch (e.type) {
        case TYPE_INT:{
            intMap::iterator itInt = indexIntMap.find(indexName);
            if (itInt == indexIntMap.end())
                return -1;
            return itInt->second->searchVal(e.intT);
            break;
        }
        case TYPE_FLOAT:{
            floatMap::iterator itFloat = indexFloatMap.find(indexName);
            if (itFloat == indexFloatMap.end())
                return -1;
            return itFloat->second->searchVal(e.floatT);
            break;
        }
        default:{
            stringMap::iterator itString = indexStringMap.find(indexName);
            if (itString == indexStringMap.end())
                return -1;
            return itString->second->searchVal(e.stringT);
            break;
        }
    }
    return 0;
}

void IndexManager::searchRange(const std::string &indexName, element data1, element data2, std::vector<int> &values) {
    if (data1.type != data2.type)
        return;
    switch (data1.type) {
        case TYPE_INT:{
            intMap::iterator itInt = indexIntMap.find(indexName);
            if (itInt == indexIntMap.end())
                return;
            itInt->second->searchRange(data1.intT, data2.intT, values);
            break;
        }
        case TYPE_FLOAT:{
            floatMap::iterator itFloat = indexFloatMap.find(indexName);
            if (itFloat == indexFloatMap.end())
                return;
            itFloat->second->searchRange(data1.floatT, data2.floatT, values);
            break;
        }
        default:{
            stringMap::iterator itString = indexStringMap.find(indexName);
            if (itString == indexStringMap.end())
                return;
            itString->second->searchRange(data1.stringT, data2.stringT, values);
            break;
        }
    }
    return;
}
