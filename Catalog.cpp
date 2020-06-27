#include"Catalog.h"

void CatalogManager::readTable() {
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
			fin >> attr.isPrimeryKey;
			fin >> attr.isUnique;
			tab.attributes.push_back(attr);
			tab.totalLength += attr.length;
		}
		Tables.push_back(tab);
	}
	fin.close();
}

void CatalogManager::readIndex() {
	const string filename = "index.catlog";
	fstream  fin(filename.c_str(), ios::in);
	fin >> indexNum;
	for (int i = 0; i < indexNum; i++)
	{
		Index index;
		fin >> index.index_name;
		fin >> index.table_name;
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
			fout << Tables[i].attributes[j].isPrimeryKey << endl;
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
		for (int i = 0; i < table.attributes.size(); i++) 
			table.totalLength += table.attributes[i].length;
		table.totalLength += 1;//最后一位是有效位
		Tables.push_back(table);
	}
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

void CatalogManager::dropTable(Table table)
{
	dropTable(table.name);
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

void CatalogManager::dropIndex(Index index) 
{
	dropIndex(index.index_name);
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
	cout << "No Table Named " << table_name << endl;
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
		cout << "No Index Named " << index_name << endl;
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

//debug专用
void CatalogManager::ShowTableCatalog() 
{
	cout << "DEBUG:" << endl;
	cout << "Number of Tables:" << tableNum << endl;
	for (int i = 0; i < tableNum; i++)
	{
		cout << "TABLE " << i << endl;
		cout << "Table Name: " << Tables[i].name << endl;
		cout << "Number of attributes: " << Tables[i].attriNum << endl;
		cout << "Number of blocks occupied in disk: " << Tables[i].blockNum << endl;
		for (int j = 0; j < Tables[i].attriNum; j++)
		{
			cout << Tables[i].attributes[j].name << "\t";
			switch (Tables[i].attributes[j].type)
			{
			case CHAR:	cout << "CHAR(" << Tables[i].attributes[j].length << ")\t";	break;
			case INT:	cout << "INT\t";		break;
			case FLOAT:	cout << "FLOAT\t";	break;
			default:	cout << "UNKNOW TYPE\t";	break;
			}
			if (Tables[i].attributes[j].isUnique)	cout << "Unique\t";
			else cout << "NotUnique ";
			if (Tables[i].attributes[j].isPrimeryKey) cout << "PrimeryKey\t" << endl;
			else cout << endl;
		}
	}
}

void CatalogManager::ShowIndexCatalog() 
{
	cout << "DEBUG:" << endl;
	cout << "Number of Indexes:" << indexNum << endl;
	for (int i = 0; i < indexNum; i++)
	{
		cout << "INDEX " << i << endl;
		cout << "Index Name: " << Indexes[i].index_name << endl;
		cout << "Table Name: " << Indexes[i].table_name << endl;
		cout << "Column Number: " << Indexes[i].column << endl;
		cout << "Column Number of blocks occupied in disk: " << Indexes[i].blockNum << endl;
	}
}
