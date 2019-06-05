//
//  CatalogManager.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "CatalogManager.hpp"
#include "MiniSQL.h"
Table CatalogManager::createTable(const std::string &tableName, const std::vector<AttributeType> &data){
    if(hasTable(tableName)){
        std::cout<<"already created!"<<std::endl;
    }
    else{
        Table newTable(tableName,data);
        
    }
    
}
bool CatalogManager::dropTable(const std::string &tableName){
    
}
bool CatalogManager::createIndex(const std::string &tableName, const std::string &indexName, int attributeIndex){
    
}
bool CatalogManager::dropIndex(const std::string &indexName){
    
}
//验证是否存在表
bool CatalogManager::hasTable(const std::string &tableName){
    std::ifstream fin((tableName).c_str());
    std::string header;
    if (!fin) return false;
    
    fin >> header;
    fin.close();
    if(tableName == header)
        return true;
    else
        return false;
}
Table CatalogManager::loadTable(const std::string &tableName){
    
}
bool CatalogManager::hasIndex(const std::string &indexName){
    
}
//bool createIndex(const std::string &tableName, const std::string &indexName, int attrIndex);
std::pair<std::string, int> getIndex(const std::string &indexName);
//IndexTableManager IM;
