//
//  BPlusTree.h
//  MiniSQL
//
//  Created by 沈霁 on 2019/6/11.
//  Copyright © 2019 Zhao Weikai, Shen Ji, Chen Huan. All rights reserved.
//

#ifndef BPlusTree_h
#define BPlusTree_h
#include "MiniSQL.h"

template <typename T>
class BPlusTree{
public:
    BPlusTree(std::string fileName, int keySize, int degree);
    ~BPlusTree();
    
};

#endif /* BPlusTree_h */
