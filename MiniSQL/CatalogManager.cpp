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
        str_tmp=str_tmp+" "+num2str(data[i].type, 1)+" "+data[i].name+" "+(data[i].isUnique==true?"1":"0");
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

        if (checkTableName(buffer,tableName))
        {
            bufferManager.dropPage(pageID);
            if (current_block==blockNum-1)
                blockNum--;
            bufferManager.modifyPage(pageID);    
        }
    }
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
bool CatalogManager::hasTable(std::string table_name){
    //计算块的数量
    int block_num=getBlockNum(TABLE_MANAGER_PATH)/PAGESIZE;
    if(block_num<=0)
        block_num=1;
    //遍历所有的块
    for(int current_block=0;current_block<block_num;current_block++){
        char* buffer = buffer_manager.getPage(TABLE_MANAGER_PATH , current_block);
        std::string buffer_check(buffer);
        std::string str_tmp="";
        int start_index=0,end_index=0;
        do{
            //如果一开始就是#，则检查下一块
            if(buffer_check[0]=='#')
                break;
            //得到table的名字，如果与输入的名字相同，则return true
            else if(getTableName(buffer, start_index, end_index)==table_name){
                return true;
            }
            else{
                //通过字符串长度来重新确定下一个table的位置
                start_index+=str2num(buffer_check.substr(start_index,4));
                //排除空文档的特殊条件
                if(!start_index)
                    break;
            }
        }while(buffer_check[start_index]!='#');  //判断是否到头
    }
    return false;
}
bool CatalogManager::dropTable(const std::string &tableName){
    if(!hasTable(tableName)){
        std::cout<<"table not exist!"<<std::endl;
        return false;
    }
    else{
        
        
        
        
        return true;
    }
}
bool CatalogManager::createIndex(const std::string &tableName, const std::string &indexName, int attributeIndex){
    
}
bool CatalogManager::dropIndex(const std::string &indexName){
    
}
//验证是否存在表
bool CatalogManager::hasTable(const std::string &tableName){
    std::ifstream fin((tableName).c_str());
    std::string header;
    if (!fin) return false;
    
    fin >> header;
    fin.close();
    if(tableName == header)
        return true;
    else
        return false;
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