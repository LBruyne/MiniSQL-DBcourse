#include"Catalog.h"
#include"global.h"
#include"IndexManager.h"
#include"RecordManager.h"
#include"Interpreter.h"
#include"BufferManager.h"
#include"Page.h"

using namespace std;

int BufferManager::lruCounter[CACHE_CAPACITY];
Page BufferManager::cachePages[CACHE_CAPACITY];
bool BufferManager::pined[CACHE_CAPACITY];
bool BufferManager::isDirty[CACHE_CAPACITY];
//需要在全局范围内重新定义一次

Interpreter i;
RecordManager record;
IndexManager index;
CatalogManager catalog;
BufferManager buf;

int main()
{

	bool execfile = false;
	while (i.flag == true)
	{
		i.Query();
		i.Choice();
	}
	system("pause");
	return 0;
}