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
    API API;
    CatalogManager CM;
    std::string tableName;
    std::vector<std::string> atttributeName;
    std::vector<std::string> targetName;
    std::vector<condition> conditions;
    condition tmpCondition;
    std::string tmp;
    Table outTable;
    int op;
    bool flag;
    tmp=getWord(i);
    if(tmp=="*"){
        flag=true;
        tmp=getWord(i);
    }else{
        flag=false;
        atttributeName.push_back(tmp);
        tmp=getWord(i);
        while(tmp==","){
            atttributeName.push_back(getWord(i));
            tmp=getWord(i);
        }
    }
    if(tmp!="from"){
        throw 1;
    }
    tableName=getWord(i);
    std::vector<AttributeType> attributes;
    CM.getAttribute(tableName,attributes);
    if(flag){
        for(int k;k<attributes.size();k++){
            atttributeName.push_back(attributes[k].name);
        }
    }
    tmp=getWord(i);
    if(tmp=="where"){
        while(1){
            std::string tmp_target=getWord(i);
            targetName.push_back(tmp_target);
            tmp=getWord(i);
            std::string tmp2=getWord(i);
            std::string key;
            if(tmp2=="="||tmp2==">"){
                tmp+=tmp2;
                key=getWord(i);
            }
            else{
                key=tmp2;
            }
            if(tmp=="<")
                tmpCondition.relation=LESS;
            else if(tmp=="<=")
                tmpCondition.relation=LESS_OR_EQUAL;
            else if(tmp=="=")
                tmpCondition.relation=EQUAL;
            else if(tmp==">=")
                tmpCondition.relation=MORE_OR_EQUAL;
            else if(tmp==">")
                tmpCondition.relation=MORE;
            else if(tmp=="!="||tmp=="<>")
                tmpCondition.relation=NOT_EQUAL;
            else
                throw 1;
            std::string tmp_key=getWord(i);
            for(int k;k<attributes.size();k++){
                if(attributes[k].name==tmp_target){
                    tmpCondition.e.type=attributes[k].type;
                    switch(attributes[k].type){
                        case -1:{
                            try{
                                tmpCondition.e.intT=strToNum<int>(key);
                            }catch(...){
                                throw 1;
                            }
                            break;
                        }
                        case 0:{
                            try{
                                tmpCondition.e.floatT=strToNum<float>(key);
                            }catch(...){
                                throw 1;
                            }
                            break;
                        }
                        default:{
                            try{
                                tmpCondition.e.stringT=key;
                            }catch(...){
                                throw 1;
                            }
                            break;
                        }
                    }
                    
                    break;
                }
            }
            conditions.push_back(tmpCondition);
            tmp=getWord(i);
            if(tmp==";"){
                break;
            }
            else if(tmp=="and"){
                op=1;
            }
            else if(tmp=="or"){
                op=0;
            }
            
        }
        outTable=API.selectRecord(tableName, targetName,conditions,op);
        
    }
    else{
        outTable=API.selectRecord(tableName, targetName,conditions,op);
    }
    
    std::vector<AttributeType> attr_record=outTable.data;
    int use[32]={0};
    
    if(atttributeName.size()==0){
        for(int i=0;i<attr_record.size( );i++)
            use[i]=i;
    }
    else{
        for(int i=0;i<atttributeName.size();i++)
            for(int j=0;j<attr_record.size();j++){
                if(attr_record[j].name==atttributeName[i])
                {
                    use[i]=j;
                    break;
                }
            }
    }
    std::vector<Tuple> output_tuple=outTable.Tuple;
    int longest=-1;
    for(int index=0;index<atttributeName.size();index++){
        if((int)attr_record[use[index]].name.length()>longest)
            longest=(int)attr_record[use[index]].name.length();
    }
    for(int index=0;index<atttributeName.size();index++){
        int type=attr_record[use[index]].type;
        if(type==-1){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<getBits(output_tuple[i].getData()[use[index]].intT)){
                    longest=getBits(output_tuple[i].getData()[use[index]].intT);
                }
            }
        }
        if(type==0){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<getBits(output_tuple[i].getData()[use[index]].floatT)){
                    longest=getBits(output_tuple[i].getData()[use[index]].floatT);
                }
            }
        }
        if(type>0){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<output_tuple[i].getData()[use[index]].stringT.length()){
                    longest=(int)output_tuple[i].getData()[use[index]].stringT.length();
                }
            }
        }
    }
    longest+=1;
    for(int index=0;index<atttributeName.size();index++){
        if(index!=atttributeName.size()-1){
            for(int i=0;i<(longest-attr_record[use[index]].name.length())/2;i++)
                printf(" ");
            printf("%s",attr_record[use[index]].name.c_str());
            for(int i=0;i<longest-(longest-attr_record[use[index]].name.length())/2-attr_record[use[index]].name.length();i++)
                printf(" ");
            printf("|");
        }
        else{
            for(int i=0;i<(longest-attr_record[use[index]].name.length())/2;i++)
                printf(" ");
            printf("%s",attr_record[use[index]].name.c_str());
            for(int i=0;i<longest-(longest-attr_record[use[index]].name.length())/2-attr_record[use[index]].name.length();i++)
                printf(" ");
            printf("\n");
        }
    }
    for(int index=0;index<atttributeName.size()*(longest+1);index++){
        std::cout<<"-";
    }
    std::cout<<std::endl;
    for(int index=0;index<output_tuple.size();index++){
        for(int i=0;i<atttributeName.size();i++)
        {
            switch (output_tuple[index].getData()[use[i]].type) {
                case -1:
                    if(i!=atttributeName.size()-1){
                        int len=output_tuple[index].getData()[use[i]].intT;
                        len=getBits(len);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%d",output_tuple[index].getData()[use[i]].intT);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        int len=output_tuple[index].getData()[use[i]].intT;
                        len=getBits(len);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%d",output_tuple[index].getData()[use[i]].intT);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                case 0:
                    if(i!=atttributeName.size()-1){
                        float num=output_tuple[index].getData()[use[i]].floatT;
                        int len=getBits(num);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%.2f",output_tuple[index].getData()[use[i]].floatT);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        float num=output_tuple[index].getData()[use[i]].floatT;
                        int len=getBits(num);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%.2f",output_tuple[index].getData()[use[i]].floatT);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                default:
                    std::string tmp=output_tuple[index].getData()[use[i]].stringT;
                    if(i!=atttributeName.size()-1){
                        for(int i=0;i<(longest-tmp.length())/2;i++)
                            printf(" ");
                        printf("%s",tmp.c_str());
                        for(int i=0;i<longest-(longest-(int)tmp.length())/2-(int)tmp.length();i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        std::string tmp=output_tuple[index].getData()[i].stringT;
                        for(int i=0;i<(longest-tmp.length())/2;i++)
                            printf(" ");
                        printf("%s",tmp.c_str());
                        for(int i=0;i<longest-(longest-(int)tmp.length())/2-(int)tmp.length();i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
            }
        }
    }
}

void Interpreter::deleteRecord(int i){
    API API;
    CatalogManager CM;
    condition conditions;
    std::string tableName;
    std::string attributeName;
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
    attributeName=getWord(i);
    tmp=getWord(i);
    std::string tmp2=getWord(i);
    std::string key;
    if(tmp2=="="||tmp2==">"){
        tmp+=tmp2;
        key=getWord(i);
    }
    else{
        key=tmp2;
    }
    if(tmp=="<")
        conditions.relation=LESS;
    else if(tmp=="<=")
        conditions.relation=LESS_OR_EQUAL;
    else if(tmp=="=")
        conditions.relation=EQUAL;
    else if(tmp==">=")
        conditions.relation=MORE_OR_EQUAL;
    else if(tmp==">")
        conditions.relation=MORE;
    else if(tmp=="!="||tmp=="<>")
        conditions.relation=NOT_EQUAL;
    else
        throw 1;
    std::vector<AttributeType> attributes;
    CM.getAttribute(tableName,attributes);
    for(int k;k<attributes.size();k++){
        if(attributes[k].name==attributeName){
            conditions.e.type=attributes[k].type;
            switch(attributes[k].type){
                case -1:{
                    try{
                        conditions.e.intT=strToNum<int>(key);
                    }catch(...){
                        throw 1;
                    }
                    break;
                }
                case 0:{
                    try{
                        conditions.e.floatT=strToNum<float>(key);
                    }catch(...){
                        throw 1;
                    }
                    break;
                }
                default:{
                    try{
                        conditions.e.stringT=key;
                    }catch(...){
                        throw 1;
                    }
                    break;
                }
            }
            
            break;
        }
    }
    API.deleteRecord(tableName, attributeName, conditions);
    
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
    if(getWord(i)!="on"){
        throw 1;
    }
    std::string tableName=getWord(i);
    API.dropIndex(tableName,indexName);
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
    CM.getAttribute(tableName,data);
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
