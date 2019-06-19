//
//  main.cpp
//  MiniSQL
//
//  Created by Domoto Yasu on 2019/6/1.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#include "MiniSQL.h"
#include "API.hpp"
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "RecordManager.hpp"
#include "IndexManager.hpp"
#include "Interpreter.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    std::cout<<">>> Welcome to MiniSQL"<<std::endl;
    while(1){
        Interpreter query;
        query.read();
    }
    return 0;
}

