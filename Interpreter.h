#pragma once
#include"global.h"


class Interpreter
{
private:
	std::string query;		//��ѯ���
public:
	bool flag = true;			//�ж��Ƿ�quit
	void Query();				//���ڶ�ȡָ��
	void Choice();				//���ݶ����ָ���Ӧ��Ӧ�ӿ�
	void Create_Table();		
	void Create_Index();
	void Drop_Table();
	void Drop_Index();
	void Select();
	void Insert();
	void Delete();
	void Quit();
	void Execfile();

	int Next(int i);			//������һ�����ʵ�ĩβλ��
	int Next(int i, string str);//����str�ַ���������һ�����ʵ�ĩβλ��		//�����������Կո��;��'\t'��Ϊ�жϱ�־��
};

class Exception :std::exception //�쳣�׳�
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