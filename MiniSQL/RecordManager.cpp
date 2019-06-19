//
//  RecordManager.cpp
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "RecordManager.hpp"


void RecordManager::createTableFile(std::string table_name) {
    table_name = "./database/data/" + table_name;
    FILE* f = fopen(table_name.c_str() , "w");
    fclose(f);
}

void RecordManager::dropTableFile(std::string table_name) {
    table_name = "./database/data/" + table_name;
    remove(table_name.c_str());
}


void RecordManager::insertRecord(std::string table_name , Tuple& tuple) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist!"<<std::endl;
        throw 1;
    }
    std::vector<AttributeType> attr;
    catalog_manager.getAttribute(tmp_name,attr);
    std::vector<element> v = tuple.getData();
    for (int i = 0;i < v.size();i++) {
        if (v[i].type != attr[i].type)
            std::cout<<"tuple type is conflict"<<std::endl;
    }
    Table table = selectRecord(tmp_name);
    std::vector<Tuple>& tuples = table.Tuple;
    //    if (attr.primary_key >= 0) {
    //        if (isConflict(tuples , v , attr.primary_key) == true)
    //            throw primary_key_conflict();
    //    }
    for (int i = 0;i < attr.size();i++) {
        if (attr[i].isUnique == true) {
            if (isConflict(tuples , v , i) == true){
                std::cout<<"unique conflict!"<<std::endl;
                throw 1;
            }
            
        }
    }
    
    
    int block_num = getBlockNum(table_name);
    
    if (block_num <= 0)
        block_num = 1;
    
    char* p = buffer_manager.getPage(table_name , block_num - 1);
    int i;
    
    for (i = 0;p[i] != '\0' && i < PAGESIZE;i++) ;
    int j;
    int len = 0;
    
    for (j = 0;j < v.size();j++) {
        element d = v[j];
        switch(d.type) {
            case -1:{
                int t = getDataLength(d.intT);
                len += t;
            };break;
            case 0:{
                float t = getDataLength(d.floatT);
                len += t;
            };break;
            default:{
                len += d.stringT.length();
            };
        }
    }
    len += v.size() + 7;
    int block_offset;
    
    if (PAGESIZE - i >= len) {
        block_offset = block_num - 1;
        
        insertRecord1(p , i , len , v);
        
        int page_id = buffer_manager.getPageId(table_name , block_num - 1);
        
        buffer_manager.modifyPage(page_id);
    }
    
    else {
        block_offset = block_num;
        
        char* p = buffer_manager.getPage(table_name , block_num);
        
        insertRecord1(p , 0 , len , v);
        
        int page_id = buffer_manager.getPageId(table_name , block_num);
        
        buffer_manager.modifyPage(page_id);
    }
    
    
    IndexManager index_manager;
    for (int i = 0;i < attr.size();i++) {
        if (attr[i].has_index== true) {
            std::string attr_name = attr[i].name;
            std::string file_path = "INDEX_FILE_" + attr_name + "_" + tmp_name;
            std::vector<element> d = tuple.getData();
            index_manager.insertIndex(file_path , d[i] , block_offset);
        }
    }
}


int RecordManager::deleteRecord(std::string table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist"<<std::endl;
        throw 1;
    }
    
    int block_num = getBlockNum(table_name);
    
    if (block_num <= 0)
        return 0;
    std::vector<AttributeType> attr ;
    catalog_manager.getAttribute(tmp_name,attr);
    IndexManager index_manager;
    int count = 0;
    
    for (int i = 0;i < block_num;i++) {
        
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        
        while(*p != '\0' && p < t + PAGESIZE) {
            
            Tuple tuple = readTuple(p , attr);
            for (int j = 0;j < attr.size();j++) {
                if (attr[j].has_index == true) {
                    std::string attr_name = attr[i].name;
                    std::string file_path = "INDEX_FILE_" + attr_name + "_" + tmp_name;
                    std::vector<element> d = tuple.getData();
                    index_manager.deleteIndex(file_path , d[j]);
                }
            }
            
            p = deleteRecord1(p);
            count++;
        }
        
        int page_id = buffer_manager.getPageId(table_name , i);
        
        buffer_manager.modifyPage(page_id);
    }
    return count;
}

