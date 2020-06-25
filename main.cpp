#include"Catalog.h"
#include"global.h"
#include"Interpreter.h"

using namespace std;

int main()
{
	Interpreter i;
	while (i.flag == true)
	{
		i.Query();
		i.Choice();
	}
	

	system("pause");
	return 0;
}