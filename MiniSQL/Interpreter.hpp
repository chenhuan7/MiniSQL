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
    void selectRecord(int i);
    void deleteRecord(int i);
    void dropTable(int i);//
    void dropIndex(int i);//
    void createTable(int i);//
    void createIndex(int i);//
    void insertRecord(int i);//
    void execFile(int i);//
    void exitMiniSQL();//
    
private:
    std::string queryString;
    std::string getWord(int &i);
    int getType(int &i);
    
};
template <class Type>
Type strToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

int Interpreter::getBits(int num){
    int bit=0;
    if(num==0)
        return 1;
    if(num<0){
        bit++;
        num=-num;
    }
    while(num!=0){
        num/=10;
        bit++;
    }
    return bit;
}

//根据除法得到小数的数字的长度
int Interpreter::getBits(float num){
    int bit=0;
    if((int)num==0)
        return 4;
    if(num<0){
        bit++;
        num=-num;
    }
    int integer_part=num;
    while(integer_part!=0){
        bit++;
        integer_part/=10;
    }
    return bit+3;//为了保留小数点的后几位
}
#endif /* Interpreter_hpp */
