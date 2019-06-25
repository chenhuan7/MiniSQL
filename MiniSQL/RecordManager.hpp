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
    //输入：表名
    //功能：建立表文件
    void createTableFile(std::string table_name);
    
    //输入：表名
    //功能：删除表文件
    void dropTableFile(std::string table_name);
    
    //输入：表名，一个元组
    //功能：向对应表中插入一条记录
    //异常：如果元组类型不匹配，抛出tuple_type_conflict异常。如果主键冲突，抛出primary_key_conflict异常。如果unique属性冲突，抛出unique_conflict异常。如果表不存在，抛出table_not_exist异常。
    void insertRecord(std::string table_name , Tuple& tuple);
    
    //输入：表名
    //输出：删除的记录数
    //功能：删除对应表中所有记录（不删除表文件）
    //异常：如果表不存在，抛出table_not_exist异常
    int deleteRecord(std::string table_name);
    
    //输入：表名，目标属性，condition
    //输出：删除的记录数
    //功能：删除对应表中所有目标属性值满足condition的记录
    //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
    int deleteRecord(std::string table_name , std::string target_attr ,condition where);
    
    //输入：表名
    //输出：Table类型对象
    //功能：返回整张表
    //异常：如果表不存在，抛出table_not_exist异常
    Table selectRecord(std::string table_name , std::string result_table_name = "tmp_table");
    
    //输入：表名，目标属性，condition
    //输出：Table类型对象
    //功能：返回包含所有目标属性满足condition的记录的表
    //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常。
    Table selectRecord(std::string table_name , std::string target_attr , condition where , std::string result_table_name = "tmp_table");
    
    //输入：表名，目标属性名
    //功能：对表中已经存在的记录建立索引
    //异常：如果表不存在，抛出table_not_exist异常。如果属性不存在，抛出attribute_not_exist异常。
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



