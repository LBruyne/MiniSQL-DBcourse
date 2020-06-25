## Catalog

类定义：

```
class CatalogManager 
{
private:

	int tableNum;
	vector<Table> Tables;				//用于存储table信息
	int indexNum;
	vector<Index> Indexes;				//用于存储index信息	              	
	void readTable();					//从文件中读取
	void readIndex();
	void writeTable();					//写回文件
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

	void createTable(Table& table);				//创建表格
	void createIndex(Index index);				//创建索引
	void dropTable(Table table);				//删除表格
	void dropIndex(Index index);				//删除索引
	void dropTable(string table_name);			
	void dropIndex(string index_name);
	bool ExistTable(string table_name);			//判断是否已经有该表格
	bool ExistIndex(string indexname);			//判断是否已经有该索引
	Table getTable_info(string table_name);		//返回表格信息
	Index getIndex_info(string index_name);		//返回索引信息
	int GetColumnNumber(Table& table, string columnname);	//返回对应属性所在的位置
	int GetColumnAmount(Table& table);			//返回表格的属性数量
	void ShowTableCatalog();					
	void ShowIndexCatalog();
};
```

注：

​	很大程度都是参照样例来的，太难了

​	我本来自己写了一个Catalog，每个表格一个文件，每个索引一个文件，但是读取和存储太麻烦了，把自己写崩了

​	就，很大程度上借鉴了样例



工作原理：

​	构造函数用于从磁盘文件中读取已有的表格和索引信息，保存到容器里；

​	相应的子函数的操作是对容器的操作；

​	析构函数是把容器中的信息重新写回磁盘文件。



接口：

​	感觉这个接口我不知道该怎么写，，

​	因为每次构造一个CatalogManager cat都可以用cat.dropTable(table)等函数

​	感觉最大的接口连接在于和interpreter的表格和索引的建立和删除



有什么需要的联系我