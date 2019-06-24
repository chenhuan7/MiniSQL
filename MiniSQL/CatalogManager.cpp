//
//  CatalogManager.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//


#include "CatalogManager.hpp"

BufferManager bufferManager;
CatalogManager::CatalogManager()
{
    blockNum=0;
}

bool CatalogManager::createTable(std::string tableName, const std::vector<AttributeType> &data, int primary)
{
    if (hasTable(tableName))
    {
        return false;
    }
    std::string str_tmp="T";

    //添加name
    str_tmp+=tableName;
    //添加attribute的数量
    str_tmp=str_tmp+" "+num2str(data.size(), 2);
    //添加每个attribute的信息，顺序为类型，名字，是否为唯一
    for(int i=0;i<data.size();i++)
        str_tmp=str_tmp+" "+num2str(data[i].type, 1)+" "+num2str(data[i].length, 2)+' '+data[i].name+" "+(data[i].isUnique==true?"1":"0");
    //添加主键信息
    str_tmp=str_tmp+" "+num2str(primary, 2);

    for(int current_block=0;current_block<blockNum;current_block++)
    {
        char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , current_block);
        int pageID = bufferManager.getPageId(TABLE_MANAGER_PATH , current_block);

        if (buffer[0]!='T')
        {
            buffer[0]='\0';
            strcat(buffer , str_tmp.c_str());
            bufferManager.modifyPage(pageID);    
            return true;
        }
    }
    //插入块
    blockNum++;
    char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , blockNum-1);
    int pageID = bufferManager.getPageId(TABLE_MANAGER_PATH , blockNum-1);
    buffer[0]='\0';
    strcat(buffer , str_tmp.c_str());
    bufferManager.modifyPage(pageID);    
    return true;
}

bool CatalogManager::dropTable(std::string tableName)
{
    if (!hasTable(tableName))
        return false;
    
    for(int current_block=0;current_block<blockNum;current_block++)
    {
        char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , current_block);
        int pageID = bufferManager.getPageId(TABLE_MANAGER_PATH , current_block);

        if (buffer[0]!='T')
            continue;
        if (checkTableName(buffer,tableName))
        {
            bufferManager.dropPage(pageID);
            if (current_block==blockNum-1)
                blockNum--;
            bufferManager.modifyPage(pageID);    
        }
    }
    return true;
}

bool CatalogManager::getAttribute(std::string tableName, std::vector<AttributeType> &data)
{
    if (!hasTable(tableName))
        return false;
    
    for(int current_block=0;current_block<blockNum;current_block++)
    {
        char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , current_block);
        int pageID = bufferManager.getPageId(TABLE_MANAGER_PATH , current_block);
        std::string buffer_check(buffer);
        if (buffer[0]!='T')
            continue;
        if (checkTableName(buffer,tableName))
        {
           int index=0;
           while (buffer[index]!=' ')
                index++;
           index++;

           std::string attr_num=buffer_check.substr(index,2);
           int num=str2num(attr_num);
           index+=3;

           for (int i=0;i<num;i++)
           {
                AttributeType tmp;
                std::string type=buffer_check.substr(index,1);
                tmp.type=str2num(type);
                std::string l=buffer_check.substr(index+2,2);
                tmp.length=str2num(l);
                index+=5;
               
                int length=0;
                while (buffer[length+index]!=' ')
                    length++;
                tmp.name=buffer_check.substr(index,length);
                index+=length+1;

                if (buffer[index]=='0')
                    tmp.isUnique=false;
                else tmp.isUnique=true;

                data.push_back(tmp);
                index+=2;
           }
           return true;
        }
    }
    return false;
}

bool CatalogManager::hasAttribute(std::string tableName , std::string attrName)
{
    std::vector<AttributeType> data;
    if (!getAttribute(tableName,data))
    {
        return false;
    }

    for (int i=0; i<data.size(); i++)
    {
        if (tableName==data[i].name)
            return true;
    }
    return false;
}

bool CatalogManager::hasTable(std::string tableName)
{
    for(int current_block=0; current_block<blockNum; current_block++)
    {
        char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , current_block);
        std::string buf = buffer;
        if (buffer[0]!='T')
            continue;
        if (checkTableName(buf, tableName))
            return true;
    }
    return false;
}

