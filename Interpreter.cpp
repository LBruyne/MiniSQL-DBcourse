#include"Catalog.h"
#include"global.h"
#include"IndexManager.h"
#include"RecordManager.h"
#include"Interpreter.h"
#include"BufferManager.h"
#include"Page.h"
#include <time.h>
extern Interpreter i;
extern RecordManager record;
extern IndexManager index;
extern CatalogManager catalog;
extern BufferManager buf;

extern long long  start, endtime;
extern bool infile;
void Interpreter::Query()
{
	string s;
	getline(cin, s);
	query = s;
	while (query.length()>0&&query[query.length() - 1] != ';')
	{
		getline(cin, s);
		query += s;
	}
	//cout << query << endl;

}
inline string Interpreter::getQ() {
	return query;
}

void Interpreter::Choice()
{
	double dura;
	//cout << query << endl;
	if (query.length()>=5&&query.substr(0,5)=="clear") {
		system("cls");
		return;
	}

	if (query.substr(0, 6) == "select")
	{
		if (!infile) {
			start = clock();
			Select();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Select();
	}

	else if (query.substr(0, 10) == "drop table")
		 {
		if (!infile) {
			start = clock();
			Drop_Table();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Drop_Table();
	}

	else if (query.substr(0, 10) == "drop index")
		
	{
		if (!infile) {
			start = clock();
			Drop_Index();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Drop_Index();
	}

	else if (query.substr(0, 6) == "insert")
		{
		if (!infile) {
			start = clock();
			Insert();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Insert();
	}


	else if (query.substr(0, 12) == "create table")
	{
		if (!infile) {

			start = clock();
			Create_Table();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Create_Table();
	}

	else if (query.substr(0, 12) == "create index")
		
	{
		if (!infile) {

			start = clock();
			Create_Index(); 
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Create_Index();
	}
	else if (query.substr(0, 11) == "delete from")
		{
		if (!infile) {

			start = clock();
			Delete();
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else
			Delete();
	}

	else if (query.substr(0, 4) == "quit")
		Quit();

	else if (query.substr(0, 8) == "execfile")
		 {
		if (!infile) {

			start = clock();
			infile = 1;
			Execfile();
			infile = 0;
			endtime = clock();
			dura = (endtime - start) * 1.0 / CLOCKS_PER_SEC;
			cout << "This operation takes " << dura << " seconds" << endl;
		}
		else {
			
			Execfile();
		}

	}

	else
	{
		//throw Exception("Invalid Query");
		cout << "Invalid Query" << endl;
	}

}

void Interpreter::Create_Table()
{

	/*
	int comma_num = 0;
	int i, j, length;

	i = 13;j = Next(i);length = j - i;
	table_name = query.substr(i, length);
	i = j+1;length = query.length() - i-1;
	values = query.substr(i, length);			//读取将括号内的东西汇合成一个字符串
	//cout << values << endl;

	for (int k = 0;k < values.length();k++)		//判断逗号个数，以判断属性数量
	{
		if (values[k] == ',')
		{
			comma_num++;
			comma_index.push_back(k);
		}
	}

	value = values.substr(0, comma_index[0]);	//此时value是第一个属性的信息，例如：（sno char(8)
	Value.push_back(value);						
	for (int k = 1;k < comma_num;k++)
	{
		value = values.substr(comma_index[k - 1] + 1, comma_index[k] - comma_index[k - 1] - 1);
		Value.push_back(value);
	}
	value = values.substr(comma_index[comma_num - 1] + 1, values.length() - comma_index[comma_num - 1] - 1);
	Value.push_back(value);						//此时value是最后一个字段的信息，例如primary key (sno)）

	if (Value[0][0] != '(' || Value[Value.size() - 1][Value[Value.size() - 1].length() - 1] != ')')//判断create 语句的左右括号
	{
		//throw Exception("Invalid Query of Create Table(())");
		cout << "Invalid Query of Create Table(())" << endl;
	}

	Value[0] = Value[0].substr(1, Value[0].length() - 1);
	Value[Value.size() - 1] = Value[Value.size() - 1].substr(0, Value[Value.size() - 1].length() - 1);	//这两句语句在去掉首尾属性字符串中的左右括号

	for (int k = 0;k < Value.size();k++)
	{
		i = 0;j = Next(i,Value[k]);length = j - i;
		str = Value[k].substr(i, length);
		temp.push_back(str);
		//cout << i<< j<< length<<endl;
		while (1)								//将一整段字段分解为几个单词，如：sname char(16) unique分解为sname、char(16)、unique三个单词
		{
			if (j >= Value[k].length()) break;
			i = j + 1;j = Next(i, Value[k]);length = j - i;
			str = Value[k].substr(i, length);
			temp.push_back(str);
			
			//cout << i << j << length << endl;
		}
		
		if (temp[0] != "primary")				//不是primary key信息，即仍是在规定表的属性
		{
			string char_length;
			attr.name = temp[0];
			switch (temp[1][0])
			{
			case 'i':
				attr.type = INT;
				attr.length = INTLEN;
				break;
			case 'f':
				attr.type = FLOAT;
				attr.length = FLOATLEN;
				break;
			case 'c':
				attr.type = CHAR;
				{
					char_length = temp[1].substr(5, temp[1].length() - 6);
					//cout << char_length<<endl;
					attr.length = atoi(char_length.c_str()) ;
				}
				break;
			default:
				cout << "Invalid Query of Create Table(Invalid type)";
				//throw Exception("Invalid Query of Create Table(Invalid type)");
				break;
			}

			if (temp.size() == 3)			//语句中有三个单词，第三个必定是unique，例如：sname char(16) unique
			{
				if (temp[2] == "unique")
					attr.isUnique = true;
				if (temp[2] != "unique")
				{
					cout << "Invalid Query of Create Table(Invalid unique)";
					//throw Exception("Invalid Query of Create Table(Invalid unique)");
				}
			}
			if (temp.size() == 2)			//两个单词就没有unique
				attr.isUnique = false;
			tab.attributes.push_back(attr);
		}
		if (temp[0] == "primary")			//到了primary key信息了
		{
			if (temp[1] != "key")
			{
				cout << "Invalid Query of Create Table(Invalid key)";
				//throw Exception("Invalid Query of Create Table(Invalid key)");
			}
			else
			{
				string a_name = temp[2].substr(1, temp[2].length() - 2);
				for (int k = 0;k < tab.attributes.size();k++)
				{
					if (tab.attributes[k].name == a_name);
					{
						tab.attributes[k].isPrimaryKey = true;
						break;
					}
				}
			}
		}

		//for (int k = 0;k < temp.size();k++)
		//	cout << temp[k] << endl;
		
		temp.clear();	
	}
	tab.name = table_name;
	tab.attriNum = tab.attributes.size();
	
	*/
	//for (int k = 0;k < tab.attributes.size();k++)
	//	cout << tab.attributes[k].name <<" "<<tab.attributes[k].isUnique<< " " << tab.attributes[k].isPrimaryKey<<" " << tab.attributes[k].type<< endl;
	//再接一个Catalog接口

	//cout << "Create_Table" << endl;

	Table tab;
	string keyword;
	size_t index;
	//先获取表格名字
	keyword = query.substr(query.find("table")+5);
	keyword = keyword.substr(keyword.find_first_not_of(" \t"));
	keyword = keyword.substr(0, keyword.find_first_of(" \t"));
	tab.name = keyword;
	query = query.substr(query.find(tab.name) + tab.name.length());
	query = query.substr(query.find_first_not_of(" \t\n("));
	do
	{
		Attribute attr;
		query = query.substr(query.find_first_not_of(" \t"));
		keyword = query.substr(0, query.find_first_of(" \t"));
		attr.name = keyword;
		query = query.substr(keyword.length());
		//获取属性名字
		if (keyword != "primary") {
			query = query.substr(query.find_first_not_of(" \t"));
			keyword = query.substr(0, query.find_first_of(" \t,"));
			attr.type = keyword=="int"? INT: keyword=="float"? FLOAT:CHAR;
			attr.length= keyword == "int" ? INTLENGTH : keyword == "float" ? FLOATLENGTH : 1;
			if (attr.type == CHAR) {
				keyword = keyword.substr(keyword.find("(")+1);
				keyword = keyword.substr(keyword.find_first_not_of(" \t"));
				keyword = keyword.substr(0, keyword.find_first_of(" )\t"));
				attr.length = stoi(keyword);

			}


			//获取类型

			if (index = keyword.find("unique") != string::npos) {
				attr.isUnique = 1;
				attr.isPrimaryKey = 0;
			}
			query = query.substr(query.find(",")+1);
			tab.attributes.push_back(attr);
		}
		//primary key 语句 
		
		else{
			query = query.substr(query.find("(")+1);
			query = query.substr(query.find_first_not_of(" \t"));
			query = query.substr(0, query.find_first_of(" \t;)"));
			for (size_t i = 0; i < tab.attributes.size(); i++)
			{
				if (query == tab.attributes[i].name)
				{
					tab.attributes[i].isPrimaryKey = 1;
				}
				tab.totalLength += tab.attributes[i].length;
			}
			tab.totalLength++;
			tab.attriNum = tab.attributes.size();
			break;
		}
	} while (1);
	
	catalog.createTable(tab);
	cout << "Succeed to create table" << endl;
	catalog.reload();
}

void Interpreter::Create_Index()	
{
	//create index 格式: 左右括号分别都要加空格，详见参考文档//例子：create index stunameidx on student ( sname );
	//cout << "Create_Index" << endl;
	/*
	string temp[4];					//create index 后有四个单词，如：create index stunameidx on student (sname)，四个单词分别是stunameidx、on、student和(sname)
	int i, j, length;
	i = 13;							//index_name的起始位置
	for (int k = 0;k <= 3;k++)
	{
		j = Next(i);
		length = j - i;
		temp[k]= query.substr(i, length);
		i = j + 1;
		//cout <<k<<":"<< temp[k] << endl;
	}
	if (temp[1] != "on" || temp[3][0] != '(' || temp[3][temp[3].length()-1] != ')')
	{
		cout << "Invalid Query of Create Index!" << endl;
		//throw Exception("Invalid Query of Create Index");
	}
	*/
	string index_name, table_name, attribute_name;
	index_name = query.substr(query.find("create index") + 12);
	index_name = index_name.substr(index_name.find_first_not_of(" \t"));
	index_name = index_name.substr(0, index_name.find_first_of(" \t"));
	query = query.substr(query.find(index_name) + index_name.length());
	query = query.substr(query.find("on") + 2);
	query = query.substr(query.find_first_not_of(" \t"));
	table_name = query.substr(0, query.find_first_of(" \t"));
	query = query.substr(table_name.length());
	query = query.substr(query.find_first_not_of(" \t("));
	attribute_name = query.substr(0,query.find_first_of(" \t)"));


	//cout << index_name << table_name << attribute_name << endl;

	//再加一个Catalog接口or其他接口
	Index id;Table tab;
	id.attribute_name = attribute_name;
	id.index_name = index_name;
	id.table_name = table_name;
	//CatalogManager cat;
	catalog.reload();
	tab = catalog.getTable_info(table_name);
	id.column = catalog.getAttr_no(tab, attribute_name);
	if (id.column < 0) {
		cout << "Wrong datafield name! Failed to creat an index." << endl;
		return;
	}
	id.columnLength = tab.attributes[id.column].length;
	if (index.HasIndex(tab.name, id.attribute_name))
	{
		cout << "Attribute " + id.attribute_name + " already has an index." << endl;
		return;
	}
	index.CreateIndex(tab,id);
	catalog.createIndex(id);
	catalog.reload();
	record.createIndex(tab, id);
	
	cout << "Succeed to create index " +id.index_name+" ."<< endl;
}

void Interpreter::Drop_Table()
{
	//cout << "Drop_Table" << endl;

	string table_name;
	table_name = query.substr(10);
	table_name = table_name.substr(table_name.find_first_not_of(" \t"));
	table_name = table_name.substr(0,table_name.find_first_of(" \t;"));
	Table T = catalog.getTable_info(table_name);
	if (T.name=="")
	{
		cout << "Invalid Query of Drop Table!" << endl;
		//throw Exception("Invalid Query of Drop Table!");
	}
	else
	{
		//再加一个Catalog接口or其他接口
		RecordResult res=record.drop(T);
		catalog.dropTable(table_name);
		if (res.status)
			cout << "Succeed to drop the table." << endl;
		else
			cout << res.Reason << endl;
		catalog.reload();
	}
}

void Interpreter::Drop_Index()
{
	//cout << "Drop_Index" << endl;

	string index_name;
	index_name = query.substr(10);
	index_name = index_name.substr(index_name.find_first_not_of(" \t"));
	index_name = index_name.substr(0, index_name.find_first_of(" \t;"));
	Index id = catalog.getIndex_info(index_name);
	Table T = catalog.getTable_info(id.table_name);

	if (id.table_name=="")
	{
		cout << "No such index found." << endl;
		return;
		//throw Exception("Invalid Query of Drop Index!");
	}
	//再加一个Catalog接口or其他接口

	index.DropIndex(T.name,id.attribute_name);
	catalog.dropIndex(index_name);
	cout << "Succeed to drop the index." << endl;
	catalog.reload();
}

void Interpreter::Select()
{
	//cout << "Select" << endl;

	//if (query.substr(0, 13) != "select * from")
	//{
	//	cout << "Invalid Query of Select!";
		//throw exception("Invalid Query of Select!");
	//}
	/*
	string table_name, str, attribute_name;
	vector<string>temp;
	vector<int>and_index;
	CatalogManager cat;
	vector<Condition>cons;


	int i, j, length;
	i = 14;
	j = Next(i);
	length = j - i;
	//table_name = query.substr(i, length);
	while (1)
	{
		if (j == query.length() - 1) break;
		i = j + 1; j = Next(i); length = j - i;
		str = query.substr(i, length);
		temp.push_back(str);
	}
	if (0)//temp.size() == 0)
	{
		cout << "The whole table" << endl;
		table_name = table_name.substr(0, table_name.find_last_not_of(" \t,"));
		//整个表展示的接口，table_name信息已经有了
		vector<Condition> cons;
		Table T = catalog.getTable_info(table_name);
		DATA res = record.select(T, cons, 'a');
		cout << endl;
		for (size_t i = 0; i < res.ResultSet.size(); i++)
		{
			for (size_t j = 0; j < res.ResultSet[i].DataField[j].size(); j++)
			{
				cout << res.ResultSet[i].DataField[j] << "    ";
			}
			cout << endl;
		}
		if (!res.ResultSet.size())
			cout << "No matching records." << endl;
	}
	else
	{
	*/
		//if (temp[0] != "where")
		//{
			//cout << "Invalid Query of Select(where)!";
			//throw exception("Invalid Query of Select(where)!");
		//}
		/*
		for (int k = 0;k < temp.size();k++)			//看看条件语句中有多少个and
			if (temp[k] == "and")
				and_index.push_back(k);

		for (int k = 0;k <= and_index.size();k++)	//循环找到每一条用and分割开的条件语句
		{
			attribute_name = temp[1 + 4 * k];
			int attr_no;
			tab = cat.getTable_info(table_name);
			attr_no = cat.getAttr_no(tab, attribute_name);
			con.columnNum = attr_no;

			if (temp[2 + 4 * k] == "<")
				con.op = Lt;
			else if (temp[2 + 4 * k] == "<=")
				con.op = Le;
			else if (temp[2 + 4 * k] == ">")
				con.op = Gt;
			else if (temp[2 + 4 * k] == ">=")
				con.op = Ge;
			else if (temp[2 + 4 * k] == "=")
				con.op = Eq;
			else if (temp[2 + 4 * k] == "<>")
				con.op = Ne;

			if ((temp[3 + 4 * k].substr(0, 1) == "'"&&temp[3 + 4 * k][temp[3 + 4 * k].length() - 1] != '\'') || (temp[3 + 4 * k].substr(0, 1) != "'"&&temp[3 + 4 * k][temp[3 + 4 * k].length() - 1] == '\''))
			{
				cout << "Invalid Query of Delete!";
				//throw Exception("Invalid Query of Delete!");
			}

			if (temp[3 + 4 * k].substr(0, 1) == "'"&&temp[3 + 4 * k][temp[3 + 4 * k].length() - 1] == '\'')
				con.value = temp[3 + 4 * k].substr(1, temp[3 + 4 * k].length() - 2);
			else
				con.value = temp[3 + 4 * k];

			cons.push_back(con);
		*/

	Table tab; string table_name;
		table_name = query.substr(query.find("from") + 4);
		table_name = table_name.substr(table_name.find_first_not_of(" \t"));
		table_name = table_name.substr(0, table_name.find_last_not_of(" \t,") + 1);
		table_name = table_name.substr(0, table_name.find_first_of(", \t"));
		table_name = table_name.substr(0,table_name.find_last_not_of(" \t;")+1);
		Condition con;
		Table T = catalog.getTable_info(table_name);
		vector <Condition> cons;
		size_t conditionStart = query.find("where");
		size_t subStart = conditionStart;
		string getAttri = query;
		vector<string> showAttri;
		string oper;
		//拿到需要展示的所有属性
		getAttri = getAttri.substr(getAttri.find("select") + 6);
		getAttri = getAttri.substr(0, getAttri.find("from"));

		if (getAttri.find_first_not_of(" \t")==string::npos) {
			cout << "Enter any attribute. Invalid sql syntax." << endl;
		}
		else
		{
			if ((getAttri.find("*") != string::npos)) {
				for (auto iter : T.attributes) {
					showAttri.push_back(iter.name);
				}
			}
			else
			{

			
			do {
				getAttri = getAttri.substr(getAttri.find_first_of(" \t,"));
				getAttri = getAttri.substr(getAttri.find_first_not_of(" \t,"));
				showAttri.push_back(getAttri.substr(0, getAttri.find_first_of(" \t,")));
			} while (getAttri.find(",") != string::npos);
			}
			if (conditionStart != string::npos)
			{
				conditionStart += 5;
				query = query.substr(conditionStart);
				char operation;
				if (query.find("or") != string::npos) {
					operation = 'o';
				}
				else
				{
					operation = 'a';
				}
				oper = operation == 'a' ? "and" : "or";
				string subCon = query = query.substr(query.find_first_not_of(" \t"));//where 后第一个非空格字符
				subCon = query = query.substr(0, query.find_last_of(";") + 1);
				while (query != ";")
				{
					string attri = subCon.substr(0, subCon.find_first_of(" "));
					con.columnNum = catalog.getAttr_no(T, attri);
					if (con.columnNum == -1) {
						cout << "Wroing data field name of \' " << attri << " \' of table " << T.name << endl;
						return;
						//提前结束 出错了  不存在的字段。
					}
					//attri += " ";
					subCon = subCon.substr(subCon.find_first_not_of(attri));// 舍去attribute name
					string op = subCon=subCon.substr(subCon.find_first_not_of(" \t"));
					op = subCon.substr(0, subCon.find_first_of(" \t"));
					if (op == ">=")
						con.op = Ge;
					else if (op == "<=")
						con.op = Le;
					else if (op == ">")
						con.op = Gt;
					else if (op == "<")
						con.op = Lt;
					else if (op == "!=" || op == "<>")
						con.op = Ne;
					else
						con.op = Eq;
					subCon = subCon.substr(subCon.find_first_not_of(op));
					if (T.attributes[con.columnNum].type == CHAR) {
						subCon = subCon.substr(subCon.find_first_of("\'\""));//
						con.value = subCon.substr(1).substr(0, subCon.substr(1).find_first_of("\"\'"));//
						query = subCon = subCon.substr(subCon.find_first_not_of(con.value + "\'\" " + oper));
						cons.push_back(con);
					}
					else {
						subCon = subCon.substr(subCon.find_first_not_of(" \t"));
						con.value = subCon.substr(0, subCon.find_first_of(" \t;"));
						query = subCon = subCon.substr(subCon.find_first_not_of(con.value + " \t" + oper));
						cons.push_back(con);
					}


				}
			}
		}
		DATA res = record.select(T, cons, oper[0]);
		cout << endl;
		vector<int>outCol;
		for (size_t i = 0,j=0; i < T.attributes.size(); i++)
		{
			if (j<showAttri.size()&&(T.attributes[i].name == showAttri[j]))
				outCol.push_back(i),j++;
		}
		for (size_t i = 0; i < res.ResultSet.size(); i++)
		{
			for (auto iter : outCol)
					cout << res.ResultSet[i].DataField[iter] << "    ";
			
			cout << endl;
		}
		if (!res.ResultSet.size())
			cout << "No matching records." << endl;


		/// 
		/// 
		/// 
		//DATA resultSelect = record.select(tab, cons, 'a');
		//for (auto each : resultSelect.ResultSet)
		//	for (auto each1 : each.DataField)
		//		cout << each1;
		//再加一个接口
		//条件全部保存在cons里
	

	//for (int k = 0;k < cons.size();k++)//验证cons内容是否正确
	//	cout << cons[k].columnNum << endl << cons[k].op << endl << cons[k].value << endl;
}

void Interpreter::Insert()
{
	//Insert 格式：insert into student values ('12345678','y',22,'M'); values与括号有空格，括号内容不得有空格
	//cout << "Insert" << endl;

	string table_name;
	table_name = query=query.substr(11);
	table_name = table_name.substr(table_name.find_first_not_of(" \t"));
	table_name = table_name.substr(0, table_name.find_first_of(" \t"));
	query = query.substr(query.find(table_name)+table_name.length());
	Table tab;
	vector<string> value;
	//RecordResult res;
	string keyword;
	tab = catalog.getTable_info(table_name);
	if (tab.name == "") {
		cout << "No such table found!" << endl;
		return;
	}
	query = query.substr(query.find("values")+6);
	query = query.substr(query.find_first_not_of(" \t("));
	for (size_t i = 0; i < tab.attributes.size(); i++)
	{
		switch (tab.attributes[i].type)
		{
		case INT:
			keyword = query.substr(query.find_first_not_of(" \t,"));
			keyword = query.substr(0,query.find_first_of(" \t,);"));

			if (keyword.length() > INTLENGTH) {
				cout << keyword << "is too long for int. Truncation occured." << endl;
				keyword = keyword.substr(0, INTLENGTH);
			}
			value.push_back(keyword);
			query = query.substr(query.find(keyword) + keyword.length());
			break;

		case FLOAT:
			keyword = query.substr(query.find_first_not_of(" \t,"));
			keyword = query.substr(0, query.find_first_of(" \t,);"));
			if (keyword.length() > FLOATLEN) {
				cout << keyword << "is too long for float. Truncation occured." << endl;
				keyword = keyword.substr(0, FLOATLEN);
			}
			value.push_back(keyword);
			query = query.substr(query.find(keyword) + keyword.length());
			break;
		case CHAR:
			keyword = query=query.substr(query.find_first_not_of(" \t,'\""));
			keyword = query.substr(0, query.find_first_of(" \t,'\");"));
			if (keyword.length() > tab.attributes[i].length) {
				cout << keyword << "is too long for "<< tab.name<<"."<<tab.attributes[i].name<<". Truncation occured." << endl;
				keyword = keyword.substr(0, tab.attributes[i].length);
			}
			value.push_back(keyword);
			query = query.substr(query.find(keyword) + keyword.length());
			query = query.substr(query.find_first_not_of(",'\" \t"));
			break;
		}
	}
	/*
	int comma_num = 0;


	i = j + 1;j = Next(i);length = j - i;
	if (query.substr(i, length) != "values")
	{
		cout << "Invalid Query of Insert!2";
		//throw Exception("Invalid Query of Insert!");
	}

	i = j + 1;j = Next(i);length = j - i;
	values = query.substr(i, length);

	if (values[0] != '('&&values[values.length()-1]!=')')
	{
		cout << "Invalid Query of Insert!3";
		//throw Exception("Invalid Query of Insert!");
	}

	for (int k = 0;k < values.length();k++)
	{
		if (values[k] == ',')
		{
			comma_num++;
			comma_index.push_back(k);
		}			
	}
	//cout << comma_num << endl;

	if (comma_num != cat.getAttr_size(tab) - 1)
	{
		cout << "Invalid Query of the Number of the Values!4";
		//throw Exception("Invalid Query of the Number of the Values!");
	}
	else
	{
		value = values.substr(1, comma_index[0]-1);
		if (value[0] == '\'')
			value = value.substr(1, value.length() - 2);
		Value.push_back(value);
		for (int k = 1;k < comma_num;k++)
		{
			value = values.substr(comma_index[k-1]+1, comma_index[k] - comma_index[k-1]-1);
			if (value[0] == '\'')
				value = value.substr(1, value.length() - 2);
			Value.push_back(value);
		}
		value = values.substr(comma_index[comma_num-1]+1, values.length()-comma_index[comma_num-1]-2);
		if (value[0] == '\'')
			value = value.substr(1, value.length() - 2);
		Value.push_back(value);

		//for (int k = 0;k < Value.size();k++)
		//	cout << Value[k] << endl;
	}
	
	//map->string
	*/
	const RecordResult& res=record.insert(tab, value);
	if (!res.status) {
		cout << res.Reason << endl;
	}
	else
	{
		;//cout << "Successful insertion" << endl;
	}
	//再加一个他接口
	//内容全部保存在Value里面，是一个字符串的容器，所有类型都是以字符串形式存储起来
}

void Interpreter::Delete()
{
	//cout << "Delete" << endl;

	int i, j, length;
	string table_name,attribute_name;
	i = 12;
	j = Next(i);
	length = j - i;
	table_name = query.substr(i, length);
	Table T = catalog.getTable_info(table_name);
	if (query[j] == ';')			//例如：delete from student;
	{
		//删除表的接口
		vector<Condition> con;
		RecordResult res = record.deleteR(T, con, 'a');
		if (res.status) {
			cout << "Successful deletion." << endl;
		}
		else
		{
			cout << "Failed to execute the command. " << res.Reason << endl;
		}
	}
	else                            //例如：delete from student where sno = '88888888';
	{
		/*
		string temp[4];
		for (int k = 0;k <= 3;k++)
		{
			i = j + 1;
			j = Next(i);
			length = j - i;
			temp[k] = query.substr(i, length);
			
			//cout <<k<<":"<< temp[k] << endl;
		}

		if (temp[0] != "where")
		{
			cout << "Invalid Query of Delete!" << endl;
			//throw Exception("Invalid Query of Delete!");
		}

		CatalogManager cat;
		attribute_name = temp[1];

		int attr_no;
		
		tab = cat.getTable_info(table_name);
		attr_no = cat.getAttr_no(tab, attribute_name);
		con.columnNum = attr_no;

		if (temp[2] == "<")
			con.op = Lt;
		else if (temp[2] == "<=")
			con.op = Le;
		else if (temp[2] == ">")
			con.op = Gt;
		else if (temp[2] == ">=")
			con.op = Ge;
		else if (temp[2] == "=")
			con.op = Eq;
		else if (temp[2] == "<>"||temp[2]=="!=")
			con.op = Ne;

		if ((temp[3].substr(0, 1) == "'"&&temp[3][temp[3].length() - 1] != '\'') || (temp[3].substr(0, 1) != "'"&&temp[3][temp[3].length() - 1] == '\''))
		{
			cout << "Invalid Query of Delete!";
			//throw Exception("Invalid Query of Delete!");
		}
			
		if (temp[3].substr(0, 1) == "'"&&temp[3][temp[3].length() - 1] == '\'')
			con.value = temp[3].substr(1, temp[3].length() - 2);
		else
			con.value = temp[3];
		*/
		//cout << con.op << con.value << endl;

		//再加一个删除某些值的接口
		//table_name,attribute_name,Condition con都已经储存完毕，con里的value也是以字符串形式存储的
		//例如delete from student where sno = '88888888'; 中 con的value是 88888888 已经把单引号全部去掉了

		Condition con;
		Table tab;
		vector <Condition> cons;
		string table_name(query.substr(query.find("from") + 4));
		table_name = table_name.substr(table_name.find_first_not_of(" \t"));
		table_name = table_name.substr(0,table_name.find_first_of(" \t"));
		tab=catalog.getTable_info(table_name);
		size_t conditionStart = query.find("where")+5;
		size_t subStart = conditionStart ;
		query = query.substr(conditionStart );
		char operation;
		if (query.find("or")!=string::npos) {
			operation = 'o';
		}
		else
		{
			operation = 'a';
		}

		string oper = operation == 'a' ? "and" : "or";
		string subCon = query = query.substr(query.find_first_not_of(" \t"));//where 后第一个非空格字符
		subCon = query = query.substr(0, query.find_last_of(";")+1);
		while (query!=";")
		{
			string attri = subCon.substr(0, subCon.find_first_of(" \t") );
			con.columnNum = catalog.getAttr_no(T, attri);
			if (con.columnNum == -1) {
				//提前结束 出错了  不存在的字段。
			}
			attri += " ";
			subCon = subCon.substr(subCon.find_first_not_of(attri));// 舍去attribute name
			string op = subCon.substr(0, subCon.find_first_of(" \t"));
			if (op==">=")
				con.op = Ge;
			else if (op=="<=")
				con.op = Le;
			else if (op==">")
				con.op = Gt;
			else if (op=="<")
				con.op = Lt;
			else if (op=="!="||op=="<>")
				con.op = Ne;
			else
				con.op = Eq;
			subCon = subCon.substr(subCon.find_first_not_of(op));
			if (T.attributes[con.columnNum].type == CHAR) {
				subCon = subCon.substr(subCon.find_first_of("\'\""));//
				con.value = subCon.substr(1).substr(0, subCon.substr(1).find_first_of("\"\'"));//
				query = subCon = subCon.substr(subCon.find_first_not_of(con.value + "\'\" " + oper));
				cons.push_back(con);
			}
			else {
				subCon = subCon.substr(subCon.find_first_not_of(" \t"));
				con.value = subCon.substr(0, subCon.find_first_of(" \t;"));
				query = subCon = subCon.substr(subCon.find_first_not_of(con.value + " \t" + oper));
				cons.push_back(con);
			}

		}
		RecordResult res=record.deleteR(tab, cons, oper[0]);
		if (res.status) {
			cout << "Successful deletion." << endl;
		}
		else
		{
			cout << "Failed to execute the command. " << res.Reason << endl;
		}
	}
}

void Interpreter::Quit()
{
	flag = false;
	cout << "Bye ~ " << endl;
}

void Interpreter::Execfile()
{


	string file_name = query.substr(9, query.length() - 10);
	ifstream file(file_name);
	if (file.fail())
	{
		//throw Exception("The File Does Not Exist");
		cout << "The File Does Not Exist" << endl;
		return;
	}
	query.clear();
	while (!file.eof())
	{
		do
		{
			string s;
			getline(file, s);
			query += s;

		} while (query.find(";") == string::npos);
		Choice();
		query.clear();
	}

}

int Interpreter::Next(int i)
{
	int j = i + 1;
	while (j <= query.length() && query[j] != ' '&&query[j]!=';')
		j++;
	return j;
}

int Interpreter::Next(int i, string str)
{
	int j = i + 1;
	while (j <= str.length() && str[j] != ' '&&str[j]!='\t')
		j++;
	return j;
}
