#pragma once
#include"global.h"


class Interpreter
{
private:
	std::string query;		//查询语句
public:
	bool flag = true;			//判断是否quit
	void Query();				//用于读取指令
	void Choice();				//根据读入的指令对应相应接口
	void Create_Table();		
	void Create_Index();
	void Drop_Table();
	void Drop_Index();
	void Select();
	void Insert();
	void Delete();
	void Quit();
	void Execfile();

	int Next(int i);			//返回下一个单词的末尾位置
	int Next(int i, string str);//返回str字符串里面下一个单词的末尾位置		//这两个都是以空格和;或'\t'作为判断标志的
};

class Exception :std::exception //异常抛出
{
private:
	std::string error;
public:
	Exception(std::string s) :error(s) {}
	std::string what() 
	{
		return error;
	}
};