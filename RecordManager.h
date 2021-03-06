#ifndef __Record__Manager__header__
#define __Record__Manager__header__

#include "global.h"
#include "BufferManager.h"
#include "IndexManager.h"
//#include ""
using namespace std;


class RecordManager
{
public:
	DATA select(Table& T, vector<Condition>& con, char op = 'a');
	//select接口，默认谓词由and连接。可置为o 但是目前只支持全and或者全or操作

	RecordResult insert(Table& T, vector <string>& value);

	RecordResult deleteR (Table& T, vector<Condition>&con, char op='a');
	//delete接口，默认谓词由and连接。可置为o 但是目前只支持全and或者全or操作
	//以上为元组操作
	//以下为表操作
	using TableResult = RecordResult;
	TableResult create(Table& T);
	TableResult drop(const Table& T);
	void createIndex(Table&, Index&);

private:
	void SetCondition(bool&, vector<Condition>&,char);
	//针对or操作时，此函数将所有谓词取反。

	void ScanTable(Table&,DATA&, vector<Condition>&, bool negative=0);
	//此函数被select和delete调用。
	//select时，根据索引遍历数据表，挨个push满足条件的元组到返回值中。
	//delete时，根据索引遍历数据表，挨个删除满足条件的元组。
	//根据有无索引，索引性质和谓词进行分类，可以不同程度地减少判定次数
	//没有索引就从头到尾遍历
	//给出的参数是区分操作类型和谓词连接类型
	void ScanTable(Table&, RecordResult&, vector<Condition>&, bool negative = 0);
	inline bool comp(Table&,Condition&, Condition&);
	void MySort(Table&, vector<Condition> &);
	inline void splitRow(const string&, Row&, Table&);
	inline void testifyOneRow(Row&, vector<Condition>&, bool&, DATA&,Table&);
	inline bool testifyOneRow(Row&, vector<Condition>&, bool&,Table&);
	void deleteIndex(Table&, Row&);
	inline void insertIndex(Table&, Row&,const Pointer&);
	inline void insertIndex(Table&, Row&, const Pointer&,string&);
	inline void checkDuplicate(Table&, vector<string>&, RecordResult&);

};




#endif