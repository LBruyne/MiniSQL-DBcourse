/*
 * File name: BufferManager.h
 * Borrow from my roommate qi,Ai
 * Latest revised: 2020.06.16
 * Description: 
 * Realize B+ tree
**/

#ifndef _MINISQL_BPTREE_H_
#define _MINISQL_BPTREE_H_

#include "global.h"
using namespace std;

template <class T>
class Node{
public:
    int MaxChildrens;
    int numElements;
    T *values;
    Pointer *element;
    Node **children;
    Node *parent;
    Node *before;
    Node *next;

public:
    Node() {}; 
    Node(int _MAXChildrens = defaultMaxChildrens);
    void Save(FILE *fp);
    void Load(FILE *fp);
    ~Node();
};


template <class T>
class BPlusTree{
private:
    Node<T> *root;
    string TableName;
    string AttribName;
    int MaxChildrens;

private:
    Node<T>* _FindPosition(const T &data);
    Node<T>* _Find(const T &data, int &index);
    int _Find(const T &data, Node<T> *node);
    int _GetTotal();
    bool _Borrow(Node<T> *node);
    bool _Merge(Node<T> *node);
    void _InsertKey(const T& data, const Pointer& pointer, Node<T> *node, Node<T> *parent);
    void _InsertKey(const T &data, const Pointer &pointer, Node<T> *node);
    void _DeleteKey(Node<T>* node, T &data);

public:
    BPlusTree() {};
    BPlusTree(std::string &FileName);
    BPlusTree(std::string &_TableName, std::string &_AttribName, int _MaxChildrens);
    bool Find(const T &data, Pointer &pointer);
    Pointer FindPointer(const T& data);
    int  FindLess(const T &data, bool CanEqual, vector<Pointer> &pointers);
    int  FindLarger(const T & data, bool CanEqual, vector<Pointer> &pointers);
    int  FindNonEqual(const T &data, vector<Pointer> &pointers);
    int  FindBetween(const T &data1, bool CanEqual1, T &data2, bool CanEqual2, std::vector<Pointer> &pointers);
    int  CalNodeNum(Node<T> *node);
    string GetSaveFileName();
    void Insert(const T &data, const Pointer &pointer);
    void SplitLeafNode(Node<T> *node);
    void SplitNode(Node<T> *node);
    void Delete(T &data);
    void DoSave(Node<T> *node, FILE *fp);
    void Save(const char *filename);
    void Load(const char *filename);
    bool DeletePointers(vector<Pointer> &pointers);
    void Update(T &data_old, T &data_new, Pointer pointer_new);
    int PrintAll();
    int PrintAllreverse();
    int PrintAll(std::ofstream &fout);
    void setAttribute(string&, string&);
};

#endif