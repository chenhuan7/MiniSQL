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
    str_tmp=str_tmp+" "+num2str(data.size, 2);
    //添加每个attribute的信息，顺序为类型，名字，是否为唯一
    for(int i=0;i<data.size;i++)
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

    for (int i=0; i<data.size; i++)
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


/* 
bool CatalogManager::createIndex(const std::string &tableName, const std::string &indexName, int attributeIndex){
    
}
bool CatalogManager::dropIndex(const std::string &indexName){
    
}

Table* CatalogManager::loadTable(const std::string &tableName){
    if(!hasTable(tableName)){
        std::cout<<"no table"<<std::endl;
        return NULL;
    }
}
bool CatalogManager::hasIndex(const std::string &indexName){
    
}
//bool createIndex(const std::string &tableName, const std::string &indexName, int attrIndex);
std::pair<std::string, int> getIndex(const std::string &indexName);
//IndexTableManager IM;
*/

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