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
    API API;
    CatalogManager CM;
    condition conditions;
    std::string tableName;
    if(getWord(i)!="from"){
        throw 1;
    }
    tableName=getWord(i);
    std::string tmp=getWord(i);
    if(tmp==":"){
        API.deleteRecord(tableName, "", conditions);
        return;
    }
    
    if(getWord(i)!="where"){
        throw 1;
    }
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
    API API;
    int k;
    int primary=-1;
    std::string tableName;
    tableName=getWord(i);
    std::vector<AttributeType> data;
    std::string attributeName;
    if(getWord(i)!="("){
        throw 1;
    }
    k=0;
    while(1){
        attributeName=getWord(i);
        if(attributeName=="primary"){
            std::string tmp=getWord(i);
            if(tmp!="key"){
                data[k++].name=attributeName;
                break;
            }
            else{
                std::string tmp=getWord(i);
                if(tmp!="("){
                    throw 1;
                }
                tmp=getWord(i);
                for(int find=0;find<data.size();find++){
                    if(data[find].name==tmp){
                        primary=find;
                        data[find].isUnique=true;
                        break;
                    }
                }
                if(primary==-1){
                    throw 1;
                }
                if(tmp!=")"){
                    throw 1;
                }
                continue;
            }
        }
        else{
            data[k++].name=attributeName;
        }
        data[k-1].type=getType(i);
        data[k-1].isUnique=false;
        std::string tmp=getWord(i);
        if(tmp==","){
            continue;
        }
        else if(tmp==")"){
            break;
        }
        else{
            if(tmp=="unique"){
                data[k-1].isUnique=true;
            }
            else{
                throw 1;
            }
        }
    }
    API.createTable(tableName, data, primary);
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
    Tuple tupleToInsert;
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
                break;
            }
        }
        tupleToInsert.addData(e);
    }
    if(k+1!=data.size()){
        std::cout<<"attribute number error."<<std::endl;
        throw 1;
    }
    API.insertRecord(tableName, tupleToInsert);
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

int Interpreter::getType(int &i){
    std::string type;
    type=getWord(i);
    if(type=="int"){
        return -1;
    }
    else if(type=="float"){
        return 0;
    }
    else if(type=="char"){
        if(getWord(i)!="(")
            throw 1;
        std::string tmp=getWord(i);
        if(getWord(i)!=")")
            throw 1;
        return strToNum<int>(tmp);
    }
    else{
        throw 1;
    }
}
