//
//  API.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "API.hpp"
using namespace std;

void API::createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey) {
    if (API::catalogManager.hasTable(tableName)) {
        std::cout << "table already exits" << std::endl;
        return;
    }
    
    Table newTable = API::catalogManager.createTable(tableName, data);
}
void API::dropTable(const std::string &tableName) {
    if (!API::catalogManager.hasTable(tableName)){
        std::cout << "table does not exit" << std::endl;
        return;
    }
    
    API::catalogManager.dropTable(tableName);
}

void API::createIndex(const std::string &tableName, const std::string &indexName, const std::string &attributeName) {
//    if (API::catalogManager.hasTable(tableName))
//        std::cout << "" << std::endl;
    
    int id = -1;
    API::catalogManager.createIndex(tableName, indexName, id);
}

void API::dropIndex(const std::string &indexName) {
    
}

void API::selectRecord(const std::string &tableName, const std::vector<std::string> &condition){
    
}

void API::insertRecord(const std::string &tableName, const std::vector<std::string> &value){
    
}

void API::deleteRecord(const std::string &tableName, const std::vector<std::string> &condition){
    
}
