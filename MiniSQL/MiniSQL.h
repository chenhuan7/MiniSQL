//
//  MiniSQL.h
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef MiniSQL_h
#define MiniSQL_h
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

class AttributeType {
public:
    std::string name;
    int type;
    int length;
    bool isUnique;
   // std::set<std::string> indices;
    //bool operator == (const AttributeType &x) const;
};

class Table {
private:
    int size;
    std::string name;
    int block;
public:
    std::vector<AttributeType> data;
    std::vector<Tuple> Tuple;
    Table(const std::string &name, const std::vector<AttributeType> data){
        this->size=0;
        this->name=name;
        this->block=1;
        this->data=data;
        for(int i=0;i<data.size();i++){
            switch(data[i].type){
                case 0:size+=sizeof(int);break;
                case 1:size+=sizeof(float);break;
                case 2:size+=data[i].length;break;
                default:std::cout<<"Type Error"<<std::endl;break;
            }
        }
    }
    Table(){
        
    }
    int indexID(const AttributeType &a){
        for(int i=0;i<data.size();i++){
            if(data[i].name==a.name)
                return i;
        }
        return -1;
    }
    void writeTable(){
        
    }
    
};
class element{
public:
    int intT;
    float floatT;
    std::string stringT;
    int type;
    element(){
        type=-1;
    }
    element(int x){
        intT=x;
        type=0;
    }
    element(double x){
        floatT=x;
        type=1;
    }
    element(float x){
        floatT=x;
        type=1;
    }
    element(std::string x){
        stringT=x;
        type=2;
    }
    element(const char *x){
        stringT=x;
        type=2;
    }
    void printElement() const {
        switch (type) {
            case 0: std::cout << intT; return;
            case 1: std::cout << floatT; return;
            case 2: std::cout << stringT; return;
            default:std::cout << "element error!"<<std::endl; return;
        }
    }
    bool operator < (const element &x) const {
       // assert(type == x.type);
        switch (type) {
            case 0: return intT < x.intT;
            case 1: return floatT < x.floatT;
            case 2: return stringT < x.stringT;
            default: assert(false);
        }
    }
    
    bool operator == (const element &x) const {
       // assert(type == x.type);
        switch (type) {
            case 0: return intT == x.intT;
            case 1: return fabs(floatT - x.floatT) < 1e-8;
            case 2: return stringT == x.stringT;
            default: assert(false);
        }
    }
    
    bool operator > (const element &x) const {
        return !(*this < x || *this == x);
    }
    
    bool operator >= (const element &x) const {
        return *this > x || *this == x;
    }
    
    bool operator <= (const element &x) const {
        return *this < x || *this == x;
    }
    
    bool operator != (const element &x) const {
        return !(*this == x);
    }
};
class Tuple{
private:
    std::vector<element> data;
    bool isDeleted;
public:
    Tuple() : isDeleted(false) {};
    Tuple(const Tuple &tuple_in);  //拷贝元组
    void addData(element data_in);  //新增元组
    std::vector<element> getData() const;  //返回数据
    int getSize();  //返回元组的数据数量
    bool isDel();
    void setDeleted();
    void showTuple();  //显示元组中的所有数据
};

typedef enum{
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    MORE_OR_EQUAL,
    MORE,
    NOT_EQUAL
} CONDITION;

struct condition{
    element e;
    CONDITION relation;
};

struct Index{
    int num;  //index的总数
    short location[10];  //每个index在Attribute的name数组中是第几个
    std::string indexname[10];  //每个index的名字
};
#endif /* MiniSQL_h */

