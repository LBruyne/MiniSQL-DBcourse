#include "RecordManager.h"
#include "BufferManager.h"
#include "IndexManager.h"
extern BufferManager buf;
extern IndexManager index;
using namespace std;
//temporary name

//记得给tuple末尾加入'0'表示有效
//请限制一条tuple的长度不得超过4096Byte
//为了简单起见 忽略了null的情况  改成了每个属性必定存在
RecordResult RecordManager::insert(Table& T, vector<string>& value) {

	RecordResult insertResult;
	string row;
	Row onerow;
	for (auto attribute : value) 
		row+=attribute;
	row += "0";

	//纠结是直接加在末尾好呢还是扫描看有无被删除的记录呢
	//扫描肯定也是费时间的，最好为每个schema维持一张已删除的表。。
	Page &readIn =buf.recordManagerGetBlankPage();
	readIn.tableName = T.name;
	readIn.ofs = T.blockNum;
	//readIn.pageIndex = T.blockNum-1;
	//read in the last block
	//try if i can insert
	buf.readPage(readIn);
	checkDuplicate(T, value,insertResult);
	if (!insertResult.status)
		return insertResult;
	if (PAGE_SIZE-strlen(readIn.pageData ) <= T.totalLength)
	{
		splitRow(row, onerow, T);
		insertIndex(T, onerow, (T.blockNum-1)*T.totalLength+strlen(readIn.pageData)+row.length());
		strcat(readIn.pageData, row.c_str());
	}
	else
	{
		bool flagSuccess = 0;
		string block(readIn.pageData);
		for (size_t i = 0; i < block.length(); i+=T.totalLength)
		{
			if (block[i + T.totalLength - 1] == '0')
				continue;
			else {
				strcpy(readIn.pageData + i, row.c_str());
				splitRow(block.substr(i, T.totalLength), onerow, T);
				insertIndex(T, onerow, (T.blockNum -1)* T.totalLength + i);
				flagSuccess = 1;
				break;
			}
		}
		if (!flagSuccess) {
			//开出一片新的块，存这个元组。
			readIn.pageType = PageType::Undefined;
			readIn.pageIndex = -1;
			readIn.ofs = T.blockNum;
			buf.readPage(readIn);
			strcat(readIn.pageData, row.c_str());
			insertIndex(T, onerow, (T.blockNum-1 )* T.totalLength);
		}
		
	}
	buf.writePage(readIn);
	return insertResult;
}

DATA RecordManager::select(Table& T, vector<Condition>& con, char op) {
	/*
	思路：
	若是or操作，则将所有Condition取反，验证合取字句的真假的取反，如此可以合并两种操作的处理过程。
	*/
	bool negative = 0;
	DATA result;
	//will only output all tuples that satisfy the Conditions.
	//return in DATA pattern.
	negative = (op == 'a') ? 1 : 0;
	SetCondition(negative, con,op);
	MySort(T, con);
	ScanTable(T, result, con, negative);
	return result;


}

RecordResult RecordManager::deleteR(Table& T, vector<Condition>& con, char op) {
	bool negative = 0;
	RecordResult deleteResult;
	//will only output all tuples that satisfy the Conditions.
	//return in DATA pattern.
	negative = (op == 'a') ? 1 : 0;
	SetCondition(negative, con,op);
	MySort(T, con);
	ScanTable(T, deleteResult, con, negative);
	//maybe should set status to sth but not yet figured out.
	return deleteResult;
}

//以上为元组操作
//以下为表操作
using TableResult = RecordResult;
TableResult RecordManager::create(Table& T) {
	TableResult res;
	return res;
}

TableResult RecordManager::drop(Table& T) {
	TableResult res;
	return res;
}
//
//以下为内部接口
//

void RecordManager::SetCondition(bool& negative, vector<Condition>& con, char op = 'a') {
	negative = (op == 'o');//后面的合取字句逻辑结果与之相与，其余操作就被合并了。
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
	return;
}

void RecordManager::splitRow(const string& tuple, Row& row, Table& T) {
	int ofsInOneLine = 0;
	for (auto attribute : T.attributes) {
		row.DataField.push_back(tuple.substr(ofsInOneLine, attribute.length));
		ofsInOneLine += attribute.length;
	}
}

