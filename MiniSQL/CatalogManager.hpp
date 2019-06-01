//
//  CatalogManager.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef CatalogManager_hpp
#define CatalogManager_hpp

#include "MiniSQL.h"

class CatalogManager {
public:
    Table createTable(const std::string &tableName, const std::vector<AttributeType> &data);
    bool dropTable(const std::string &tableName);
    bool createIndex(const std::string &tableName, const std::string &indexName, int attributeIndex);
    bool dropIndex(const std::string &indexName);
};

#endif /* CatalogManager_hpp */
