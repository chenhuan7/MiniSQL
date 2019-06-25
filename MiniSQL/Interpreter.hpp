//
//  Interpreter.hpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "API.hpp"
#include "CatalogManager.hpp"
#include "MiniSQL.h"

class Interpreter{
public:
    Interpreter();
    ~Interpreter();
    void query();
    void read();
    //功能：进行选择操作，支持单表多约束
    //异常：格式错误则抛出input_format_error
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    void selectRecord(int i);
    
    //功能：从表中删除相应的元组
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果属性不存在，抛出attribute_not_exist异常
    //如果Where条件中的两个数据类型不匹配，抛出data_type_conflict异常
    void deleteRecord(int i);
    
    //功能：删除表
    //异常：格式错误则抛出input_format_error
    //如果表不存在，抛出table_not_exist异常
    void dropTable(int i);
    
    //功能：在表中删除一个索引
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性没有索引，抛出index_not_exist异常
    void dropIndex(int i);
    
    //功能：在数据库中插入一个表的元信息
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    void createTable(int i);
    
    //功能：在表中插入一个索引，其对应属性为XX
    //异常：格式错误则抛出input_format_error异常
    //如果表不存在，抛出table_not_exist异常
    //如果对应属性不存在，抛出attribute_not_exist异常
    //如果对应属性已经有了索引，抛出index_exist异常
    void createIndex(int i);
    
    //功能：向T1内插入值的信息
    void insertRecord(int i);
    
    //输入：execfile 文件路径
    //功能：根据文件路径读取文件信息，并用于数据库的操作
    void execFile(int i);
    
    //输入：exit;
    //功能：退出数据库
    void exitMiniSQL();
    
private:
    std::string queryString;
    std::string getWord(int &i);
    int getType(int &i);//获得类型
    int getBits(int num);//获得长度
    int getBits(float num);
    
};
//类型转换
template <class Type>
Type strToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

#endif /* Interpreter_hpp */
