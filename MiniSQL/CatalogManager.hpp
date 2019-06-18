//
//  CatalogManager.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef CatalogManager_hpp
#define CatalogManager_hpp

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include "BufferManager.hpp"
#include "MiniSQL.h"

// 每一页的大小为4KB
#define PAGESIZE 4096
// 最大页数为100
#define MAXFRAMESIZE 100

#ifndef TABLE_MANAGER_PATH
#define TABLE_MANAGER_PATH "./database/catalog/catalog_file"
#endif

class CatalogManager {
public:
    //初始化函数
    CatalogManager();
    //输入：表名，属性对象，主键编号，索引对象(暂无)
    //输出：void
    //功能：在catalog文件中插入一个表的元信息。成功返回true，失败返回false（表已经存在）
    //异常：如果已经有相同表名的表存在，则抛出table_exist异常
    bool createTable(std::string tableName, const std::vector<AttributeType> &data, int primary);
    //输入：表名
    //输出：void
    //功能：在catalog文件中删除一个表的元信息，成功返回true，失败返回false（表不存在）
    bool dropTable(std::string tableName);
    //输入：表名
    //输出：bool
    //功能：查找对应表是否存在，存在返回true，不存在返回false
    bool hasTable(std::string table_name);

    //输入：表名
    //输出：属性对象
    //功能：获取一个表的属性
    //异常：如果表不存在，抛出table_not_exist异常
    bool getAttribute(std::string tableName,  std::vector<AttributeType> &data);

    //输入：表名，属性名
    //输出：bool
    //功能：查找对应表中是否有某一属性，如果有返回true，如果没有返回false
    //异常：如果表不存在，抛出table_not_exist异常
    bool hasAttribute(std::string tableName , std::string attrName);

    

    /* Table* createTable(const std::string &tableName, const std::vector<AttributeType> &data);
    bool dropTable(const std::string &tableName);
    bool createIndex(const std::string &tableName, const std::string &indexName, int attributeIndex);
    bool dropIndex(const std::string &indexName);
    
    bool hasTable(const std::string &tableName);
    Table* loadTable(const std::string &tableName);
    bool hasIndex(const std::string &indexName);
    //bool createIndex(const std::string &tableName, const std::string &indexName, int attrIndex);
    std::pair<std::string, int> getIndex(const std::string &indexName);
    //IndexTableManager IM;*/
private:
    int blockNum;
    bool checkTableName(std::string buffer, std::string tableName);
    int str2num(std::string str);
    std::string num2str(int num,short bit);
};

#endif /* CatalogManager_hpp */


/* 

    //输入：表名
    //输出：属性对象
    //功能：获取一个表的属性
    //异常：如果表不存在，抛出table_not_exist异常
    Attribute getAttribute(std::string table_name);
    //输入：表名，属性名，索引名
    //输出：void
    //功能：在catalog文件中更新对应表的索引信息（在指定属性上建立一个索引）
    //异常：如果表不存在，抛出table_not_exist异常。如果对应属性不存在，抛出attribute_not_exist异常。
    //如果对应属性已经有了索引，抛出index_exist异常。
    void createIndex(std::string table_name,std::string attr_name,std::string index_name);
    //输入：表名，索引名
    //输出：索引所对应的属性名
    //功能：通过索引名定位属性名
    //异常：如果表不存在，抛出table_not_exist异常。如果对应索引不存在，抛出index_not_exist异常。
    std::string IndextoAttr(std::string table_name,std::string index_name);
    //输入：表名，索引名
    //输出：void
    //功能：删除对应表的对应属性上的索引
    //异常：如果表不存在，抛出table_not_exist异常。如果对应属性不存在，抛出attribute_not_exist异常。
    //如果对应属性没有索引，抛出index_not_exist异常。
    void dropIndex(std::string table_name,std::string index_name);
    //输入：表名
    //输出：void
    //功能：显示表的信息
    //异常：如果表不存在，抛出table_not_exist异常
    void showTable(std::string table_name);
private:
    //数字转字符串，bit为数的位数
    std::string num2str(int num,short bit);
    //字符串转数字
    int str2num(std::string str);
    //得到该行的表的名字
    std::string getTableName(std::string buffer,int start,int &rear);
    //返回表在文件中的位置,返回具体位置，引用传出数据所在的块信息
    int getTablePlace(std::string name,int &suitable_block);
    //返回该表的index
    Index getIndex(std::string table_name);
    //获取文件大小
    int getBlockNum(std::string table_name);
};

#endif /* catalog_hpp */ 
