//
//  API.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "API.hpp"
API::API(){}
API::~API(){}
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
bool isSatisfied(Tuple& tuple, int target_attr, condition where);
//输入：表名，属性，主键，索引
//功能：在数据库中插入一个表的元信息
//异常：由底层处理
//如果已经有相同表名的表存在，则抛出table_exist异常
void API::createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey) {
    recordManager.createTableFile(tableName);
    catalogManager.createTable(tableName, data, primaryKey);
}
//输入：表名
//功能：在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
void API::dropTable(const std::string &tableName) {
    //    if (API::catalogManager.haveTable(tableName))
    recordManager.dropTableFile(tableName);
    catalogManager.dropTable(tableName);
}
//输入：表名，索引名，属性名
//功能：在数据库中更新对应表的索引信息（在指定属性上建立一个索引）
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果对应属性不存在，抛出attribute_not_exist异常
//如果对应属性已经有了索引，抛出index_exist异常
void API::createIndex(std::string tableName, std::string indexName, std::string attributeName) {
    //    if (API::catalogManager.haveTable(tableName))
    IndexManager index;
    std::string file_path = "INDEX_FILE_" + attributeName + "_" + tableName;
    int type;
    
    catalogManager.createIndex(tableName, attributeName, indexName);
    std::vector<AttributeType>attr;
    catalogManager.getAttribute(tableName,attr);
    for (int i = 0; i < attr.size(); i++) {
        if (attr[i].name == attributeName) {
            type = (int)attr[i].type;
            break;
        }
    }
    index.createIndex(file_path, type);
    
    recordManager.createIndex(index , tableName, attributeName);
    int id = -1;
    //API::catalogManager.createIndex(tableName, indexName, id);
}
//输入：表名，索引名
//功能：删除对应表的对应属性上的索引
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果对应属性不存在，抛出attribute_not_exist异常
//如果对应属性没有索引，抛出index_not_exist异常
void API::dropIndex(std::string tableName, std::string indexName) {
    IndexManager index;
    
    std::string attr_name = catalogManager.IndextoAttr(tableName, indexName);
    std::string file_path = "INDEX_FILE_" + attr_name + "_" + tableName;
    int type;
    
    std::vector<AttributeType>attr;
    catalogManager.getAttribute(tableName,attr);
    for (int i = 0; i < attr.size(); i++) {
        if (attr[i].name == attr_name) {
            type = (int)attr[i].type;
            break;
        }
    }
    index.dropIndex(file_path);
    catalogManager.dropIndex(tableName, indexName);
    
    file_path = "./database/index/" + file_path;
    remove(file_path.c_str());
}
//输入：表名、condition条件属性名、condition条件值域
//输出：Table类型对象(包含对应的属性元组)
//功能：返回包含所有目标属性满足condition的记录的表
//在多条件查询情况下，根据condition下的逻辑条件进行Table的拼接
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果属性不存在，抛出attribute_not_exist异常
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
Table API::selectRecord(const std::string &tableName, const std::vector<std::string> targetName,const std::vector<condition> &condition,int op){
    if(targetName.size()==0){
        return recordManager.selectRecord(tableName);
    }else if(targetName.size()==1){
        recordManager.selectRecord(tableName, targetName[0], condition[0]);
        return recordManager.selectRecord(tableName, targetName[0], condition[0]);
    }else{
        Table table1 = recordManager.selectRecord(tableName, targetName[0], condition[0]);
        Table table2 = recordManager.selectRecord(tableName, targetName[1], condition[1]);
        
        if (op)
            return joinTable(table1, table2, targetName[0], condition[0]);
        else
            return unionTable(table1, table2, targetName[0], condition[0]);
    }
}
//输入：表名、一个元组对象
//功能：向对应表内插入一条记录
//异常：由底层处理
//如果元组类型不匹配，抛出tuple_type_conflict异常
//如果主键冲突，抛出primary_key_conflict异常
//如果unique属性冲突，抛出unique_conflict异常
//如果表不存在，抛出table_not_exist异常
void API::insertRecord(const std::string &tableName, Tuple& tuple){
    recordManager.insertRecord(tableName, tuple);
}
//输入：表名、Where条件属性名、Where条件值域
//功能：删除对应条件下的Table内记录(不删除表文件)
//异常：由底层处理
//如果表不存在，抛出table_not_exist异常
//如果属性不存在，抛出attribute_not_exist异常
//如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
void API::deleteRecord(const std::string &tableName, const std::string targetName,const condition &condition){
    int result;
    if (targetName== ""){
        
        result = recordManager.deleteRecord(tableName);
    }
    else
        result = recordManager.deleteRecord(tableName, targetName, condition);
}