bool CatalogManager::checkTableName(std::string buffer, std::string tableName)
{
    int index=0;

    while (buffer[index]!=' ')
        index++;

    std::string  str_tmp;
    str_tmp=buffer.substr(0,index);
    if (!tableName.compare(str_tmp))
        return true;
    else return false;
}

bool CatalogManager::createIndex(std::string table_name,std::string attr_name,std::string index_name){
    //异常判断
    if(!hasTable(table_name))
        return false;
    if(!hasAttribute(table_name, attr_name))
        return false;
    Index index_record=getIndex(table_name);
    if(index_record.num>=10)
        return false;
    
    std::vector<AttributeType> data;
    getAttribute(table_name, data);
    //遍历所有已有的index
    for(int i=0;i<index_record.num;i++){
        //索引名重复错误
        if(index_record.indexname[i]==index_name)
            return false;
        //索引重复
        if(data[i].name==attr_name)
            return false;
    }
    int key;

    index_record.indexname[index_record.num]=index_name;
    for(int index=0;index<data.size();index++){
        if(attr_name==data[index].name)
        {
            index_record.location[index_record.num]=index;
            break;
        }
    }
    index_record.num++;
    //刷新
    dropTable(table_name);
    createTable(table_name, data, key);
    return true;
}

bool CatalogManager::dropIndex(std::string table_name,std::string index_name){
    //如果不存在该索引，则异常
    if(!hasTable(table_name)){
        return false;
    }
    //得到该表的index的信息
    Index index_record=getIndex(table_name);
    //得到属性的信息
    std::vector<AttributeType> data;
    getAttribute(table_name, data);
    //遍历所有的index，查找是否有对应名字的索引，如果不存在则异常
    int hasindex=-1;
    for(int index=0;index<index_record.num;index++){
        if(index_record.indexname[index]==index_name){
            hasindex=index;
            break;
        }
    }
    if(hasindex==-1){
        return false;
    }
    int key;
    //通过将该信息与最后位置的索引替换的方式来删除索引
    index_record.indexname[hasindex]=index_record.indexname[index_record.num-1];
    index_record.location[hasindex]=index_record.location[index_record.num-1];
    index_record.num--;
    //在原有表中删除该表后再插入，实现刷新
    dropTable(table_name);
    createTable(table_name, data, key);
    return true;
}

std::string CatalogManager::num2str(int num,short bit){
    std::string str="";
    if(num<0){
        num=-num;
        str+="-";
    }
    int divisor=pow(10,bit-1);
    for(int i=0;i<bit;i++){
        str+=(num/divisor%10+'0');
        divisor/=10;
    }
    return str;
}

int CatalogManager::str2num(std::string str){
    return atoi(str.c_str());
}

Index CatalogManager::getIndex(std::string table_name){
    Index index_record;
    //得到该表的位置和对应的块
    int suitable_block;
    int start_index=(table_name,suitable_block);
    char* buffer = bufferManager.getPage(TABLE_MANAGER_PATH , suitable_block);
    //将start_index对齐索引信息的位置
    std::string buffer_check(buffer);
    while(buffer_check[start_index]!=';')
        start_index++;
    start_index++;
    //得到索引的数量
    index_record.num=str2num(buffer_check.substr(start_index,2));
    //得到索引的所有信息
    for(int times=0;times<index_record.num;times++){
        start_index+=3;
        index_record.location[times]=str2num(buffer_check.substr(start_index,2));
        start_index+=3;
        while(buffer_check[start_index]!=' '&&buffer_check[start_index]!='#'&&buffer_check[start_index]!='\n'){
            index_record.indexname[times]+=buffer_check[start_index++];
        }
        start_index-=2;
    }
    return index_record;
}

std::string CatalogManager::IndextoAttr(std::string table_name,std::string index_name) {
    if(!hasTable(table_name))
        return "";
    Index index_record=getIndex(table_name);
    int hasfind=-1;
    for(int i=0;i<index_record.num;i++){
        if(index_record.indexname[i]==index_name){
            hasfind=i;
            break;
        }
    }
    if(hasfind==-1)
        return "";
    std::vector<AttributeType> data;
    int attr_record=getAttribute(table_name, data);
    return "";
}
