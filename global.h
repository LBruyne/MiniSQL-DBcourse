/*
 * File name: gloabl.h
 * Author: Xuanming, Liu
 * Latest revised: 2020.06.13
 * Description: 
 * reference for project
**/

#ifndef _MINISQL_GLOBAL_H_
#define _MINISQL_GLOBAL_H_
#define  _CRT_SECURE_NO_WARNINGS
//#include <unistd.h>
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
#include <sstream>
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
typedef int BlockOffset;
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
	bool isPrimaryKey;
	bool isUnique;
	Attribute()
	{
		isPrimaryKey = false;
		isUnique = false;
	}
	Attribute(string n, int t, int l, bool isP, bool isU)
		:name(n), type(t), length(l), isPrimaryKey(isP), isUnique(isU) {}
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
	RecordResult() :status(1) {}
};
struct Row
{
	vector<string> DataField;
};

struct DATA
{
	vector<Row> ResultSet;
};
//写这个东西的原因：老师给的字符串的大小比较可能不按套路来
//比如 nber5 按道理应该 < nber100
//然而string的运算符是按照字典序来的  也就是nber100>nber5  
//替换工作量非常大，暂时先不动。
/*
class sqlString {
public:
	string Content;
	sqlString() {}
	sqlString(const string& copy) :Content(copy) {}
	sqlString& operator=(const sqlString& copy) {
		Content = copy.Content;
		return *this;
	}
	bool operator ==(const sqlString& cmp) {
		return Content == cmp.Content;
	}
	bool operator!=(const sqlString& cmp) {
		return !((*this) == cmp);
	}
	bool operator >(const sqlString& cmp) {
		return (Content.length() == cmp.Content.length() && Content > cmp.Content) || (Content.length() > cmp.Content.length());
	}
	bool operator <=(const sqlString& cmp) {
		return !(*this > cmp);
	}
	bool operator <(const sqlString& cmp) {
		return (cmp > * this);
	}
	bool operator <=(const sqlString& cmp) {
		return cmp >= *this;
	}
};
*/


#endif
