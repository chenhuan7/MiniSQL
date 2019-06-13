//
//  IndexManager.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//
#include "IndexManager.hpp"
using namespace std;
bool isExist(const string &fileName) {
    ifstream f(fileName.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}
void IndexManager::createIndex(const std::string &indexName, const std::string &dbName, Table &table, const AttributeType &attr){
    
}
void IndexManager::dropIndex(const std::string &indexName){
    
}
