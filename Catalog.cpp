#include"Catalog.h"
#include "IndexManager.h"
extern IndexManager index;
void CatalogManager::readTable() 
{
	const string filename = "table.catlog";
	fstream  fin(filename.c_str(), ios::in);
	fin >> tableNum;
	for (int i = 0; i < tableNum; i++)
	{
		Table tab;
		fin >> tab.name;
		fin >> tab.attriNum;
		fin >> tab.blockNum;
		for (int j = 0; j < tab.attriNum; j++)
		{
			Attribute attr;
			fin >> attr.name;
			fin >> attr.type;
			fin >> attr.length;
			fin >> attr.isPrimaryKey;
			fin >> attr.isUnique;
			tab.attributes.push_back(attr);
			tab.totalLength += attr.length;
		}
		tab.totalLength++;
		Tables.push_back(tab);
	}
	fin.close();
}

void CatalogManager::readIndex() 
{
	const string filename = "index.catlog";
	fstream  fin(filename.c_str(), ios::in);
	fin >> indexNum;
	for (int i = 0; i < indexNum; i++)
	{
		Index index;
		fin >> index.index_name;
		fin >> index.table_name;
		fin >> index.attribute_name;
		fin >> index.column;
		fin >> index.columnLength;
		fin >> index.blockNum;
		Indexes.push_back(index);
	}
	fin.close();
}

void CatalogManager::writeTable() 
{
	string filename = "table.catlog";
	fstream  fout(filename.c_str(), ios::out);

	fout << tableNum << endl;
	for (int i = 0; i < tableNum; i++)
	{
		fout << Tables[i].name << " ";
		fout << Tables[i].attriNum << " ";
		fout << Tables[i].blockNum << endl;

		for (int j = 0; j < Tables[i].attriNum; j++)
		{
			fout << Tables[i].attributes[j].name << " ";
			fout << Tables[i].attributes[j].type << " ";
			fout << Tables[i].attributes[j].length << " ";
			fout << Tables[i].attributes[j].isUnique << " ";
			fout << Tables[i].attributes[j].isPrimaryKey << endl;
		}
	}
	fout.close();
}

void CatalogManager::writeIndex() 
{
	string filename = "index.catlog";
	fstream  fout(filename.c_str(), ios::out);

	fout << indexNum << endl;
	for (int i = 0; i < indexNum; i++)
	{
		fout << Indexes[i].index_name << " ";
		fout << Indexes[i].table_name << " ";
		fout << Indexes[i].attribute_name << " ";
		fout << Indexes[i].column << " ";
		fout << Indexes[i].columnLength << " ";
		fout << Indexes[i].blockNum << endl;
	}
	fout.close();
}

void CatalogManager::createTable(Table& table)
{
	if (ExistTable(table.name) == true)
	{
		cout << "The Table named " << table.name << " has existed." << endl;
		//throw
	}
	else
	{
		tableNum++;
		//for (int i = 0; i < table.attributes.size(); i++) 
		//	table.totalLength += table.attributes[i].length;
		//table.totalLength += 1;//最后一位是有效位
		Tables.push_back(table);
	}
	index.CreateIndex(table);
	//Index newindex;
	//newindex.table_name = table.name;
	//for (size_t i = 0; i < table.attributes.size(); i++)
	//{
	//	if (table.attributes[i].isPrimaryKey)
	//	{
	//		newindex.index_name = table.attributes[i].name;
	//		newindex.column = i;
	//		newindex.columnLength = table.attributes[i].length;
	//		newindex.attribute_name = table.attributes[i].name;
	//		break;
	//	}
	//}
	//createIndex(newindex);
	FILE* newfile;
	string filename = table.name + ".record";
	newfile=fopen(filename.c_str(),"wb");
	//fwrite("", PAGE_SIZE, 1, newfile);
	fclose(newfile);
}

void CatalogManager::createIndex(Index index)
{
	if (ExistIndex(index.index_name) == true)
	{
		cout << "The Index named " << index.index_name << " has existed." << endl;
		//throw
	}
	else
	{
		indexNum++;
		Indexes.push_back(index);
	}
}


void CatalogManager::dropTable(string table_name)
{
	int flag = 1;
	for (int i = tableNum - 1; i >= 0; i--)
	{
		if (Tables[i].name == table_name)
		{
			Tables.erase(Tables.begin() + i);
			tableNum--;
			flag = 0;
		}
	}
	if (flag == 1)
	{
		cout << "There Isn't A Table Named " << table_name << endl;
		//throw
	}
	else
	{
		for (int i = indexNum - 1; i >= 0; i--)
		{
			if (Indexes[i].table_name == table_name)
			{
				Indexes.erase(Indexes.begin() + i);
				indexNum--;
			}
		}
	}
}

void CatalogManager::dropIndex(string index_name) 
{
	int flag = 1;
	for (int i = indexNum - 1; i >= 0; i--) 
	{
		if (Indexes[i].index_name == index_name) 
		{
			Indexes.erase(Indexes.begin() + i);
			indexNum--;
			flag = 0;
		}
	}
	if (flag == 1)
	{
		cout << "There Isn't An Index Named " << index_name << endl;
		//throw
	}
}

Table CatalogManager::getTable_info(string table_name) 
{
	Table temp;
	for (int i = 0;i < tableNum;i++) 
		if ((Tables[i].name) == table_name) 
			return Tables[i];
	//cout << "No Table Named " << table_name << endl;
	return temp;
}

Index CatalogManager::getIndex_info(string index_name) 
{
	int flag = 0;
	for (int i = 0; i < tableNum; i++) 
	{
		if (Indexes[i].index_name == index_name)
		{
			flag = 1;
			return Indexes[i];
		}
	}
	if (flag==0) 
	{
		//cout << "No Index Named " << index_name << endl;
		Index tmpt;
		return tmpt;		//没找到
	}
}

bool CatalogManager::ExistTable(string table_name) 
{
	int flag = 0;
	for (int i = 0;i < Tables.size();i++) 
		if (Tables[i].name == table_name)
		{
			flag = 1;
			break;
		}
	return flag;
}

bool CatalogManager::ExistIndex(string indexname)
{
	int flag = 0;
	for (int i = 0; i < Indexes.size(); i++) 
	{
		if (Indexes[i].index_name == indexname)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

int CatalogManager::getAttr_no(Table& table, string attr_name)
{
	for (int i = 0;i < table.attributes.size();i++)
		if (table.attributes[i].name == attr_name)
			return i;
	cout << "No Attribute Named " << attr_name << endl;
	return -1;
}

int CatalogManager::getAttr_size(Table& table)
{
	return table.attributes.size();
}



void CatalogManager::changeTable(Table& T) {
	for (int i = 0; i < tableNum; i++)
		if ((Tables[i].name) == T.name)
			Tables[i].blockNum = T.blockNum;	
}

void CatalogManager::clear() {
	Tables.clear();
	Indexes.clear();
	tableNum = 0;
	indexNum = 0;
}

void CatalogManager::reload()
{
	writeTable();
	writeIndex();
	clear();
	readTable();
	readIndex();
}