int RecordManager::deleteRecord(std::string table_name , std::string target_attr , condition where) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    //检测表是否存在
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist"<<std::endl;
        throw 1;
    }
    std::vector<AttributeType> attr ;
    catalog_manager.getAttribute(tmp_name,attr);
    int index = -1;
    bool flag = false;
    
    for (int i = 0;i < attr.size();i++) {
        if (attr[i].name == target_attr) {
            index = i;
            if (attr[i].has_index == true)
                flag = true;
            break;
        }
    }
    
    if (index == -1) {
        std::cout<<"attribute is not exist"<<std::endl;
        throw 1;
        
    }
    
    else if (attr[index].type != where.e.type) {
        std::cout<<"data type is conflict!"<<std::endl;
        throw 1;
        
    }
    
    
    
    int count = 0;
    
    if (flag == true && where.relation != NOT_EQUAL) {
        std::vector<int> block_ids;
        
        searchWithIndex(tmp_name , target_attr , where , block_ids);
        for (int i = 0;i < block_ids.size();i++) {
            count += conditionDeleteInBlock(tmp_name , block_ids[i] , attr , index , where);
        }
    }
    else {
        
        int block_num = getBlockNum(table_name);
        
        if (block_num <= 0)
            return 0;
        
        for (int i = 0;i < block_num;i++) {
            count += conditionDeleteInBlock(tmp_name , i , attr , index , where);
        }
    }
    return count;
}


Table RecordManager::selectRecord(std::string table_name , std::string result_table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist"<<std::endl;
        throw 1;
    }
    
    int block_num = getBlockNum(table_name);
    
    if (block_num <= 0)
        block_num = 1;
    
    std::vector<AttributeType> attr ;
    catalog_manager.getAttribute(tmp_name,attr);
    
    Table table(result_table_name , attr);
    std::vector<Tuple>& v = table.Tuple;
    
    for (int i = 0;i < block_num;i++) {
        
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        
        while (*p != '\0' && p < t + PAGESIZE) {
            
            Tuple tuple = readTuple(p , attr);
            
            if (tuple.isDeleted == false)
                v.push_back(tuple);
            int len = getTupleLength(p);
            p = p + len;
        }
    }
    return table;
}

Table RecordManager::selectRecord(std::string table_name , std::string target_attr ,condition where , std::string result_table_name) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist"<<std::endl;
        throw 1;
    }
    std::vector<AttributeType> attr ;
    catalog_manager.getAttribute(tmp_name,attr);
    int index = -1;
    bool flag = false;
    
    for (int i = 0;i < attr.size();i++) {
        if (attr[i].name == target_attr) {
            index = i;
            if (attr[i].has_index == true)
                flag = true;
            break;
        }
    }
    
    if (index == -1) {
        std::cout<<"attribute is not exist"<<std::endl;
        throw 1;
    }
    
    else if (attr[index].type != where.e.type) {
        std::cout<<"data type is conflict!"<<std::endl;
        throw 1;
    }
    
    
    Table table(result_table_name , attr);
    std::vector<Tuple>& v = table.Tuple;
    if (flag == true && where.relation != NOT_EQUAL) {
        std::vector<int> block_ids;
        
        searchWithIndex(tmp_name , target_attr , where , block_ids);
        for (int i = 0; i < block_ids.size();i++) {
            conditionSelectInBlock(tmp_name , block_ids[i] , attr , index , where , v);
        }
    }
    else {
        
        int block_num = getBlockNum(table_name);
        
        if (block_num <= 0)
            block_num = 1;
        
        for (int i = 0;i < block_num;i++) {
            conditionSelectInBlock(tmp_name , i , attr , index , where , v);
        }
    }
    return table;
}

