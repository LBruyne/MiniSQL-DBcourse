#include "RecordManager.h"
#include "BufferManager.h"
#include "IndexManager.h"
extern BufferManager buffer;
extern IndexManager indexManager;
using namespace std;
//temporary name

//记得给tuple末尾加入'0'表示有效
//请限制一条tuple的长度不得超过4096Byte
RecordResult RecordManager::insert(Table& T, map <string, string>& value) {

	
	string row;
	for (auto attribute : T.attributes) {
		if (value.find(attribute.name)) {
			row.push_back(value(attribute.name));
		else
			row.push_back('*' * attribute.length);
		}
	}
	row += "0";

	//纠结是直接加在末尾好呢还是扫描看有无被删除的记录呢
	//扫描肯定也是费时间的，最好为每个schema维持一张已删除的表。。
	Page readIn;
	readIn.tableName = T.name;
	readIn.pageIndex = T.blockNum-1;
	//read in the last block
	//try if i can insert
	buffer.readPage(readIn);
	if (PAGE_SIZE-strlen(readIn.pageData ) <= T.totalLength)
	{
		strcat(readIn.pageData, row.c_str);
	}
	else
	{
		Page newPage=buffer.recordManagerGetBlankPage();
		newPage.tableName = T.name;
		strcpy(newPage.pageData, row.c_str);
		T.blockNum++;
		newPage.pageIndex = T.blockNum - 1;
	}
	buffer.writePage(readIn);
}

//以上为元组操作
//以下为表操作
using TableResult = RecordResult;
TableResult RecordManager::create(Table& T) {

}
TableResult RecordManager::drop(Table& T) {

}

void RecordManager::SetCondition(bool& negative, vector<Condition>& con) {
	negative = op == 'o';//后面的合取字句逻辑结果与之相与，其余操作就被合并了。
	if (negative) {
		for (auto each : con) {
			switch (each.op)
			{
			case Lt:each.op = Ge; break;
			case Le:each.op = Gt; break;
			case Eq:each.op = Ne; break;
			case Gt:each.op = Le; break;
			case Ge:each.op = Lt; break;
			}
		}
	}
	MySort(con.begin(), con.end(), comp);
	return;
}

bool RecordManager::comp(Table& T,Condition& a, Condition& b) {
	if ((T.attributes[a.columnNum].isUnique || T.attributes[a.columnNum].isPrimary) && T.attributes[a.columnNum].op == Eq)
		return 1;
	else if ((T.attributes[b.columnNum].isUnique || T.attributes[b.columnNum].isPrimary) && T.attributes[b.columnNum].op == Eq)
		return 0;
	//case 1 index on sth + op==Eq  
	//only need to test a few Conditions
	if ((T.attributes[a.columnNum].isUnique || T.attributes[a.columnNum].isPrimary) && T.attributes[a.columnNum].op != Ne)
		return 1;
	else if ((T.attributes[b.columnNum].isUnique || T.attributes[b.columnNum].isPrimary) && T.attributes[b.columnNum].op != Ne)
		return 0;
	//case 2 index on sth + op !=Ne
	//only need to conduct all Condition testing on some of records;
	if ((T.attributes[a.columnNum].isUnique || T.attributes[a.columnNum].isPrimary) && T.attributes[a.columnNum].op == Ne)
		return 1;
	else if ((T.attributes[b.columnNum].isUnique || T.attributes[b.columnNum].isPrimary) && T.attributes[b.columnNum].op == Ne)
		return 0;
	//case 3 index on sth + op ==Ne
	//conduct testing for all records;

	return 1;
	//final test case—— only have to test all Conditions to all records;
}

//non increasing order
void RecordManager::MySort(Table& T, vector<Condition>con) {
	for (size_t i = 0; i < con.size()-1; i++)
	{
		size_t max = i;
		for (size_t j = i+1; j < con.size(); j++)
		{
			if (comp(T, con[j], con[max]))
				max = j;
		}
		swap(con[max], con[i]);
	}
}

