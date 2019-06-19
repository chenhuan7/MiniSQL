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
    API();
    ~API();
    
    void createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey);
    void dropTable(const std::string &tableName);
    void createIndex(std::string tableName, std::string indexName, std::string attr_name);
    void dropIndex(std::string tableName, std::string indexName);
    Table selectRecord(const std::string &tableName, const std::vector<std::string> targetName,const std::vector<condition> &condition,int op);
    void insertRecord(const std::string &tableName, Tuple& tuple);
    void deleteRecord(const std::string &tableName, const std::string targetName,const condition &condition);
    CatalogManager catalogManager;
    RecordManager recordManager;
    IndexManager indexManager;
    Table unionTable(Table &table1, Table &table2, std::string target_attr, condition where);
    Table joinTable(Table &table1, Table &table2, std::string target_attr, condition where);

};

#endif /* API_hpp */