void RecordManager::createIndex(IndexManager& index_manager , std::string table_name , std::string target_attr) {
    std::string tmp_name = table_name;
    table_name = "./database/data/" + table_name;
    CatalogManager catalog_manager;
    
    if (!catalog_manager.hasTable(tmp_name)) {
        std::cout<<"table is not exist"<<std::endl;
        throw 1;
    }
    std::vector<AttributeType> attr ;
    catalog_manager.getAttribute(tmp_name,attr);
    int index = -1;
    
    for (int i = 0;i < attr.size();i++) {
        if (attr[i].name == target_attr) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        std::cout<<"attribute is not exist"<<std::endl;
        throw 1;
    }
    
    int block_num = getBlockNum(table_name);
    
    if (block_num <= 0)
        block_num = 1;
    
    std::string file_path = "INDEX_FILE_" + target_attr + "_" + tmp_name;
    
    for (int i = 0;i < block_num;i++) {
        char* p = buffer_manager.getPage(table_name , i);
        char* t = p;
        while (*p != '\0' && p < t + PAGESIZE) {
            Tuple tuple = readTuple(p , attr);
            if (tuple.isDeleted == false) {
                std::vector<element> v = tuple.getData();
                index_manager.insertIndex(file_path , v[index] , i);
            }
            int len = getTupleLength(p);
            p = p + len;
        }
    }
}


int RecordManager::getBlockNum(std::string table_name) {
    char* p;
    int block_num = -1;
    do {
        p = buffer_manager.getPage(table_name , block_num + 1);
        block_num++;
    } while(p[0] != '\0');
    return block_num;
}

void RecordManager::insertRecord1(char* p , int offset , int len , const std::vector<element>& v) {
    std::stringstream stream;
    stream << len;
    std::string s = stream.str();
    while (s.length() < 4)
        s = "0" + s;
    for (int j = 0;j < s.length();j++,offset++)
        p[offset] = s[j];
    for (int j = 0;j < v.size();j++) {
        p[offset] = ' ';
        offset++;
        element d = v[j];
        switch(d.type) {
            case -1:{
                copyString(p , offset , d.intT);
            };break;
            case 0:{
                copyString(p , offset , d.floatT);
            };break;
            default:{
                copyString(p , offset , d.stringT);
            };
        }
    }
    p[offset] = ' ';
    p[offset + 1] = '0';
    p[offset + 2] = '\n';
}

char* RecordManager::deleteRecord1(char* p) {
    int len = getTupleLength(p);
    p = p + len;
    *(p - 2) = '1';
    return p;
}

Tuple RecordManager::readTuple(const char* p ,std::vector<AttributeType> attr) {
    Tuple tuple;
    p = p + 5;
    for (int i = 0;i < attr.size();i++) {
        element data;
        data.type = attr[i].type;
        char tmp[100];
        int j;
        for (j = 0;*p != ' ';j++,p++) {
            tmp[j] = *p;
        }
        tmp[j] = '\0';
        p++;
        std::string s(tmp);
        switch(data.type) {
            case -1:{
                std::stringstream stream(s);
                stream >> data.intT;
            };break;
            case 0:{
                std::stringstream stream(s);
                stream >> data.floatT;
            };break;
            default:{
                data.stringT = s;
            }
        }
        tuple.addData(data);
    }
    if (*p == '1')
        tuple.setDeleted();
    return tuple;
}

int RecordManager::getTupleLength(char* p) {
    char tmp[10];
    int i;
    for (i = 0;p[i] != ' ';i++)
        tmp[i] = p[i];
    tmp[i] = '\0';
    std::string s(tmp);
    int len = stoi(s);
    return len;
}

bool RecordManager::isConflict(std::vector<Tuple>& tuples , std::vector<element>& v , int index) {
    for (int i = 0;i < tuples.size();i++) {
        if (tuples[i].isDeleted == true)
            continue;
        std::vector<element> d = tuples[i].getData();
        switch(v[index].type) {
            case -1:{
                if (v[index].intT == d[index].intT)
                    return true;
            };break;
            case 0:{
                if (v[index].floatT == d[index].floatT)
                    return true;
            };break;
            default:{
                if (v[index].stringT == d[index].stringT)
                    return true;
            };
        }
    }
    return false;
}

