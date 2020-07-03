#include"Catalog.h"
#include"global.h"
#include"IndexManager.h"
#include"RecordManager.h"
#include"Interpreter.h"
#include"BufferManager.h"
#include"Page.h"

using namespace std;

int BufferManager::		lruCounter[CACHE_CAPACITY];
Page BufferManager::	cachePages[CACHE_CAPACITY];
bool BufferManager::	pined[CACHE_CAPACITY];
bool BufferManager::	isDirty[CACHE_CAPACITY];
//需要在全局范围内重新定义一次

Interpreter i;
RecordManager record;
IndexManager index;
CatalogManager catalog;
BufferManager buf;

int main()
{

	bool execfile = false;
	static auto _ = []()
	{
		ios::sync_with_stdio(false);
		cin.tie(0);
		cout.tie(0);
		return 0;
	}();
	while (i.flag == true)
	{
		cout << "MINI SQL >>>" << ends;
		i.Query();
		if(i.query!="")
		i.Choice();
	}
	system("pause");
	return 0;
}