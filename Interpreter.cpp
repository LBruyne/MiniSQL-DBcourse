#include"Catalog.h"
#include"global.h"
#include"IndexManager.h"
#include"RecordManager.h"
#include"Interpreter.h"
#include"BufferManager.h"
#include"Page.h"
extern Interpreter i;
extern RecordManager record;
extern IndexManager index;
extern CatalogManager catalog;
extern BufferManager buf;

void Interpreter::Query()
{
	string s;
	getline(cin, s);
	query = s;
	while (query[query.length() - 1] != ';')
	{
		getline(cin, s);
		query += s;
	}
	cout << query << endl;
}

void Interpreter::Choice()
{
	if (query.substr(0, 6) == "select")
		Select();

	else if (query.substr(0, 10) == "drop table")
		Drop_Table();

	else if (query.substr(0, 10) == "drop index")
		Drop_Index();

	else if (query.substr(0, 6) == "insert")
		Insert();

	else if (query.substr(0, 12) == "create table")
		Create_Table();

	else if (query.substr(0, 12) == "create index")
		Create_Index();

	else if (query.substr(0, 11) == "delete from")
		Delete();

	else if (query.substr(0, 4) == "quit")
		Quit();

	else if (query.substr(0, 8) == "execfile")
		Execfile();

	else
	{
		//throw Exception("Invalid Query");
		cout << "Invalid Query" << endl;
	}

}

void Interpreter::Create_Table()
{
	cout << "Create_Table" << endl;

	string table_name,str;
	Table tab;
	Attribute attr;
	vector<string>temp;
	string values, value;
	vector<string> Value;
	vector<int>comma_index;

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
						tab.attributes[k].isPrimeryKey = true;
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


	//for (int k = 0;k < tab.attributes.size();k++)
	//	cout << tab.attributes[k].name <<" "<<tab.attributes[k].isUnique<< " " << tab.attributes[k].isPrimeryKey<<" " << tab.attributes[k].type<< endl;
	//再接一个Catalog接口

	catalog.createTable(tab);
}

void Interpreter::Create_Index()	
{
	//create index 格式: 左右括号分别都要加空格，详见参考文档//例子：create index stunameidx on student ( sname );
	cout << "Create_Index" << endl;

	string index_name, table_name, attribute_name;
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
	index_name = temp[0];
	table_name = temp[2];
	attribute_name = temp[3].substr(1,temp[3].length()-2);

	cout << index_name << table_name << attribute_name << endl;

	//再加一个Catalog接口or其他接口
	Index id;Table tab;
	id.attribute_name = attribute_name;
	id.index_name = index_name;
	id.table_name = table_name;
	//CatalogManager cat;
	tab = catalog.getTable_info(table_name);
	id.column = catalog.getAttr_no(tab, attribute_name);
	id.columnLength = tab.attributes[id.column].length;
	catalog.createIndex(id);
}

void Interpreter::Drop_Table()
{
	cout << "Drop_Table" << endl;

	string table_name;
	int i, j, length;
	i = 11;
	j = Next(i);
	length = j - i;
	if (query[j] != ';')
	{
		cout << "Invalid Query of Drop Table!" << endl;
		//throw Exception("Invalid Query of Drop Table!");
	}
	else
	{
		table_name = query.substr(i, length);
		//再加一个Catalog接口or其他接口
		catalog.dropTable(table_name);
	}
}

void Interpreter::Drop_Index()
{
	cout << "Drop_Index" << endl;

	string index_name;
	int i, j,length;
	i = 11;
	j = Next(i);
	length = j - i;
	if (query[j ] != ';')
	{
		cout << "Invalid Query of Drop Index!" << endl;
		//throw Exception("Invalid Query of Drop Index!");
	}
	else
	{
		index_name = query.substr(i, length);
		//cout<<index_name<<endl;
	}
	//再加一个Catalog接口or其他接口
	catalog.dropIndex(index_name);
}