void RecordManager::testifyOneRow(Row& oneRow, vector<Condition>& con, bool& negative, DATA& result) {
	int satisfy = 0;
	for (auto singleCon : con) {
		bool flag = 0;
		switch (singleCon.op)
		{
		case Eq:
			if (oneRow.DataField[singleCon.columnNum] == singleCon.value)
				satisfy++, flag = 1;
			break;
		case Ne:
			if (oneRow.DataField[singleCon.columnNum] != singleCon.value)
				satisfy++, flag = 1;
			break;
		case Gt:
			if (oneRow.DataField[singleCon.columnNum] > singleCon.value)
				satisfy++, flag = 1;
			break;
		case Ge:
			if (oneRow.DataField[singleCon.columnNum] >= singleCon.value)
				satisfy++, flag = 1;
			break;

		case Lt:
			if (oneRow.DataField[singleCon.columnNum] < singleCon.value)
				satisfy++, flag = 1;
			break;

		case Le:
			if (oneRow.DataField[singleCon.columnNum] <= singleCon.value)
				satisfy++, flag = 1;
			break;
		}
		if (!flag)
		{
			break;
		}
		satisfy = 0;
	}

	if ((satisfy == con.size()) && !negative || (satisfy != con.size()) && negative)
	{
		result.ResultSet.push_back(oneRow);
	}

}

bool RecordManager::testifyOneRow(Row& oneRow, vector<Condition>& con, bool& negative) {
	int satisfy = 0;
	for (auto singleCon : con) {
		bool flag = 0;
		switch (singleCon.op)
		{
		case Eq:
			if (oneRow.DataField[singleCon.columnNum] == singleCon.value)
				satisfy++, flag = 1;
			break;
		case Ne:
			if (oneRow.DataField[singleCon.columnNum] != singleCon.value)
				satisfy++, flag = 1;
			break;
		case Gt:
			if (oneRow.DataField[singleCon.columnNum] > singleCon.value)
				satisfy++, flag = 1;
			break;
		case Ge:
			if (oneRow.DataField[singleCon.columnNum] >= singleCon.value)
				satisfy++, flag = 1;
			break;

		case Lt:
			if (oneRow.DataField[singleCon.columnNum] < singleCon.value)
				satisfy++, flag = 1;
			break;

		case Le:
			if (oneRow.DataField[singleCon.columnNum] <= singleCon.value)
				satisfy++, flag = 1;
			break;
		}
		if (!flag)
		{
			break;
		}
		satisfy = 0;
	}
	if ((satisfy == con.size()) && !negative || (satisfy != con.size()) && negative)
	{
		return 1;
	}
	return	0;
}

bool RecordManager::comp(Table& T, Condition& a, Condition& b) {
	if ((index.HasIndex(T.name, T.attributes[a.columnNum].name)) && a.op == Eq)
		return 1;
	else if ((index.HasIndex(T.name, T.attributes[b.columnNum].name)) && b.op == Eq)
		return 0;
	//case 1 index on sth + op==Eq  
	//only need to test a few Conditions
	if ((index.HasIndex(T.name, T.attributes[a.columnNum].name)) && a.op != Ne)
		return 1;
	else if ((index.HasIndex(T.name, T.attributes[b.columnNum].name)) && b.op != Ne)
		return 0;
	//case 2 index on sth + op !=Ne
	//only need to conduct all Condition testing on some of records;
	if ((index.HasIndex(T.name, T.attributes[a.columnNum].name)) && a.op == Ne)
		return 1;
	else if ((index.HasIndex(T.name, T.attributes[b.columnNum].name)) && b.op == Ne)
		return 0;
	//case 3 index on sth + op ==Ne
	//conduct testing for all records;

	return 1;
	//final test case—— only have to test all Conditions to all records;
}

//non increasing order
void RecordManager::MySort(Table& T, vector<Condition>& con) {
	for (size_t i = 0; i < con.size() - 1; i++)
	{
		size_t max = i;
		for (size_t j = i + 1; j < con.size(); j++)
		{
			if (comp(T, con[j], con[max]))
				max = j;
		}
		swap(con[max], con[i]);
	}
}

