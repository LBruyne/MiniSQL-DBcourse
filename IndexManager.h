/*
 * File name: IndexManager.h
 * Author: Xuanming, Liu
 * Latest revised: 2020.06.16 
 * Description: 
 * Header for IM module.
 * Function:
 * 1. Using B+ tree to realize indexing.
 * 2. Provide API for outer part.
**/

#ifndef _MINISQL_INDEXMANAGER_H_
#define _MINISQL_INDEXMANAGER_H_

#include "global.h"
#include "BPlusTree.h"
using namespace std;

class IndexManager
{
public:
	std::string bptIntName;
	std::string bptFloatName;
	std::string bptStringName;
	BPlusTree<int> *bpt_INT;
	BPlusTree<float> *bpt_FLOAT;
	BPlusTree<string> *bpt_STRING;

public:
	IndexManager();
	bool CreateIndex(Table &table, Index &index);
	bool CreateIndex(Table &table);
	bool HasIndex(std::string &TableName, std::string &AttribName);
	bool DropIndex(std::string &TableName, std::string &AttribName);
	void ResetBptInt(bool save);
	void ResetBptFloat(bool save);
	void ResetBptString(bool save);
	void SaveIndex();
	bool LoadIndex(std::string &TableName, std::string &AttribName, int type);
	bool InsertItem(const std::string &value, Pointer pointer, int type);
	bool DeleteItem(std::string &value, int type) ;
	bool DeletePointers(std::vector<Pointer> &pointers, int type);
	bool SelectItem(Condition &cond, int type, std::vector<Pointer> &pointers);
	Pointer FindPointer(std::string& value, int type);
	bool Find(std::string& value, int type);
	~IndexManager();
};

#endif