void Interpreter::Select()
{
	cout << "Select" << endl;

	if (query.substr(0, 13) != "select * from")
	{
		cout << "Invalid Query of Select!";
		//throw exception("Invalid Query of Select!");
	}

	string table_name, str,attribute_name;
	vector<string>temp;
	vector<int>and_index;
	CatalogManager cat;
	vector<Condition>cons;
	Condition con;
	Table tab;

	int i, j, length;
	i = 14;
	j = Next(i);
	length = j - i;
	table_name = query.substr(i, length);
	while(1)
	{
		if (j == query.length() - 1) break;
		i = j + 1;j = Next(i);length = j - i;
		str = query.substr(i, length);
		temp.push_back(str);
	}
	if (temp.size() == 0)
	{
		cout << "The whole table" << endl;
		//整个表展示的接口，table_name信息已经有了
		vector<Condition> cons;
		record.select(tab, cons,'a');
	}
	else
	{
		if (temp[0] != "where")
		{
			cout << "Invalid Query of Select(where)!";
			//throw exception("Invalid Query of Select(where)!");
		}
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
			/// <问题来了，记得判定是or还是and连接！
			/// 此处先假设是and
			/// 记得改回去 
			DATA resultSelect = record.select(tab, cons, 'a');
			for (auto each : resultSelect.ResultSet)
				for (auto each1 : each.DataField)
					cout << each1;
			//再加一个接口
			//条件全部保存在cons里
		}

		//for (int k = 0;k < cons.size();k++)//验证cons内容是否正确
		//	cout << cons[k].columnNum << endl << cons[k].op << endl << cons[k].value << endl;
	}
}

void Interpreter::Insert()
{
	//Insert 格式：insert into student values ('12345678','y',22,'M'); values与括号有空格，括号内容不得有空格
	cout << "Insert" << endl;

	string table_name;
	if (query.substr(0, 11) != "insert into")
	{
		cout << "Invalid Query of Select!1";
		//throw exception("Invalid Query of Select!");
	}

	int i, j, length;
	i = 12;
	j = Next(i);
	length = j - i;
	table_name = query.substr(i, length);

	CatalogManager cat;
	Condition con;
	Table tab;
	string values,value;
	vector<string> Value;
	vector<int>comma_index;
	int comma_num = 0;

	tab = cat.getTable_info(table_name);

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

	const RecordResult& res=record.insert(tab, Value);
	if (!res.status) {
		cout << res.Reason << endl;
	}
	else
	{
		cout << "Successful insertion" << endl;
	}
	//再加一个他接口
	//内容全部保存在Value里面，是一个字符串的容器，所有类型都是以字符串形式存储起来
}

void Interpreter::Delete()
{
	cout << "Delete" << endl;

	int i, j, length;
	string table_name,attribute_name;
	i = 12;
	j = Next(i);
	length = j - i;
	table_name = query.substr(i, length);
	if (query[j] == ';')			//例如：delete from student;
	{
		//删除表的接口
		CatalogManager cat;
		cat.dropTable(table_name);
	}
	else                            //例如：delete from student where sno = '88888888';
	{
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
		Condition con;
		Table tab;
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

		//cout << con.op << con.value << endl;

		//再加一个删除某些值的接口
		//table_name,attribute_name,Condition con都已经储存完毕，con里的value也是以字符串形式存储的
		//例如delete from student where sno = '88888888'; 中 con的value是 88888888 已经把单引号全部去掉了

		//先暂时只给一个condition
		vector <Condition> cons;
		cons.push_back(con);
		record.deleteR(tab, cons, 'a');
	}
}

void Interpreter::Quit()
{
	flag = false;
}

void Interpreter::Execfile()
{
	cout << "Execfile" << endl;

	string file_name = query.substr(9, query.length() - 10);
	ifstream file(file_name);
	if (!file)
	{
		//throw Exception("The File Does Not Exist");
		cout << "The File Does Not Exist" << endl;
	}
	while (getline(file, query))
		Choice();
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