//this one is for selecting
void RecordManager::ScanTable(Table& T, DATA& result, vector<Condition>& con, bool negative) {
	if (con.empty()) {
		Page& readIn = buf.recordManagerGetBlankPage();
		//readIn.pageType = RecordPage;
		readIn.tableName = T.name;
		for (readIn.ofs = 0; readIn.ofs< T.blockNum; readIn.ofs++)
		{
			buf.readPage(readIn);
			//read in one block/page each time.
			string tempRow(readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length() / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * T.totalLength;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				//separate the record into a string.
				Row oneRow;
				splitRow(recordLine, oneRow, T);
				result.ResultSet.push_back(oneRow);
			}
		}
		return;
	}//empty Condition set , select all records;
	//at least one of attributes to be checked 
	//are equipped with indexes
	//take measures to simplify all process.

	else if (index.HasIndex(T.name, T.attributes[con[0].columnNum].name))//(T.attributes[con[0].columnNum].isUnique|| T.attributes[con[0].columnNum].isPrimeryKey//))
	{
		Page& checkPage = buf.recordManagerGetBlankPage();
		checkPage.tableName = T.name;
		string tuple;
		Row oneRow;
		int satisfy = 0;
		index.LoadIndex(T.name, T.attributes[con[0].columnNum].name, T.attributes[con[0].columnNum].type);
		vector<int>blockOfs, inBlockOfs;
		Pointer singleBlock, singleBlockOfs;
		switch (con[0].op)
		{
		case Eq://fastest case
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->Find(stoi(con[0].value), singleBlock);
				break;
			case CHAR:
				index.bpt_STRING->Find(con[0].value, singleBlock);
				break;
			case FLOAT:
				index.bpt_FLOAT->Find(atof(con[0].value.c_str()), singleBlock);
				break;
			}
			singleBlockOfs = singleBlock % PAGE_SIZE;
			singleBlock /= PAGE_SIZE;
			if (!negative) {//如果是合取操作，则将记录取出来 仅仅测试这一条记录即可
				checkPage.ofs = singleBlock;
				buf.readPage(checkPage);
				tuple = checkPage.pageData;
				tuple = tuple.substr(singleBlockOfs, T.totalLength);
				splitRow(tuple, oneRow, T);
				testifyOneRow(oneRow, con, negative, result);
				oneRow.DataField.clear();
			}
			else
			{
				//当析取操作时，必须遍历所有的记录。
				for (size_t i = 0; i < T.blockNum; i++)
				{
					checkPage.ofs = i;
					buf.readPage(checkPage);
					string all(checkPage.pageData);
					for (size_t j = 0; j < all.length(); j += T.totalLength)
					{
						splitRow(all.substr(j, T.totalLength), oneRow, T);
						testifyOneRow(oneRow, con, negative, result);
						oneRow.DataField.clear();
					}
				}
			}
			// speed 2nd case	
			//  > >= < <=
			break;

		// speed 2nd case	
		//  > >= < <=
		case Ge:case Gt:
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->FindLarger(stoi(con[0].value), con[0].op == Ge, inBlockOfs);
				break;
			case CHAR:
				index.bpt_STRING->FindLarger(con[0].value, con[0].op == Ge, inBlockOfs);
				break;
			case FLOAT:
				index.bpt_FLOAT->FindLarger(atof(con[0].value.c_str()), con[0].op == Ge, inBlockOfs);
				break;
			}
			sort(inBlockOfs.begin(), inBlockOfs.end());
			for (auto iter : inBlockOfs) {
				blockOfs.push_back(iter / PAGE_SIZE);
				iter %= iter % PAGE_SIZE;
			}

			for (size_t i = 0; i < inBlockOfs.size(); i++)
			{
				checkPage.ofs = blockOfs[i];
				buf.readPage(checkPage);
				//已经读入对应的块了
				tuple = checkPage.pageData;
				tuple = tuple.substr(inBlockOfs[i], T.totalLength);
				splitRow(tuple, oneRow, T);
				testifyOneRow(oneRow, con, negative, result);
				oneRow.DataField.clear();
			}

			oneRow.DataField.clear();
			if (negative)//如果是or 则B+树底层左侧的全部都要push进来
			{
				inBlockOfs.clear(); blockOfs.clear();
				switch (T.attributes[con[0].columnNum].type)
				{
				case INT:
					index.bpt_INT->FindLess(stoi(con[0].value), !(con[0].op == Ge), inBlockOfs);
					break;
				case CHAR:
					index.bpt_STRING->FindLess(con[0].value, !(con[0].op == Ge), inBlockOfs);
					break;
				case FLOAT:
					index.bpt_FLOAT->FindLess(atof(con[0].value.c_str()), !(con[0].op == Ge), inBlockOfs);
					break;
				}
				sort(inBlockOfs.begin(), inBlockOfs.end());
				for (auto iter : inBlockOfs) {
					blockOfs.push_back(iter / PAGE_SIZE);
					iter %= iter % PAGE_SIZE;
				}

				for (size_t i = 0; i < inBlockOfs.size(); i++)
				{
					checkPage.ofs = blockOfs[i];
					buf.readPage(checkPage);
					//已经读入对应的块了
					tuple = checkPage.pageData;
					tuple = tuple.substr(inBlockOfs[i], T.totalLength);
					splitRow(tuple, oneRow, T);
					result.ResultSet.push_back(oneRow);
					oneRow.DataField.clear();
				}
			}
			break;
		case Lt: case Le:
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->FindLess(stoi(con[0].value), con[0].op == Le, inBlockOfs);
				break;
			case CHAR:
				index.bpt_STRING->FindLess(con[0].value, con[0].op == Le, inBlockOfs);
				break;
			case FLOAT:
				index.bpt_FLOAT->FindLess(atof(con[0].value.c_str()), con[0].op == Le, inBlockOfs);
				break;
			}
			sort(inBlockOfs.begin(), inBlockOfs.end());
			for (auto iter : inBlockOfs) {
				blockOfs.push_back(iter / PAGE_SIZE);
				iter %= iter % PAGE_SIZE;
			}

			for (size_t i = 0; i < inBlockOfs.size(); i++)
			{
				checkPage.ofs = blockOfs[i];
				buf.readPage(checkPage);
				//已经读入对应的块了
				tuple = checkPage.pageData;
				tuple = tuple.substr(inBlockOfs[i], T.totalLength);
				splitRow(tuple, oneRow, T);
				testifyOneRow(oneRow, con, negative, result);
				oneRow.DataField.clear();
			}
			oneRow.DataField.clear();
			if (negative)
			{
				blockOfs.clear(); inBlockOfs.clear();
				switch (T.attributes[con[0].columnNum].type)
				{
				case INT:
					index.bpt_INT->FindLarger(stoi(con[0].value), !(con[0].op == Le), inBlockOfs);
					break;
				case CHAR:
					index.bpt_STRING->FindLarger(con[0].value, !(con[0].op == Le), inBlockOfs);
					break;
				case FLOAT:
					index.bpt_FLOAT->FindLarger(atof(con[0].value.c_str()), !(con[0].op == Le), inBlockOfs);
					break;
				}
				sort(inBlockOfs.begin(), inBlockOfs.end());
				for (auto iter : inBlockOfs) {
					blockOfs.push_back(iter / PAGE_SIZE);
					iter %= iter % PAGE_SIZE;
				}

				for (size_t i = 0; i < inBlockOfs.size(); i++)
				{
					checkPage.ofs = blockOfs[i];
					buf.readPage(checkPage);
					//已经读入对应的块了
					tuple = checkPage.pageData;
					tuple = tuple.substr(inBlockOfs[i], T.totalLength);
					splitRow(tuple, oneRow, T);
					result.ResultSet.push_back(oneRow);
					oneRow.DataField.clear();
				}
			}

			break;
		default:
			//speed 3rd case
			//really need to scan the whole table, test every condition.
			while (buf.readPage(checkPage)) {
				string thisPage = checkPage.pageData;
				for (size_t i = 0; i < thisPage.length(); i += T.totalLength)
				{
					splitRow(thisPage.substr(i, T.totalLength), oneRow, T);
					testifyOneRow(oneRow, con, negative, result);
					oneRow.DataField.clear();
				}
				checkPage.ofs++;
			}
		}
	}

	//no index && have Conditions.
	else
	{
	Page& readIn = buf.recordManagerGetBlankPage();
		//readIn.pageType = RecordPage;
		readIn.tableName = T.name;
		for (readIn.ofs= 0; readIn.ofs< T.blockNum; readIn.ofs++)
		{
			buf.readPage(readIn);
			//read in one block/page each time.
			string tempRow(readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length() / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * T.totalLength;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				// if marked as deleted
				if (recordLine[recordLine.length() - 1] == '1')
					continue;
				//separate the record into a string.
				Row oneRow;
				splitRow(recordLine, oneRow, T);
				testifyOneRow(oneRow, con, negative, result);
				}
				//the code above separate one row ouT.
				
				//if the operation is to select
				//check all valid tuples and then return matching DATA
			}
		}


		return;
}
	//the worst case, no indexes are avalible.

