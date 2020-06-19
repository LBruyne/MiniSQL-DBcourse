
#include"global.h"

class CatalogManager 
{
private:

	int tableNum;
	vector<Table> Tables;		//���ڴ洢table��Ϣ

	int indexNum;
	vector<Index> Indexes;		//���ڴ洢index��Ϣ	
	                	
	void readTable();			//���ļ��ж�ȡ

	void readIndex();

	void writeTable();			//д���ļ�

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

	void createIndex(Index index);

	void dropTable(Table table);

	void dropIndex(Index index);

	void dropTable(string table_name);

	void dropIndex(string index_name);

	bool ExistTable(string table_name);

	bool ExistIndex(string indexname);

	Table getTable_info(string table_name);

	Index getIndex_info(string index_name);

	int GetColumnNumber(Table& table, string columnname);

	int GetColumnAmount(Table& table);

	void ShowTableCatalog();

	void ShowIndexCatalog();
};

