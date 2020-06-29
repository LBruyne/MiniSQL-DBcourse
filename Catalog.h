
#include"global.h"

class CatalogManager 
{
private:

	int tableNum;
	vector<Table> Tables;		//用于存储table信息

	int indexNum;
	vector<Index> Indexes;		//用于存储index信息	
	                	
	void readTable();			//从文件中读取

	void readIndex();

	void writeTable();			//写回文件

	void writeIndex();

public:
	CatalogManager() 
	{
		readTable();
		readIndex();
	}

	~CatalogManager() 
	{
		writeTable();
		writeIndex();
	}

	void createTable(Table& table);

	void changeTable(Table& table);

	void createIndex(Index index);

	void dropTable(Table table);

	void dropIndex(Index index);

	void dropTable(string table_name);

	void dropIndex(string index_name);

	bool ExistTable(string table_name);

	bool ExistIndex(string indexname);

	Table getTable_info(string table_name);

	Index getIndex_info(string index_name);

	int getAttr_no(Table& table, string attr_name);

	int getAttr_size(Table& table);

	void ShowTableCatalog();

	void ShowIndexCatalog();

	void clear();

	void reload();
};