//this one is used to delete
void RecordManager::ScanTable(Table& T, RecordResult& deleteResult, vector<Condition>& con, bool negative) {
	Page& readIn = buf.recordManagerGetBlankPage();
	//readIn.pageType = RecordPage;
	readIn.tableName = T.name;
	if (con.empty()) {
		//将表删干净，索引也得删干净。因此先drop所有的，再重建。
		for (auto single : T.attributes) {
			if (index.HasIndex(T.name, single.name))
			{
				index.DropIndex(T.name, single.name);
			}
		}
		for (; readIn.ofs < T.blockNum; readIn.ofs++)
		{
			buf.readPage(readIn);

			//read in one block/page each time.
			int recordNum = strlen(readIn.pageData) / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * T.totalLength;
				//ofs in a block, used to locate the entire record;
				readIn.pageData[positionInBlock + T.totalLength] = '1';
				
			}
			buf.writePage(readIn);
		}
		deleteResult.status = 1;
		index.CreateIndex(T);
	}
	//empty Condition set , delete all records;


	//at least one of attributes to be checked 
	//are equipped with indexes
	//take measures to simplify all process.
	else if (index.HasIndex(T.name, T.attributes[con[0].columnNum].name))// (T.attributes[con[0].columnNum].isUnique || T.attributes[con[0].columnNum].isPrimeryKey)
	{
		Page& checkPage = buf.recordManagerGetBlankPage();
		checkPage.tableName = T.name;
		string tuple;
		Row oneRow;
		int satisfy = 0;
		index.LoadIndex(T.name, T.attributes[con[0].columnNum].name, T.attributes[con[0].columnNum].type);
		vector<int>blockOfs, inBlockOfs;
		Pointer singleBlock,singleBlockOfs;
		switch (con[0].op)
		{
		case Eq://contains the fastest case
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->Find(stoi(con[0].value),singleBlock);
				break;
			case CHAR:
				index.bpt_STRING->Find(con[0].value,  singleBlock);
				break;
			case FLOAT:
				index.bpt_FLOAT->Find(atof(con[0].value.c_str()), singleBlock);
				break;
			}
			singleBlockOfs = singleBlock % PAGE_SIZE;
			singleBlock /= PAGE_SIZE;
			if (!negative) {//如果是合取操作，则将记录取出来 删掉即可
				checkPage.ofs = singleBlock;
				buf.readPage(checkPage);
				tuple = checkPage.pageData;
				tuple = tuple.substr(singleBlockOfs, T.totalLength);
				splitRow(tuple, oneRow, T);
				if (testifyOneRow(oneRow, con, negative)) {
					checkPage.pageData[singleBlockOfs + T.totalLength] = '1';
					deleteIndex(T, oneRow);
				}
				buf.writePage(checkPage);
			}
			else
			{
				//当析取操作时，必须遍历所有的记录。
				for (size_t i = 0; i < T.blockNum; i++)
				{
					checkPage.ofs = i;
					buf.readPage(checkPage);
					string all(checkPage.pageData);
					for (size_t j = 0; j < all.length(); j+=T.totalLength)
					{
						splitRow(all.substr(j, T.totalLength), oneRow, T);
						if (!(checkPage.ofs==singleBlock&&singleBlockOfs==j))
						{
							checkPage.pageData[j + T.totalLength] = '1';

							//因为后面会取反，此处已经有表达式=false 
						}
						else
						{
							if (testifyOneRow(oneRow, con, negative))
								checkPage.pageData[j + T.totalLength] = '1';
							
						}
						deleteIndex(T, oneRow);
						oneRow.DataField.clear();
					}
					buf.writePage(checkPage);
				}
			}
		// speed 2nd case	
		//  > >= < <=
			break;
		case Ge:case Gt:
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->FindLarger(stoi(con[0].value), con[0].op == Ge, inBlockOfs);
				break;
			case CHAR:
				index.bpt_STRING->FindLarger(con[0].value, con[0].op == Ge, inBlockOfs);
				break;
			case FLOAT:
				index.bpt_FLOAT->FindLarger(atof(con[0].value.c_str()), con[0].op == Ge, inBlockOfs);
				break;
			}
			sort(inBlockOfs.begin(), inBlockOfs.end());
			for (auto iter : inBlockOfs) {
				blockOfs.push_back(iter % PAGE_SIZE);
				iter %= iter / PAGE_SIZE;
			}

			for (size_t i = 0; i < inBlockOfs.size(); i++)
			{
				checkPage.ofs = blockOfs[i];
				buf.readPage(checkPage);
				//已经读入对应的块了
				tuple = checkPage.pageData;
				splitRow(tuple.substr(inBlockOfs[i], T.totalLength), oneRow, T);
				if (testifyOneRow(oneRow, con, negative))
				{
					//标记为已删除
					tuple[inBlockOfs[i] + T.totalLength] = '1';
					deleteIndex(T, oneRow);
				}
				oneRow.DataField.clear();
				buf.writePage(checkPage);
			}
			if (negative)//如果是or 则B+树底层左侧的全部都要push进来
			{
				blockOfs.clear(); inBlockOfs.clear();
				switch (T.attributes[con[0].columnNum].type)
				{
				case INT:
					index.bpt_INT->FindLess(stoi(con[0].value), !(con[0].op == Ge), inBlockOfs);
					break;
				case CHAR:
					index.bpt_STRING->FindLess(con[0].value, !(con[0].op == Ge), inBlockOfs);
					break;
				case FLOAT:
					index.bpt_FLOAT->FindLess(atof(con[0].value.c_str()), !(con[0].op == Ge), inBlockOfs);
					break;
				}
				sort(inBlockOfs.begin(), inBlockOfs.end());
				for (auto iter : inBlockOfs) {
					blockOfs.push_back(iter % PAGE_SIZE);
					iter %= iter % PAGE_SIZE;
				}
				for (size_t i = 0; i < inBlockOfs.size(); i++)
				{
					checkPage.ofs = blockOfs[i];
					buf.readPage(checkPage);
					//已经读入对应的块了
					tuple = checkPage.pageData;
					splitRow(tuple.substr(inBlockOfs[i], T.totalLength), oneRow, T);
					if (testifyOneRow(oneRow, con, negative))
					{
						//标记为已删除
						tuple[inBlockOfs[i] + T.totalLength] = '1';
						deleteIndex(T, oneRow);

					}
					oneRow.DataField.clear();
					buf.writePage(checkPage);
				}
			}
			break;
		case Lt: case Le:
			switch (T.attributes[con[0].columnNum].type)
			{
			case INT:
				index.bpt_INT->FindLess(stoi(con[0].value), con[0].op == Le, inBlockOfs);
				break;
			case CHAR:
				index.bpt_STRING->FindLess(con[0].value, con[0].op == Le, inBlockOfs);
				break;
			case FLOAT:
				index.bpt_FLOAT->FindLess(atof(con[0].value.c_str()), con[0].op == Le, inBlockOfs);
				break;
			}
			sort(inBlockOfs.begin(), inBlockOfs.end());
			for (auto iter : inBlockOfs) {
				blockOfs.push_back(iter % PAGE_SIZE);
				iter %= iter % PAGE_SIZE;
			}

			for (size_t i = 0; i < inBlockOfs.size(); i++)
			{
				checkPage.ofs = blockOfs[i];
				buf.readPage(checkPage);
				//已经读入对应的块了
				tuple = checkPage.pageData;
				splitRow(tuple.substr(inBlockOfs[i], T.totalLength), oneRow, T);
				if (testifyOneRow(oneRow, con, negative)) {
					tuple[inBlockOfs[i] + T.totalLength] = '1';
					deleteIndex(T, oneRow);
				}
				buf.writePage(checkPage);
				oneRow.DataField.clear();
			}
			inBlockOfs.clear(); blockOfs.clear();
			if (negative)
			{
				//右侧全push
				switch (T.attributes[con[0].columnNum].type)
				{
				case INT:
					index.bpt_INT->FindLarger(stoi(con[0].value), !(con[0].op == Le), inBlockOfs);
					break;
				case CHAR:
					index.bpt_STRING->FindLarger(con[0].value, !(con[0].op == Le), inBlockOfs);
					break;
				case FLOAT:
					index.bpt_FLOAT->FindLarger(atof(con[0].value.c_str()), !(con[0].op == Le), inBlockOfs);
					break;
				}
				sort(inBlockOfs.begin(), inBlockOfs.end());
				for (auto iter : inBlockOfs) {
					blockOfs.push_back(iter % PAGE_SIZE);
					iter %= iter / PAGE_SIZE;
				}
				for (size_t i = 0; i < inBlockOfs.size(); i++)
				{
					//即使是同一个块也无妨，buffer manager会自己处理
					checkPage.ofs = blockOfs[i];
					buf.readPage(checkPage);
					//已经读入对应的块了
					tuple = checkPage.pageData;
					splitRow(tuple.substr(inBlockOfs[i], T.totalLength), oneRow, T);
					if (testifyOneRow(oneRow, con, negative))
					{
						//标记为已删除
						tuple[inBlockOfs[i] + T.totalLength] = '1';
						deleteIndex(T, oneRow);

					}
					oneRow.DataField.clear();
					buf.writePage(checkPage);
				}
			}

			break;
		}
	}
	//no index && have Conditions.
	else
	{
		for (readIn.ofs= 0; readIn.ofs < T.blockNum; readIn.ofs++)
		{
			buf.readPage(readIn);
			string tempRow(readIn.pageData);
			// c string is horrible! transform to string.
			int recordNum = tempRow.length() / T.totalLength;
			//calc num of records in this page.
			for (int offset = 0; offset < recordNum; offset++)
			{
				int positionInBlock = offset * T.totalLength;
				//ofs in a block, used to locate the entire record;
				string recordLine = tempRow.substr(positionInBlock, T.totalLength);
				Row oneRow;
				splitRow(recordLine, oneRow, T);
				//the code above separate one row ouT.
				if (testifyOneRow(oneRow, con, negative))
				{
					readIn.pageData[T.totalLength + positionInBlock] = '1';
					deleteIndex(T, oneRow);
				}
			}
			buf.writePage(readIn);
		}
		deleteResult.status = 1;
		
		
	}
	//the worst case, no indexes are avalible.
	return;
}

