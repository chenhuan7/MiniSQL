//
//  RecordManager.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef RecordManager_hpp
#define RecordManager_hpp

#include "MiniSQL.h"
#include"BufferManager.hpp"
#include"RecordManager.hpp"
#include"CatalogManager.hpp"
#include"IndexManager.hpp"
#define INF 10000000
extern BufferManager buffer_manager;

class RecordManager {
public:
    void createTableFile(std::string table_name);
    void dropTableFile(std::string table_name);
    void insertRecord(std::string table_name , Tuple& tuple);
    int deleteRecord(std::string table_name);
    int deleteRecord(std::string table_name , std::string target_attr ,condition where);
    Table selectRecord(std::string table_name , std::string result_table_name = "tmp_table");
    Table selectRecord(std::string table_name , std::string target_attr , condition where , std::string result_table_name = "tmp_table");
    void createIndex(IndexManager& index_manager , std::string table_name , std::string target_attr);
private:
    int getBlockNum(std::string table_name);
    void insertRecord1(char* p , int offset , int len , const std::vector<element>& v);
    char* deleteRecord1(char* p);
    Tuple readTuple(const char* p , std::vector<AttributeType> attr);
    int getTupleLength(char* p);
    bool isConflict(std::vector<Tuple>& tuples , std::vector<element>& v , int index);
    void searchWithIndex(std::string table_name , std::string target_attr , condition where , std::vector<int>& block_ids);
    int conditionDeleteInBlock(std::string table_name , int block_id , std::vector<AttributeType> attr , int index , condition where);
    void conditionSelectInBlock(std::string table_name , int block_id , std::vector<AttributeType> attr , int index , condition where , std::vector<Tuple>& v);
};
template <typename T>
int getDataLength(T data) {
    std::stringstream stream;
    stream << data;
    return stream.str().length();
    
}

template <typename T>
void copyString(char* p , int& offset , T data) {
    std::stringstream stream;
    stream << data;
    std::string s1 = stream.str();
    for (int i = 0;i < s1.length();i++,offset++)
        p[offset] = s1[i];
}

template <typename T>
bool isSatisfied(T a , T b , CONDITION relation) {
    switch(relation) {
        case LESS:{
            if (a < b)
                return true;
            else
                return false;
        };break;
        case LESS_OR_EQUAL:{
            if (a <= b)
                return true;
            else
                return false;
        };break;
        case EQUAL:{
            if (a == b)
                return true;
            else
                return false;
        };break;
        case MORE_OR_EQUAL:{
            if (a >= b)
                return true;
            else
                return false;
        };break;
        case MORE:{
            if (a > b)
                return true;
            else
                return false;
        };break;
        case NOT_EQUAL:{
            if (a != b)
                return true;
            else
                return false;
        };break;
    }
}

#endif



