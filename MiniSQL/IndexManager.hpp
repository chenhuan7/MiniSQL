//
//  IndexManager.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef IndexManager_hpp
#define IndexManager_hpp

#include "MiniSQL.h"

class IndexManager {
    void createIndex(const std::string &indexName, const std::string &dbName, Table &table, const AttributeType &attr);
    void dropIndex(const std::string &indexName);
    void insertIndex(const std::string &indexName, const element &e, long offset);
    
};

#endif /* IndexManager_hpp */
