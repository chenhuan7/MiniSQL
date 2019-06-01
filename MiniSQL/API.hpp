//
//  API.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef API_hpp
#define API_hpp

#include "MiniSQL.h"
#include "CatalogManager.hpp"
#include "RecordManager.hpp"
#include "IndexManager.hpp"

class API{
public:
    void createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey);
    void dropTable(const std::string &tableName);
    void createIndex(const std::string &tableName, const std::string &indexName, const std::string &attributeName);
    void dropIndex(const std::string &indexName);
    void selectRecord(const std::string &tableName, const std::vector<std::string> &condition);
    void insertRecord(const std::string &tableName, const std::vector<std::string> &value);
    void deleteRecord(const std::string &tableName, const std::vector<std::string> &condition);
    CatalogManager catalogManager;
    RecordManager recordManager;
    IndexManager indexManager;
    
private:
    
};

#endif /* API_hpp */
