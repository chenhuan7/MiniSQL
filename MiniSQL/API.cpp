//
//  API.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "API.hpp"
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
bool isSatisfied(Tuple& tuple, int target_attr, condition where);
void API::createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey) {
    recordManager.createTableFile(tableName);
    catalogManager.createTable(tableName, data, primaryKey);
}
void API::dropTable(const std::string &tableName) {
    //    if (API::catalogManager.haveTable(tableName))
    recordManager.dropTableFile(tableName);
    catalogManager.dropTable(tableName);
}

void API::createIndex(std::string tableName, std::string indexName, std::string attributeName) {
    //    if (API::catalogManager.haveTable(tableName))
    IndexManager index;
    std::string file_path = "INDEX_FILE_" + attributeName + "_" + tableName;
    int type;
    
    catalogManager.createIndex(tableName, attributeName, index_name);
    std::vector<AttributeType>attr;
    catalogManager.getAttribute(tableName,attr);
    for (int i = 0; i < attr.size(); i++) {
        if (attr[i].name == attributeName) {
            type = (int)attr[i].type;
            break;
        }
    }
    IndexManager.createIndex(file_path, type);
    recordManager.createIndex(index , tableName, attributeName);
    int id = -1;
    //API::catalogManager.createIndex(tableName, indexName, id);
}

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
    IndexManager.dropIndex(file_path, type);
    catalogManager.dropIndex(tableName, indexName);
    
    file_path = "./database/index/" + file_path;
    remove(file_path.c_str());
}

Table API::selectRecord(const std::string &tableName, const std::vector<std::string> targetName,const std::vector<std::string> &condition,int op){
    if(targetName.size()==0){
        return recordManager.selectRecord(tableName);
    }else if(targetName.size()==1){
        recordManager.selectRecord(tableName, targetName[0], condition[0]);
        return recordManager.selectRecord(tableName, targetName[0], condition[0]);
    }else{
        Table table1 = record.selectRecord(tableName, targetName[0], condition[0]);
        Table table2 = record.selectRecord(tableName, targetName[1], condition[1]);
        
        if (op)
            return joinTable(table1, table2, targetName[0], condition[0]);
        else
            return unionTable(table1, table2, targetName[0], condition[0]);
    }
}

void API::insertRecord(const std::string &tableName, Tuple& tuple){
    recordManager.insertRecord(tableName, tuple);
}

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
