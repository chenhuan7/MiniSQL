//
//  Interpreter.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "Interpreter.hpp"

Interpreter::Interpreter(){
    
}
Interpreter::~Interpreter(){
    
}

void Interpreter::read(){
    std::string line;
    do{
        std::cout<<">> ";
        getline(std::cin,line);
        queryString=queryString+line+' ';
    }while(line[line.length()-1]!=';');
    queryString[queryString.length()-2]='\0';
    query();
}

void Interpreter::query(){
    for(int i=0;i<queryString.length();i++){
        if(queryString[i]=='*'||queryString[i]==','||queryString[i]=='('||queryString[i]==')'||queryString[i]=='='||queryString[i]=='<'||queryString[i]=='>'){
            queryString.insert(i," ");
            queryString.insert(i+2," ");
            i=i+2;
        }
    }
    queryString.insert(queryString.length()-2," ");
    transform(queryString.begin(), queryString.end(), queryString.begin(), ::tolower);

    int i=0;
    while(queryString[i++]==' ');
    i--;
    if(queryString.substr(i,6)=="select"){
        selectRecord(i+6);
    }
    else if(queryString.substr(i,6)=="delete"){
        deleteRecord(i+6);
    }
    else if(queryString.substr(i,4)=="drop"){
        i++;
        while(queryString[i++]==' ');
        i--;
        if(queryString.substr(i,5)=="table"){
            dropTable(i+5);
        }
        else if(queryString.substr(i,5)=="index"){
            dropIndex(i+5);
        }
    }
    else if(queryString.substr(i,6)=="create"){
        i++;
        while(queryString[i++]==' ');
        i--;
        if(queryString.substr(i,5)=="table"){
            createTable(i+5);
        }
        else if(queryString.substr(i,5)=="index"){
            createIndex(i+5);
        }
    }
    else if(queryString.substr(i,4)=="quit"){
        exitMiniSQL();
    }
    else if(queryString.substr(i,8)=="execfile"){
        execFile(i+8);
    }
    else{
        //throw inputError();
        std::cout<<"input format error!"<<std::endl;
    }
}

void Interpreter::selectRecord(int i){
    
}

void Interpreter::deleteRecord(int i){
    
}

void Interpreter::dropTable(int i){
    API API;
    std::string tableName;
    tableName=getWord(i);
    API.dropTable(tableName);
}

void Interpreter::dropIndex(int i){
    API API;
    std::string indexName;
    indexName=getWord(i);
    API.dropIndex(indexName);
}

void Interpreter::createTable(int i){
    std::string tableName;
    
}

void Interpreter::createIndex(int i){
    API API;
    CatalogManager CM;
    std::string indexName;
    std::string tableName;
    std::string attributeName;
    
    indexName=getWord(i);
    if(getWord(i)!="on"){
        throw 1;
    }
    tableName=getWord(i);
    if(!CM.hasTable(tableName)){
        std::cout<<"no such table!"<<std::endl;
        throw 1;
    }
    if(getWord(i)!="("){
        throw 1;
    }
    attributeName=getWord(i);
    if(getWord(i)!=")"){
        throw 1;
    }
    //待解决：）后面多东西的报错
    API.createIndex(tableName, indexName, attributeName);
}

void Interpreter::insertRecord(int i){
    API API;
    CatalogManager CM;
    std::vector<AttributeType> data;
    std::string tableName;
    std::string value;
    element e;
    int k;
    if(getWord(i)!="into"){
        throw 1;
    }
    tableName=getWord(i);
    if(getWord(i)!="values"){
        throw 1;
    }
    if(getWord(i)!="("){
        throw 1;
    }
    data=CM.getAttribute(tableName);
    k=-1;
    while(1){
        value=getWord(i);
        if(value==")"){
            break;
        }
        k++;
        
        if(k>=data.size()){
            std::cout<<"attribute not match"<<std::endl;
            throw 1;
        }
        switch(data[k].type){
            case -1:{
                try{
                    e.intT=strToNum<int>(value);
                }catch(...){
                    std::cout<<"convert failed."<<std::endl;
                    throw 1;
                }
                break;
            }
            case 0:{
                try{
                    e.floatT=strToNum<float>(value);
                }catch(...){
                    std::cout<<"convert failed."<<std::endl;
                    throw 1;
                }
                break;
            }
            default:{
                if(value[0]!='\''&&value[0]!='"'){
                    throw 1;
                }
                if(value[value.length()-1]!='\''&&value[value.length()-1]!='"'){
                    throw 1;
                }
                if(value.length()-1>data[k].type){
                    std::cout<<"string length too long."<<std::endl;
                    throw 1;
                }
                e.stringT=value.substr(1,value.length()-2);
            }
        }
    }
    
    
}

void Interpreter::execFile(int i){
    std::string tmp;
    std::string filePath;
    filePath=getWord(i);

    std::fstream fs(filePath);
    std::stringstream ss;
    ss<<fs.rdbuf();
    tmp=ss.str();
    int k1=0;
    int k2=0;
    do{
        while(tmp[k2++]!='\n');
        queryString=tmp.substr(k1,k2-k1-1);
        k1=k2;
        query();
    }while(tmp[k2]!='\n');
}

void Interpreter::exitMiniSQL(){
    std::cout<<">>> bye!"<<std::endl;
    exit(0);
}

std::string Interpreter::getWord(int &i){
    int k=0;
    while(queryString[i++]==' ');
    i--;
    while(queryString[i++]!=' '){
        k++;
    }
    i--;
    return queryString.substr(i-k,k);
}