void RecordManager::searchWithIndex(std::string table_name , std::string target_attr , condition where , std::vector<int>& block_ids) {
    IndexManager index_manager;
    element tmp_data;
    std::string file_path = "INDEX_FILE_" + target_attr + "_" + table_name;
    if (where.relation == LESS || where.relation == LESS_OR_EQUAL) {
        if (where.e.type == -1) {
            tmp_data.type = -1;
            tmp_data.intT = -INF;
        }
        else if (where.e.type == 0) {
            tmp_data.type = 0;
            tmp_data.floatT = -INF;
        }
        else {
            tmp_data.type = 1;
            tmp_data.stringT = "";
        }
        index_manager.searchRange(file_path , tmp_data , where.e , block_ids);
    }
    else if (where.relation == MORE || where.relation == MORE_OR_EQUAL) {
        if (where.e.type == -1) {
            tmp_data.type = -1;
            tmp_data.intT = INF;
        }
        else if (where.e.type == 0) {
            tmp_data.type = 0;
            tmp_data.floatT = INF;
        }
        else {
            tmp_data.type = -2;
        }
        index_manager.searchRange(file_path , where.e , tmp_data , block_ids);
    }
    else {
        index_manager.searchRange(file_path , where.e , where.e , block_ids);
    }
}

int RecordManager::conditionDeleteInBlock(std::string table_name , int block_id , std::vector<AttributeType> attr , int index , condition where) {
    table_name = "./database/data/" + table_name;
    char* p = buffer_manager.getPage(table_name , block_id);
    char* t = p;
    int count = 0;
    while (*p != '\0' && p < t + PAGESIZE) {
        Tuple tuple = readTuple(p , attr);
        std::vector<element> d = tuple.getData();
        switch(attr[index].type) {
            case -1:{
                if (isSatisfied(d[index].intT , where.e.intT , where.relation) == true) {
                    p = deleteRecord1(p);
                    count++;
                }
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            };break;
            case 0:{
                if (isSatisfied(d[index].floatT , where.e.floatT , where.relation) == true) {
                    p = deleteRecord1(p);
                    count++;
                }
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            };break;
            default:{
                if (isSatisfied(d[index].stringT , where.e.stringT , where.relation) == true) {
                    p = deleteRecord1(p);
                    count++;
                }
                else {
                    int len = getTupleLength(p);
                    p = p + len;
                }
            }
        }
    }
    int page_id = buffer_manager.getPageId(table_name , block_id);
    buffer_manager.modifyPage(page_id);
    return count;
}

void RecordManager::conditionSelectInBlock(std::string table_name , int block_id , std::vector<AttributeType> attr , int index , condition where , std::vector<Tuple>& v) {
    table_name = "./database/data/" + table_name;
    char* p = buffer_manager.getPage(table_name , block_id);
    char* t = p;
    while (*p != '\0' && p < t + PAGESIZE) {
        Tuple tuple = readTuple(p , attr);
        if (tuple.isDeleted == true) {
            int len = getTupleLength(p);
            p = p + len;
            continue;
        }
        std::vector<element> d = tuple.getData();
        switch(attr[index].type) {
            case -1:{
                if (isSatisfied(d[index].intT , where.e.intT , where.relation) == true) {
                    v.push_back(tuple);
                }
            };break;
            case 0:{
                if (isSatisfied(d[index].floatT , where.e.floatT , where.relation) == true) {
                    v.push_back(tuple);
                }
            };break;
            default:{
                if (isSatisfied(d[index].stringT , where.e.stringT , where.relation) == true) {
                    v.push_back(tuple);
                }
            };
        }
        int len = getTupleLength(p);
        p = p + len;
    }
}