void RecordManager::deleteIndex(Table& T, Row& row) {
	for (size_t i = 0; i < row.DataField.size(); i++)
	{
		if (index.HasIndex(T.name, T.attributes[i].name))
		{
			if (index.bptIntName==T.attributes[i].name){
				index.DeleteItem(row.DataField[i], INT);
			}
			else if (index.bptFloatName == T.attributes[i].name) {
				index.DeleteItem(row.DataField[i], FLOAT);

			}
			else if (index.bptStringName == T.attributes[i].name) {
				index.DeleteItem(row.DataField[i], CHAR);
			}
			else
			{
				index.LoadIndex(T.name, T.attributes[i].name, T.attributes[i].type);
				index.DeleteItem(row.DataField[i], T.attributes[i].type);
			}
		}
	}
}

void RecordManager::insertIndex(Table& T, Row& row, const Pointer& ptr) {
	for (size_t i = 0; i < row.DataField.size(); i++)
	{
		if (index.HasIndex(T.name, T.attributes[i].name))
		{
			if (index.bptIntName == T.attributes[i].name) {
				index.InsertItem(row.DataField[i],ptr, INT);
			}
			else if (index.bptFloatName == T.attributes[i].name) {
				index.InsertItem(row.DataField[i], ptr, FLOAT);

			}
			else if (index.bptStringName == T.attributes[i].name) {
				index.InsertItem(row.DataField[i], ptr, CHAR);
			}
			else
			{
				index.LoadIndex(T.name, T.attributes[i].name, T.attributes[i].type);
				index.InsertItem(row.DataField[i], ptr, T.attributes[i].type);
			}
		}
	}
}

void RecordManager::checkDuplicate(Table&T, vector<string>& value, RecordResult& res) {
	int i = 0;
	for (auto attribute : T.attributes) {
		if (index.HasIndex(T.name, attribute.name) && (index.Find(value[i++], attribute.type)))
		{
			res.status = 0;
			res.Reason = "Duplicate value for  \'" + T.name + "." + attribute.name + "\' !!";
			return;
		}
	}
}

