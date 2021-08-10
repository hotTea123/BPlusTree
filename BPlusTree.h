#ifndef bpt_H
#define bpt_H

#include<iostream>
#include<string>
#include<queue>
#include<fstream>

const int M = 4;
const int MIN = M % 2 ? M / 2 + 1 : M/2;

struct BPlusNode{
    bool isLeaf;
    int keynum;
    int key[M + 1];
    BPlusNode *children[M + 1];
    BPlusNode *next;
    BPlusNode *prior;
};

//template <typename T>
class BPlusTree{
    public:
        BPlusNode* NewBPlusNode();

        BPlusNode* Initialize();
        
        int findPosition(BPlusNode *p, int e);
        
        BPlusNode* Split(BPlusNode* p, BPlusNode* parent);
        
        BPlusNode* InsertT(BPlusNode* p, int e, BPlusNode* parent);
        
        BPlusNode* Insert(BPlusNode* T, int e);
        
        BPlusNode* Search(BPlusNode* p, int e);
        
        BPlusNode* RemoveElement(BPlusNode* p, BPlusNode* parent, int inpar);
        
        BPlusNode* Merge(BPlusNode* p, BPlusNode* q , BPlusNode* parent, int inpar);
        
        BPlusNode* FindSiblingAndLendOrMerge(BPlusNode* p, BPlusNode* parent, int inpar);
        
        BPlusNode* RemoveT(BPlusNode* p, int e, BPlusNode* parent);
        
        BPlusNode* Remove(BPlusNode* T, int e);
        
        std::string Serialize(BPlusNode* p);
        
        std::queue<std::string> SplitStr(std::string str, char ch);
        
        BPlusNode* DeSerialize(std::queue<std::string>& nodeQue);

};

#endif