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
#endif /* Interpreter_hpp */
