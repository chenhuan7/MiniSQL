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

class AttributeType {
public:
    std::string name;
    int type;
    int length;
    bool isUnique;
   // std::set<std::string> indices;
    //bool operator == (const AttributeType &rhs) const;
};

class Table {
private:
    int size;
    std::string name;
    std::vector<AttributeType> data;
    int block;
public:
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



#endif /* MiniSQL_h */
