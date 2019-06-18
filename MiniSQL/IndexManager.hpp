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
#include "BPlusTree.hpp"

class IndexManager {
public:
    void createIndex(const std::string &indexName, int type);
    void dropIndex(const std::string &indexName);
    void insertIndex(const std::string &indexName, const element &e, long offset);
    int findIndex(const std::string &indexName);
    void deleteIndex(const std::string &indexName, const element &e);
    void searchRange(const std::string &indexName, element data1, element data2, std::vector<int>& values);
    
private:
    int static const TYPE_FLOAT = 0;
    int static const TYPE_INT = -1;
    
    typedef std::map<std::string, BPlusTree<int> *> intMap;
    typedef std::map<std::string, BPlusTree<float> *> floatMap;
    typedef std::map<std::string, BPlusTree<std::string> *> stringMap;
    std::map<std::string, BPlusTree<int> *> indexIntMap;
    std::map<std::string, BPlusTree<float> *> indexFloatMap;
    std::map<std::string, BPlusTree<std::string> *> indexStringMap;
    
};

#endif /* IndexManager_hpp */