Table API::joinTable(Table &table1, Table &table2, std::string target_attr, condition where)
{
    Table result_table(table1);
    std::vector<Tuple>& result_tuple = result_table.Tuple;
    std::vector<Tuple> tuple1 = table1.Tuple;
    std::vector<Tuple> tuple2 = table2.Tuple;
    
    int i;
    std::vector<AttributeType> attr = table1.data;
    for (i = 0; i < 32; i++)
        if (attr[i].name == target_attr)
            break;
    
    for (int j = 0; j < tuple2.size(); j++)
        if (isSatisfied(tuple2[j], i, where))
            result_tuple.push_back(tuple2[j]);
    
    std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
    return result_table;
}

Table API::unionTable(Table &table1, Table &table2, std::string target_attr, condition where)
{
    Table result_table(table1);
    std::vector<Tuple>& result_tuple = result_table.Tuple;
    std::vector<Tuple> tuple1 = table1.Tuple;
    std::vector<Tuple> tuple2 = table2.Tuple;
    result_tuple = tuple1;
    
    //std::vector<Tuple>().swap(result_tuple);
    
    int i;
    std::vector<AttributeType> attr = table1.data;
    for (i = 0; i < 32; i++)
        if (attr[i] .name== target_attr)
            break;
    
    for (int j = 0; j < tuple2.size(); j++)
        if (!isSatisfied(tuple2[j], i, where))
            result_tuple.push_back(tuple2[j]);
    
    std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
    return result_table;
}
//判断是否符合条件
bool isSatisfied(Tuple& tuple, int target_attr, condition where)
{
    std::vector<element> data = tuple.getData();
    
    switch (where.relation) {
        case LESS : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT < where.e.intT); break;
                case 0 : return (data[target_attr].floatT < where.e.floatT); break;
                default: return (data[target_attr].stringT < where.e.stringT); break;
            }
        } break;
        case LESS_OR_EQUAL : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT <= where.e.intT); break;
                case 0 : return (data[target_attr].floatT <= where.e.floatT); break;
                default: return (data[target_attr].stringT <= where.e.stringT); break;
            }
        } break;
        case EQUAL : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT == where.e.intT); break;
                case 0 : return (data[target_attr].floatT == where.e.floatT); break;
                default: return (data[target_attr].stringT == where.e.stringT); break;
            }
        } break;
        case MORE_OR_EQUAL : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT >= where.e.intT); break;
                case 0 : return (data[target_attr].floatT >= where.e.floatT); break;
                default: return (data[target_attr].stringT >= where.e.stringT); break;
            }
        } break;
        case MORE : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT > where.e.intT); break;
                case 0 : return (data[target_attr].floatT > where.e.floatT); break;
                default: return (data[target_attr].stringT > where.e.stringT); break;
            }
        } break;
        case NOT_EQUAL : {
            switch (where.e.type) {
                case -1 : return (data[target_attr].intT != where.e.intT); break;
                case 0 : return (data[target_attr].floatT != where.e.floatT); break;
                default: return (data[target_attr].stringT != where.e.stringT); break;
            }
        } break;
        default:break;
    }
    
    return false;
}
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2)
{
    std::vector<element> data1 = tuple1.getData();
    std::vector<element> data2 = tuple2.getData();
    
    switch (data1[0].type) {
        case -1: return data1[0].intT < data2[0].intT;
        case 0: return data1[0].floatT < data2[0].floatT;
        default: return data1[0].stringT < data2[0].stringT;
    }
}
