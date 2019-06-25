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
    //输入：表名，属性，主键，索引
    //功能：在数据库中插入一个表的元信息
    //异常：由底层处理
    //如果已经有相同表名的表存在，则抛出table_exist异常
    void createTable(const std::string &tableName, const std::vector<AttributeType> &data, int primaryKey);
    
    //输入：表名
    //功能：在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
    //异常：由底层处理
    //如果表不存在，抛出table_not_exist异常
    void dropTable(const std::string &tableName);
    
    //输入：表名，索引名，属性名
    //功能：在数据库中更新对应表的索引信息（在指定属性上建立一个索引）
    //异常：由底层处理
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性已经有了索引，抛出index_exist异常
    void createIndex(std::string tableName, std::string indexName, std::string attr_name);
    
    //输入：表名，索引名
    //功能：删除对应表的对应属性上的索引
    //异常：由底层处理
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性没有索引，抛出index_not_exist异常
    void dropIndex(std::string tableName, std::string indexName);
    
    //输入：表名、condition条件属性名、condition条件值域
    //输出：Table类型对象(包含对应的属性元组)
    //功能：返回包含所有目标属性满足condition的记录的表
    //在多条件查询情况下，根据condition下的逻辑条件进行Table的拼接
    //异常：由底层处理
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    Table selectRecord(const std::string &tableName, const std::vector<std::string> targetName,const std::vector<condition> &condition,int op);
    
    //输入：表名、一个元组对象
    //功能：向对应表内插入一条记录
    //异常：由底层处理
    //如果元组类型不匹配，抛出tuple_type_conflict异常
    //如果主键冲突，抛出primary_key_conflict异常
    //如果unique属性冲突，抛出unique_conflict异常
    //如果表不存在，抛出table_not_exist异常
    void insertRecord(const std::string &tableName, Tuple& tuple);
    
    //输入：表名、Where条件属性名、Where条件值域
    //功能：删除对应条件下的Table内记录(不删除表文件)
    //异常：由底层处理
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    void deleteRecord(const std::string &tableName, const std::string targetName,const condition &condition);
    CatalogManager catalogManager;
    RecordManager recordManager;
    IndexManager indexManager;
    //union
    Table unionTable(Table &table1, Table &table2, std::string target_attr, condition where);
    //join
    Table joinTable(Table &table1, Table &table2, std::string target_attr, condition where);
    
};

#endif /* API_hpp */