void RecordManager::ScanTable(Table& T,DATA &result, vector<Condition>& con, bool negative){
	if (con.empty()) {
		Page readIn;
		//readIn.pageType = RecordPage;
		readIn.tableName = T.name;
		for (readIn.pageIndex = 0; readIn.pageIndex< T.blockNum; readIn.pageIndex++)
		{
			buffer.readPage(readIn);
			//read in one block/page each time.
			string tempRow (readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * length;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				//separate the record into a string.
				Row oneRow;
				int ofsInOneLine=0;
				for (auto attribute : T.attributes) {
					oneRow.DataField.push_back(recordLine.substr(ofsInOneLinen, attribute.length));
					ofsInOneLine += attribute.length;
				}
				result.ResultSeT.push_back(oneRow);
			}
		}
		return;
	}//empty Condition set , select all records;
	//at least one of attributes to be checked 
	//are equipped with indexes
	//take measures to simplify all process.

	else if (T.attributes[con[0].columnNum].isUnique|| T.attributes[con[0].columnNum].isPrimary)
	{
		switch (con[0].op)
		{
		case Eq://fastest case
			
		case Ne:// speed 3rd case

		default :
			//speed 2nd case
		}
	}

	//no index && have Conditions.
	else
	{
		Page readIn;
		//readIn.pageType = RecordPage;
		readIn.tableName = T.name;
		for (readIn.pageIndex = 0; readIn.pageIndex < T.blockNum; readIn.pageIndex++)
		{
			buffer.readPage(readIn);
			//read in one block/page each time.
			string tempRow(readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * length;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				// if marked as deleted
				if (recordLine[recordLine.length - 1] == '1')
					continue;
				//separate the record into a string.
				Row oneRow;
				int ofsInOneLine = 0;
				for (auto attribute : T.attributes) {
					oneRow.DataField.push_back(recordLine.substr(ofsInOneLinen, attribute.length));
					ofsInOneLine += attribute.length;
				}
				//the code above separate one row ouT.
				int satisfy = 0;
				for (auto subclause : con)
				{
					switch (subclause.op)
					{
					case Eq:
						if (oneRow[subclause.columnNum] == subclause.value)
							satisfy++;
						break;
					case Ne:
						if (oneRow[subclause.columnNum] != subclause.value)
							satisfy++;
						break;
					case Gt:
						if (oneRow[subclause.columnNum] > subclause.value)
							satisfy++;
						break;
					case Ge:
						if (oneRow[subclause.columnNum] >= subclause.value)
							satisfy++;
						break;

					case Lt:
						if (oneRow[subclause.columnNum] < subclause.value)
							satisfy++;
						break;

					case Le:
						if (oneRow[subclause.columnNum] <= subclause.value)
							satisfy++;
						break;
					}
				}
				//if the operation is to select
				//check all valid tuples and then return matching DATA
					if (((satisfy==oneRow.DataField.size())&&!negative)||((satisfy!=oneRow.DataField.size())&&negative))
					{
						//when cons are connected by and we need to satisfy all Conditions
						//when connected by or, we must not st all Conditionss
							result.ResultSeT.push_back(oneRow);

					}
			}
		}
		if (op!='s')
		{
			buffer.writePage(readIn);
		}
		return;
	}
	//the worst case, no indexes are avalible.
}

void RecordManager::ScanTable(Table& T, RecordResult& deleteResult, vector<Condition>& con, bool negative) {
	Page readIn;
	//readIn.pageType = RecordPage;
	readIn.tableName = T.name;
	if (con.empty()) {
		for (readIn.pageIndex = 0; readIn.pageIndex < T.blockNum; readIn.pageIndex++)
		{
			buffer.readPage(readIn);
			//read in one block/page each time.
			int recordNum = strlen(readIn.pageData) / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * length;
				//ofs in a block, used to locate the entire record;
				readIn.pageData[positionInBlock + T.totalLength] = '1';
			}
		}
		deleteResult.status = 1;
		return;
	}
	//empty Condition set , delete all records;


	//at least one of attributes to be checked 
	//are equipped with indexes
	//take measures to simplify all process.
	else if (T.attributes[con[0].columnNum].isUnique || T.attributes[con[0].columnNum].isPrimary)
	{
		switch (con[0].op)
		{
		case Eq://fastest case

		case Ne:// speed 3rd case

		default:
			//speed 2nd case
		}
	}

	//no index && have Conditions.
	else
	{
		for (readIn.pageIndex = 0; readIn.pageIndex < T.blockNum; readIn.pageIndex++)
		{
			buffer.readPage(readIn);
			string tempRow(readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * length;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				Row oneRow;
				int ofsInOneLine = 0;
				for (auto attribute : T.attributes) {
					oneRow.DataField.push_back(recordLine.substr(ofsInOneLinen, attribute.length));
					ofsInOneLine += attribute.length;
				}
				//the code above separate one row ouT.
				int satisfy = 0;
				for (auto subclause : con)
				{
					switch (subclause.op)
					{
					case Eq:
						if (oneRow[subclause.columnNum] == subclause.value)
							satisfy++;
						break;
					case Ne:
						if (oneRow[subclause.columnNum] != subclause.value)
							satisfy++;
						break;
					case Gt:
						if (oneRow[subclause.columnNum] > subclause.value)
							satisfy++;
						break;
					case Ge:
						if (oneRow[subclause.columnNum] >= subclause.value)
							satisfy++;
						break;

					case Lt:
						if (oneRow[subclause.columnNum] < subclause.value)
							satisfy++;
						break;

					case Le:
						if (oneRow[subclause.columnNum] <= subclause.value)
							satisfy++;
						break;
					}
				}
				if (((satisfy == con.size()) && !negative) || ((satisfy != con.size()) && negative))
				{
					readIn.pageData[T.totalLength + positionInBlock] = '1';
				}
			}
		buffer.writePage(readIn);
		}
		buffer.writePage(readIn);
		deleteResult.status = 1;
		return;
	}
	//the worst case, no indexes are avalible.
}

DATA RecordManager::select(Table& T, vector<Condition>& con) {
	/*
	思路：
	若是or操作，则将所有Condition取反，验证合取字句的真假的取反，如此可以合并两种操作的处理过程。
	*/
	bool negative = 0;
	DATA result;
	//will only output all tuples that satisfy the Conditions.
	//return in DATA pattern.
	negative = op == 'a' ? 1 : 0;
	SetCondition(negative, con);
	ScanTable(T, result, con, negative);
	return result;


}

RecordResult RecordManager::deleteR (Table& T, vector<Condition> &con,char op) {
	bool negative = 0;
	RecordResult deleteResult;
	//will only output all tuples that satisfy the Conditions.
	//return in DATA pattern.
	negative = op == 'a' ? 1 : 0;
	SetCondition(negative, con);
	ScanTable(T, deleteResult, con, negative);
	//maybe should set status to sth but not yet figured out.
	return result;
}