/*
 * File name: gloabl.h
 * Author: Xuanming, Liu
 * Latest revised: 2020.06.13
 * Description: 
 * reference for project
**/

#ifndef _MINISQL_GLOBAL_H_
#define _MINISQL_GLOBAL_H_

#include <unistd.h>
#include <windows.h>
#include <sys/types.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <map>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

// pageSize is an integral multiple of KB, define as 4KB.
#define PAGE_SIZE 4096
// cache capacity, 10 page as defined
#define CACHE_CAPACITY 10
// not found the page in cache
#define PAGE_NOTFOUND -1

#define defaultMaxChildrens 3

#define INT 1
#define FLOAT 2
#define CHAR 3
#define INTLENGTH 11
#define FLOATLENGTH 10
#define INTLEN		11
#define FLOATLEN	10

typedef int Pointer;
typedef int PageIndex;
typedef int FileHandle;

enum Comparison { Lt, Le, Gt, Ge, Eq, Ne };
enum PageType {
    Undefined,
    RecordPage,
    IndexPage,
};

class Attribute
{
public:
	string name;
	int type;
	int length;
	bool isPrimeryKey;
	bool isUnique;
	Attribute()
	{
		isPrimeryKey = false;
		isUnique = false;
	}
	Attribute(string n, int t, int l, bool isP, bool isU)
		:name(n), type(t), length(l), isPrimeryKey(isP), isUnique(isU) {}
};

class Table
{
public:
	string name;
	int blockNum;
	int attriNum;
	int totalLength;
	vector<Attribute> attributes;
	Table() : blockNum(0), attriNum(0), totalLength(0) {}
};

class Index
{
public:
	string index_name;
	string table_name;
	string attribute_name;
	int column;
	int columnLength;
	int blockNum;
	Index() : column(0), blockNum(0) {}
};

class Condition {
public:
	Comparison op;
	int columnNum;
	string value;
};

struct RecordResult
{
	bool status;
	string Reason;
};

struct DATA
{
	vector<Row> ResultSet;
};

struct Row
{
	vector<string> DataField;
};

#endif
