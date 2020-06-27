/*
 * File name: BufferManager.cpp
 * Author: Xuanming, Liu
 * Latest revised: 2020.06.18 
 * Description: 
 * Implementation for IndexManagement and B+ tree.
**/

#include "IndexManager.h"
#include "BPlusTree.h"

template<class T>
void Node<T>::Save(FILE* fp)
{
	fwrite(&numElements, sizeof(int), 1, fp);
	fwrite(values, sizeof(T), MaxChildrens, fp);
	fwrite(element, sizeof(Pointer), MaxChildrens, fp);
	fwrite(&parent, sizeof(Node<T>*), 1, fp);
	fwrite(children, sizeof(Node<T>*), MaxChildrens + 1, fp);
	fwrite(&before, sizeof(Node<T>*), 1, fp);
	fwrite(&next, sizeof(Node<T>*), 1, fp);
}

template<class T>
void Node<T>::Load(FILE* fp)
{
	fread(&numElements, sizeof(int), 1, fp);
	fread(values, sizeof(T), MaxChildrens, fp);
	fread(element, sizeof(Pointer), MaxChildrens, fp);
	fread(&parent, sizeof(Node<T>*), 1, fp);
	fread(children, sizeof(Node<T>*), MaxChildrens + 1, fp);
	fread(&before, sizeof(Node<T>*), 1, fp);
	fread(&next, sizeof(Node<T>*), 1, fp);
}

template<class T>
Node<T>::~Node()
{
	//delete []Values;
	delete[]element;
	delete[]children;
}


template<class T>
Node<T>::Node(int _MaxChildrens) {
	MaxChildrens = _MaxChildrens;
	numElements = 0;
	values = new T[MaxChildrens];
	element = new Pointer[MaxChildrens];
	children = new Node<T> * [MaxChildrens + 1];
	parent = nullptr;
	next = nullptr;
	before = nullptr;
	fill_n(element, MaxChildrens, 0);
	fill_n(children, MaxChildrens + 1, nullptr);
}

template<class T>
int BPlusTree<T>::_GetTotal() {
	int count = 0;
	Node<T>* node = root;
	while (node != nullptr && !node->children[0])
		node = node->children[0];
	while (node != nullptr) {
		count += node->numElements;
		node = node->next;
	}
	cout << "total: " << count << endl;
	return count;
}

template<class T>
void BPlusTree<T>::_InsertKey(const T& data, const Pointer& pointer, Node<T>* node, Node<T>* parent) {
	for (int i = 0; i < parent->numElements; i++) {
		if (parent->values[i] > data) {
			for (int j = parent->numElements; j > i; j--) {
				parent->values[j] = parent->values[j - 1];
				parent->element[j] = parent->element[j - 1];
				parent->children[j + 1] = parent->children[j];
			}
			parent->values[i] = data;
			parent->element[i] = pointer;
			parent->children[i + 1] = node;
			parent->numElements++;
			return;
		}
	}
	parent->values[parent->numElements] = data;
	parent->element[parent->numElements++] = pointer;
	parent->children[parent->numElements] = node;
	return;
}

template<class T>
void BPlusTree<T>::_InsertKey(const T& data, const Pointer& pointer, Node<T>* node) {
	int i;
	for (i = 0; i < node->numElements; i++) {
		if (node->values[i] > data) {
			for (int j = node->numElements; j > i; j--) {
				node->values[j] = node->values[j - 1];
				node->element[j] = node->element[j - 1];
			}
			node->values[i] = data;
			node->element[i] = pointer;
			node->numElements++;
			return;
		}
	}
	node->numElements++;
	node->element[i] = pointer;
	node->values[i] = data;
	return;
}

template<class T>
void BPlusTree<T>::SplitLeafNode(Node<T>* node) {
	Node<T>* parent = node->parent;
	if (!parent) {
		parent = new Node<T>(MaxChildrens);
		parent->children[0] = node;
		node->parent = parent;
		this->root = parent;
	}
	Node<T>* newNode = new Node<T>(MaxChildrens);
	int half = MaxChildrens / 2;
	newNode->numElements = MaxChildrens - half;
	memcpy(newNode->values, node->values + half, sizeof(T) * (MaxChildrens - half));
	memcpy(newNode->element, node->element + half, sizeof(Pointer) * (MaxChildrens - half));

	newNode->parent = parent;
	newNode->before = node;
	newNode->next = node->next;
	node->next = newNode;
	if (newNode->next)
		newNode->next->before = newNode;
	node->numElements = half;
	_InsertKey(node->values[half], node->element[half], newNode, parent);
	if (parent->numElements == MaxChildrens)
		SplitNode(parent);
	return;
}

template<class T>
void BPlusTree<T>::SplitNode(Node<T>* node) {
	Node<T>* parent = node->parent;
	if (!parent) {
		parent = new Node<T>(MaxChildrens);
		parent->children[0] = node;
		node->parent = parent;
		this->root = parent;
	}
	Node<T>* newNode = new Node<T>(MaxChildrens);
	int half = (MaxChildrens - 1) / 2;
	newNode->numElements = MaxChildrens - 1 - half;
	memcpy(newNode->values, node->values + half + 1, sizeof(T) * (MaxChildrens - 1 - half));
	memcpy(newNode->element, node->element + half + 1, sizeof(Pointer) * (MaxChildrens - 1 - half));
	memcpy(newNode->children, node->children + half + 1, sizeof(Node<T>*) * (MaxChildrens - half));

	for (int i = 0; i <= newNode->numElements; i++)
		newNode->children[i]->parent = newNode;

	newNode->parent = parent;
	newNode->before = node;
	newNode->next = node->next;
	node->next = newNode;
	if (newNode->next)
		newNode->next->before = newNode;
	node->numElements = half;
	_InsertKey(node->values[half], node->element[half], newNode, parent);
	if (parent->numElements == MaxChildrens)
		SplitNode(parent);
	return;
}

template<class T>
Node<T>* BPlusTree<T>::_FindPosition(const T& data) {
	Node<T>* node = this->root;
	while (node) {
		if (node->children[0] == nullptr)
			return node;
		else {
			for (int i = 0; i <= node->numElements; i++) {
				if (i == node->numElements || node->values[i] > data) {
					node = node->children[i];
					break;
				}
			}
		}
	}
}

template<class T>
Node<T>* BPlusTree<T>::_Find(const T& data, int& index) {
	Node<T>* node = this->root;
	while (node) {
		if (node->children[0] == nullptr) {
			if (_Find(data, node) != -1)
				return node;
			else
				return nullptr;
		}
		for (int i = 0; i <= node->numElements; i++) {
			if (i == node->numElements || node->values[i] > data) {
				node = node->children[i];
			}
		}
	}
	return nullptr;
}

template<class T>
int BPlusTree<T>::_Find(const T& data, Node<T>* node) {
	for (int i = 0; i < node->numElements; i++) {
		if (node->values[i] == data)
			return i;
	}
	return -1;
}

template<class T>
void BPlusTree<T>::Insert(const T& data, const Pointer& pointer) {
	Node<T>* node;
	if (!this->root) {
		this->root = new Node<T>(this->MaxChildrens);
		node = this->root;
	}
	else node = _FindPosition(data);
	_InsertKey(data, pointer, node);
	if (node->numElements == this->MaxChildrens)
		SplitLeafNode(node);
	return;
}

template<class T>
bool BPlusTree<T>::_Borrow(Node<T>* node) {
	Node<T>* parent = node->parent;
	Node<T>* leftSibling = node->before;
	Node<T>* rightSibling = node->next;
	if (leftSibling && ((!leftSibling->children[0] && leftSibling->numElements > (MaxChildrens / 2)) ||
		(leftSibling->children[0] && leftSibling->numElements > (MaxChildrens - 1) / 2))) {
		memcpy(node->values + 1, node->values, sizeof(T) * (MaxChildrens - 1));
		memcpy(node->element + 1, node->element, sizeof(Pointer) * (MaxChildrens - 1));
		memcpy(node->children + 1, node->children, sizeof(Node<T>*) * (MaxChildrens));
		if (parent) {
			int i = _Find(node->values[0], parent);
			parent->values[i] = leftSibling->values[leftSibling->numElements - 1];
			parent->element[i] = leftSibling->element[leftSibling->numElements - 1];
		}
		node->values[0] = leftSibling->values[leftSibling->numElements - 1];
		node->element[0] = leftSibling->element[leftSibling->numElements - 1];
		node->children[0] = leftSibling->children[leftSibling->numElements];
		if (leftSibling->children[leftSibling->numElements])
			leftSibling->children[leftSibling->numElements]->parent = node;
		node->numElements++;
		leftSibling->numElements++;
		return true;
	}
	else if (rightSibling && ((!rightSibling->children[0] && rightSibling->numElements > (MaxChildrens / 2)) ||
		(rightSibling->children[0] && rightSibling->numElements > (MaxChildrens - 1) / 2))) {
		if (parent) {
			int i = _Find(rightSibling->values[0], parent);
			parent->values[i] = rightSibling->values[1];
			parent->element[i] = rightSibling->element[1];
		}
		if (rightSibling->children[0])
			rightSibling->children[0]->parent = node;
		node->values[node->numElements] = rightSibling->values[0];
		node->element[node->numElements] = rightSibling->element[0];
		node->children[node->numElements] = rightSibling->children[0];
		memcpy(rightSibling->values, rightSibling->values + 1, sizeof(T) * (MaxChildrens - 1));
		memcpy(rightSibling->element, rightSibling->element + 1, sizeof(Pointer) * (MaxChildrens - 1));
		memcpy(rightSibling->children, rightSibling->children + 1, sizeof(Node<T>*) * (MaxChildrens));
		node->numElements++;
		rightSibling->numElements++;
		return true;
	}
	else
		return false;
}

template<class T>
bool BPlusTree<T>::_Merge(Node<T>* node) {
	Node<T>* parent = node->parent;
	Node<T>* leftSibling = node->before;
	Node<T>* rightSibling = node->next;
	if (leftSibling && ((!leftSibling->children[0] && leftSibling->numElements + node->numElements < MaxChildrens - 1) ||
		(leftSibling->children[0] && leftSibling->numElements + node->numElements < MaxChildrens - 2))) {
		memcpy(leftSibling->values + leftSibling->numElements, node->values, sizeof(T) * (node->numElements));
		memcpy(leftSibling->element + leftSibling->numElements, node->element, sizeof(Pointer) * (node->numElements));
		memcpy(leftSibling->children + leftSibling->numElements, node->children, sizeof(Node<T>*) * (node->numElements));
		if (parent) {
			int i = _Find(node->values[0], parent);
			memcpy(parent->values + i, parent->values + i + 1, sizeof(T) * (MaxChildrens - i - 1));
			memcpy(parent->element + i, parent->element + i + 1, sizeof(Pointer) * (MaxChildrens - i - 1));
			memcpy(parent->children + i, parent->children + i + 1, sizeof(Node<T>*) * (MaxChildrens - i));
		}
		if (node->children[0])
			for (int i = 0; i < node->numElements; i++) {
				node->children[i]->parent = leftSibling;
			}
		leftSibling->numElements += node->numElements;
		delete node;
		return true;
	}
	else if (rightSibling && ((!rightSibling->children[0] && rightSibling->numElements + node->numElements < MaxChildrens - 1) ||
		(rightSibling->children[0] && rightSibling->numElements > +node->numElements < MaxChildrens - 2))) {
		memcpy(node->values + node->numElements, rightSibling->values, sizeof(T) * (rightSibling->numElements));
		memcpy(node->element + node->numElements, rightSibling->element, sizeof(Pointer) * (rightSibling->numElements));
		memcpy(node->children + node->numElements, rightSibling->children, sizeof(Node<T>*) * (rightSibling->numElements));
		if (parent) {
			int i = _Find(rightSibling->values[0], parent);
			memcpy(parent->values + i, parent->values + i + 1, sizeof(T) * (MaxChildrens - i - 1));
			memcpy(parent->element + i, parent->element + i + 1, sizeof(Pointer) * (MaxChildrens - i - 1));
			memcpy(parent->children + i, parent->children + i + 1, sizeof(Node<T>*) * (MaxChildrens - i));
		}
		if (rightSibling->children[0])
			for (int i = 0; i < rightSibling->numElements; i++) {
				rightSibling->children[i]->parent = leftSibling;
			}
		node->numElements += rightSibling->numElements;
		delete rightSibling;
		return true;
	}
	else
		return false;

}

template<class T>
void BPlusTree<T>::_DeleteKey(Node<T>* node, T& data) {
	int index = _Find(data, node);
	int indexP = -1;
	Node<T>* parent = node->parent;
	if (parent)
		indexP = _Find(data, parent);
	if (index == -1 && indexP == -1)
		return;
	if (index == -1 && indexP != -1) {
		_DeleteKey(node->parent, data);
		return;
	}
	memcpy(node->values + index, node->values + index + 1, sizeof(T) * (MaxChildrens - 1 - index));
	memcpy(node->element + index, node->element + index + 1, sizeof(Pointer) * (MaxChildrens - 1 - index));
	memcpy(node->children + index + 1, node->children + index + 2, sizeof(Node<T>*) * (MaxChildrens - 1 - index));
	node->numElements--;
	if (node == this->root && node->numElements == 0) {
		this->root = node->children[0];
		if (this->root != NULL) this->root->parent = NULL;
		delete node;
		return;
	}
	if (node != root && ((node->children[0] && node->numElements < (MaxChildrens / 2)) ||
		(!node->children[0] && node->numElements < (MaxChildrens - 1) / 2))) {
		if (!_Borrow(node))
			_Merge(node);
	}
	if (index == 0 && parent)
		_DeleteKey(parent, data);
	return;
}

template<class T>
void BPlusTree<T>::Delete(T& data) {
	int i;
	if (root == NULL)
		return;
	Node<T>* node = _FindPosition(data);
	if (!node)
		return;
	_DeleteKey(node, data);
}

template<class T>
bool BPlusTree<T>::Find(const T& data, Pointer& pointer) {
	Node<T>* node = this->root;
	while (node) {
		if (node->children[0] == nullptr) {
			for (int i = 0; i < node->numElements; i++) {
				if (node->values[i] == data) {
					pointer = node->element[i];
					return true;
				}
			}
			pointer = 0;
			return false;
		}
		if (node->values[node->numElements - 1] <= data) {
			node = node->children[node->numElements];
			continue;
		}
		for (int i = 0; i < node->numElements; i++) {
			if (node->values[i] > data) {
				node = node->children[i];
				break;
			}
		}
	}
	pointer = 0;
	return false;
}


template<class T>
Pointer BPlusTree<T>::FindPointer(const T& data) {
	Node<T>* node = this->root;
	int i; 
	while (node) {
		for (i = 0; i < node->numElements; i++) {
			if (node->values[i] > data) {
				if (node->children[0] == nullptr) {
					if (i > 1)
						return node->element[i - 1];
					else if (node->before)
						return node->before->element[node->before->numElements - 1];
					else
						return node->element[i];
				}
				else {
					node = node->children[i];
				}
			}
		}
		if (node->children[i])
			node = node->children[i];
		else
			break;
	}
	return 0;
}

template<class T>
int BPlusTree<T>::FindLess(const T& data, bool CanEqual, vector<Pointer>& pointers) {
	Node<T>* node = root;
	while (node != nullptr && node->children[0])
	{
		int i;
		for (i = 0; i < node->numElements; i++)
			if (node->values[i] >= data)
				break;
		if (i == MaxChildrens - 1)
			node = node->children[MaxChildrens - 1];
		else
			if (data == node->values[i])
				node = node->children[i + 1];
			else
				node = node->children[i];
	}
	pointers.clear();
	int count = 0;
	if (node != nullptr) {
		int k;
		for (k = node->numElements - 1; k >= 0; k--) {
			if (CanEqual) {
				if (node->values[k] <= data)
					break;
			}
			else {
				if (node->values[k] < data)
					break;
			}
		}
		for (int i = k; i >= 0; i--) {
			pointers.push_back(node->element[i]);
			count++;
		}
		node = node->before;
		bool out = false;
		while (node != nullptr && !out) {
			for (int k = node->numElements - 1; k >= 0; k--) {
				pointers.push_back(node->element[k]);
				count++;
			}
			node = node->before;
		}
		std::reverse(pointers.begin(), pointers.end());
	}
	return count;
}

template<class T>
int BPlusTree<T>::FindLarger(const T& data, bool CanEqual, vector<Pointer>& pointers) {
	Node<T>* node = root;
	while (node != nullptr && node->children[0])
	{
		int i;
		for (i = 0; i < node->numElements; i++)
			if (node->values[i] >= data)
				break;
		if (i == MaxChildrens - 1)
			node = node->children[MaxChildrens - 1];
		else {
			if (data == node->values[i])
				node = node->children[i + 1];
			else
				node = node->children[i];
		}
	}
	pointers.clear();
	int count = 0;
	if (node != nullptr) {
		int i;
		for (i = 0; i < node->numElements; i++)
			if (CanEqual) {
				if (node->values[i] >= data)
					break;
			}
			else {
				if (node->values[i] > data)
					break;
			}
		for (int k = i; k < node->numElements; k++) {
			pointers.push_back(node->element[k]);
			count++;
		}
		node = node->next;
		bool out = false;
		while (node != nullptr && !out) {
			for (int k = 0; k < node->numElements; k++) {
				pointers.push_back(node->element[k]);
				count++;
			}
			node = node->next;
		}
	}
	return count;
}


template<class T>
int BPlusTree<T>::FindNonEqual(const T& data, std::vector<Pointer>& pointers) {
	int count = 0;
	pointers.clear();
	Node<T>* node = root;
	while (node != nullptr && node->children[0])
		node = node->children[0];
	bool out = false;
	while (node != nullptr && !out) {
		for (int i = 0; i < node->numElements; i++)
		{
			if (node->values[i] == data)
				continue;
			pointers.push_back(node->element[i]);
			count++;
		}
		node = node->next;
	}
	return count;
}

template<class T>
int BPlusTree<T>::FindBetween(const T& data1, bool CanEqual1, T& data2, bool CanEqual2, std::vector<Pointer>& pointers) {
	Node<T>* node = root;
	while (node != nullptr && node->children[0]) {
		int i;
		for (i = 0; i < node->numElements; i++)
			if (node->values[i] >= data1)
				break;
		if (i == MaxChildrens - 1)
			node = node->children[MaxChildrens - 1];
		else {
			if (data1 == node->values[i])
				node = node->children[i + 1];
			else
				node = node->children[i];
		}
	}
	pointers.clear();
	int count = 0;
	if (node != nullptr) {
		int i;
		for (i = 0; i < node->numElements; i++)
			if (CanEqual1) {
				if (node->values[i] >= data1)
					break;
			}
			else {
				if (node->values[i] > data1)
					break;
			}
		for (int k = i; k < node->numElements; k++) {
			if (CanEqual2) {
				if (node->values[k] > data2)
					break;
			}
			else {
				if (node->values[k] >= data2)
					break;
			}
			pointers.push_back(node->element[k]);
			count++;
		}
		node = node->next;
		bool out = false;
		while (node != nullptr && !out) {
			for (int k = 0; k < node->numElements; k++) {
				if (CanEqual2) {
					if (node->values[k] > data2) {
						out = true;
						break;
					}
				}
				else {
					if (node->values[k] >= data2) {
						out = true;
						break;
					}
				}
				pointers.push_back(node->element[k]);
				count++;
			}
			node = node->next;
		}
	}
	return count;
}

template<class T>
string BPlusTree<T>::GetSaveFileName() {
	return TableName + std::string("_") + AttribName + std::string(".index");
}

template<class T>
void BPlusTree<T>::Save(const char* filename) {
	FILE* fp = fopen(filename, "wb");
	fwrite(&AttribName, sizeof(std::string), 0, fp);
	fwrite(&MaxChildrens, sizeof(int), 1, fp);
	int tot = CalNodeNum(root);
	fwrite(&tot, sizeof(int), 1, fp);
	DoSave(root, fp);
	fclose(fp);
}

template<class T>
int BPlusTree<T>::CalNodeNum(Node<T>* node) {
	if (node == nullptr) return 0;
	int ans = 1;
	for (int i = 0; i <= node->numElements; i++)
		ans += CalNodeNum(node->children[i]);
	return ans;
}

template<class T>
void BPlusTree<T>::DoSave(Node<T>* node, FILE* fp) {
	if (node == nullptr) return;
	fwrite(&node, sizeof(Node<T>*), 1, fp);
	node->Save(fp);
	for (int i = 0; i <= node->numElements; i++)
		DoSave(node->children[i], fp);
}

template<class T>
void BPlusTree<T>::Load(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	fread(&AttribName, sizeof(std::string), 0, fp);
	fread(&MaxChildrens, sizeof(int), 1, fp);
	int tot;
	fread(&tot, sizeof(int), 1, fp);
	if (tot == 0) {
		root = nullptr;
		return;
	}
	Node<T>** tmp;
	Node<T>** old;
	tmp = new Node<T> * [tot];
	old = new Node<T> * [tot];
	for (int i = 0; i < tot; i++) {
		tmp[i] = new Node<T>(MaxChildrens);
		fread(&old[i], sizeof(Node<T>*), 1, fp);
		tmp[i]->Load(fp);
	}
	for (int i = 0; i < tot; i++) {
		for (int k = 0; k < tot; k++)
			if (tmp[i]->parent == old[k]) {
				tmp[i]->parent = tmp[k];
				break;
			}
		for (int j = 0; j <= tmp[i]->numElements; j++)
			for (int k = 0; k < tot; k++)
				if (tmp[i]->children[j] == old[k]) {
					tmp[i]->children[j] = tmp[k];
					break;
				}
		for (int k = 0; k < tot; k++)
			if (tmp[i]->before == old[k]) {
				tmp[i]->before = tmp[k];
				break;
			}
		for (int k = 0; k < tot; k++)
			if (tmp[i]->next == old[k]) {
				tmp[i]->next = tmp[k];
				break;
			}
	}
	root = tmp[0];
	delete[]tmp;
	delete[]old;
}

template<class T>
bool BPlusTree<T>::DeletePointers(vector<Pointer>& pointers) {
	int num = pointers.size();
	for (int i = 0; i < num; i++) {
		Node<T>* node = root;
		while (node != nullptr && node->children[0])
			node = node->children[0];
		bool found = false;
		int k;
		while (node != nullptr && !found) {
			for (k = 0; k < node->numElements; k++) {
				if (node->element[k] == pointers[i]) {
					found = true;
					break;
				}
			}
			if (found)
				break;
			node = node->next;
		}
		if (found)
			_DeleteKey(node, node->values[k]);
	}
	return true;
}

template<class T>
void BPlusTree<T>::Update(T& data_old, T& data_new, Pointer pointer_new) {
	Delete(data_old);
	Insert(data_new, pointer_new);
}

template<class T>
int BPlusTree<T>::PrintAll() {
	int count = 0;
	Node<T>* node = root;
	while (node != NULL && node->children[0])
		node = node->children[0];
	while (node != NULL) {
		for (int i = 0; i < node->numElements; i++) {
			count++;
			std::cout << node->values[i] << " ";
		}
		node = node->next;
	}
	std::cout << std::endl;
	std::cout << "count=" << count << std::endl;
	return count;
}

template<class T>
int BPlusTree<T>::PrintAllreverse() {
	int count = 0;
	Node<T>* node = root;
	while (node != NULL && node->children[0])
		node = node->children[node->numElements];
	while (node != NULL) {
		for (int i = 0; i < node->numElements; i++) {
			count++;
			std::cout << node->values[i] << " ";
		}
		node = node->before;
	}
	std::cout << std::endl;
	std::cout << "count=" << count << std::endl;
	return count;
}

template<class T>
int BPlusTree<T>::PrintAll(std::ofstream& fout) {
	int count = 0;
	Node<T>* node = root;
	while (node != NULL && node->children[0])
		node = node->children[0];
	while (node != NULL) {
		for (int i = 0; i < node->numElements; i++) {
			count++;
			fout << node->values[i] << " ";
		}
		node = node->next;
	}
	fout << endl;
	fout << "count=" << count << endl;
	return count;
}

template<class T>
BPlusTree<T>::BPlusTree(std::string& FileName) {
	Load(FileName.c_str());
}

template<class T>
BPlusTree<T>::BPlusTree(std::string& _TableName, std::string& _AttribName, int _MaxChildrens) {
	TableName = _TableName;
	AttribName = _AttribName;
	MaxChildrens = _MaxChildrens;
	root = nullptr;
}

//////////////////////////////////////////////////////
// INDEX MANAGER 
//////////////////////////////////////////////////////

IndexManager::IndexManager()
{
	bpt_INT = NULL;
	bpt_FLOAT = NULL;
	bpt_STRING = NULL;
}

bool IndexManager::CreateIndex(Table& table, Index& index)
{
	std::string FileName = index.table_name + std::string("_") + table.attributes[index.column].name + std::string(".index");
	int type = table.attributes[index.column].type;
	switch (type)
	{
		case INT:
		{
			ResetBptInt(true);
			bpt_INT = new BPlusTree<int>(table.name, table.attributes[index.column].name, 20);
			bpt_INT->Save(FileName.c_str());
			bptIntName = FileName;
			return true;
		}
		case FLOAT:
		{
			ResetBptFloat(true);
			bpt_FLOAT = new BPlusTree<float>(table.name, table.attributes[index.column].name, 20);
			bpt_FLOAT->Save(FileName.c_str());
			bptFloatName = FileName;
			return true;
		}
		case CHAR:
		{
			ResetBptString(true);
			bpt_STRING = new BPlusTree<std::string>(table.name, table.attributes[index.column].name, 20);
			bpt_STRING->Save(FileName.c_str());
			bptStringName = FileName;
			return true;
		}
	}
	return false;
}
bool IndexManager::CreateIndex(Table& table)
{
	int i;
	for (i = 0; i < table.attriNum; i++)
		if (table.attributes[i].isPrimeryKey == true)
			break;
	if (i == table.attriNum) return false;
	std::string FileName = table.name + std::string("_") + table.attributes[i].name + std::string(".index");

	if (table.attributes[i].type == INT)
	{
		ResetBptInt(true);
		bpt_INT = new BPlusTree<int>(table.name, table.attributes[i].name, 20);
		bpt_INT->Save(FileName.c_str());
		bptIntName = FileName;
		return true;
	}
	else if (table.attributes[i].type == FLOAT)
	{
		ResetBptFloat(true);
		bpt_FLOAT = new BPlusTree<float>(table.name, table.attributes[i].name, 20);
		bpt_FLOAT->Save(FileName.c_str());
		bptFloatName = FileName;
		return true;
	}
	else if (table.attributes[i].type == CHAR)
	{
		ResetBptString(true);
		bpt_STRING = new BPlusTree<std::string>(table.name, table.attributes[i].name, 20);
		bpt_STRING->Save(FileName.c_str());
		bptStringName = FileName;
		return true;
	}
	return false;
}
bool IndexManager::HasIndex(std::string& TableName, std::string& AttribName)
{
	std::string FileName = TableName + std::string("_") + AttribName + std::string(".index");
	FILE* fp = fopen(FileName.c_str(), "rb");
	if (fp == NULL)
		return false;
	else {
		fclose(fp);
		return true;
	}
}

bool IndexManager::DropIndex(std::string& TableName, std::string& AttribName)
{
	std::string FileName = TableName + std::string("_") + AttribName + std::string(".index");
	FILE* fp = fopen(FileName.c_str(), "rb");
	if (fp == NULL)
		return false;
	else fclose(fp);
	if (remove(FileName.c_str()) != 0)
		perror("Error deleting file");
	if (bptIntName == FileName)
		ResetBptInt(false);
	if (bptFloatName == FileName)
		ResetBptFloat(false);
	if (bptStringName == FileName)
		ResetBptString(false);
	return true;
}

void IndexManager::ResetBptInt(bool save)
{
	if (bpt_INT != NULL)
	{
		if (save) bpt_INT->Save(bpt_INT->GetSaveFileName().c_str());
		delete bpt_INT;
		bpt_INT = NULL;
	}
}

void IndexManager::ResetBptFloat(bool save)
{
	if (bpt_FLOAT != NULL)
	{
		if (save) bpt_FLOAT->Save(bpt_FLOAT->GetSaveFileName().c_str());
		delete bpt_FLOAT;
		bpt_FLOAT = NULL;
	}
}

void IndexManager::ResetBptString(bool save)
{
	if (bpt_STRING != NULL)
	{
		if (save) bpt_STRING->Save(bpt_STRING->GetSaveFileName().c_str());
		delete bpt_STRING;
		bpt_STRING = NULL;
	}
}

void IndexManager::SaveIndex() {
	if (bpt_INT)
		ResetBptInt(true);
	else if (bpt_FLOAT)
		ResetBptFloat(true);
	else if (bpt_STRING)
		ResetBptString(true);
}

bool IndexManager::LoadIndex(std::string& TableName, std::string& AttribName, int type)
{
	std::string FileName = TableName + std::string("_") + AttribName + std::string(".index");
	if (FileName == bptIntName || FileName == bptFloatName || FileName == bptStringName)
		return true;
	FILE* fp = fopen(FileName.c_str(), "rb");
	if (fp == NULL)
		return false;
	else fclose(fp);

	switch (type)
	{
	case INT:
		ResetBptInt(true);
		bpt_INT = new BPlusTree<int>(FileName);
		bptIntName = FileName;
		return true;
		break;
	case FLOAT:
		ResetBptFloat(true);
		bpt_FLOAT = new BPlusTree<float>(FileName);
		bptFloatName = FileName;
		return true;
		break;
	case CHAR:
		ResetBptString(true);
		bpt_STRING = new BPlusTree<std::string>(FileName);
		bptStringName = FileName;
		return true;
		break;
	}
	return false;
}

bool IndexManager::InsertItem(std::string& value, Pointer pointer, int type)
{
	switch (type)
	{
	case INT: {
		if (bpt_INT == NULL) return false;
		int data = atoi(value.c_str());
		bpt_INT->Insert(data, pointer);
		return true;
		break;
	}
	case FLOAT: {
		if (bpt_FLOAT == NULL) return false;
		float data = atof(value.c_str());
		bpt_FLOAT->Insert(data, pointer);
		return true;
		break;
	}
	case CHAR: {
		if (bpt_STRING == NULL) return false;
		bpt_STRING->Insert(value, pointer);
		return true;
		break;
	}
	}
	return false;
}

bool IndexManager::DeleteItem(std::string& value, int type)
{
	switch (type)
	{
	case INT: {
		if (bpt_INT == NULL) return false;
		int data = atoi(value.c_str());
		bpt_INT->Delete(data);
		return true;
		break;
	}
	case FLOAT: {
		if (bpt_FLOAT == NULL) return false;
		float data = atof(value.c_str());
		bpt_FLOAT->Delete(data);
		return true;
		break;
	}
	case CHAR: {
		if (bpt_STRING == NULL) return false;
		bpt_STRING->Delete(value);
		return true;
		break;
	}
	}
	return false;
}

bool IndexManager::DeletePointers(vector<Pointer>& pointers, int type)
{
	switch (type)
	{
	case INT:
		return bpt_INT->DeletePointers(pointers);
	case FLOAT:
		return bpt_FLOAT->DeletePointers(pointers);
	case CHAR:
		return bpt_STRING->DeletePointers(pointers);
	}
	return false;
}


// TODO: Interpreter 这里啥意思
bool IndexManager::SelectItem(Condition& cond, int type, std::vector<Pointer>& pointers)
{
	pointers.clear();
	switch (type)
	{
	case INT:
	{
		int data = atoi(cond.value.c_str());
		switch (cond.op)
		{
		case Lt: {
			bpt_INT->FindLess(data, false, pointers);
			return true;
			break;
		}
		case Le: {
			bpt_INT->FindLess(data, true, pointers);
			return true;
			break;
		}
		case Gt: {
			bpt_INT->FindLarger(data, false, pointers);
			return true;
			break;
		}
		case Ge: {
			bpt_INT->FindLarger(data, true, pointers);
			return true;
			break;
		}
		case Eq: {
			Pointer pointer;
			if (bpt_INT->Find(data, pointer))
				pointers.push_back(pointer);
			return true;
			break;
		}
		case Ne: {
			bpt_INT->FindNonEqual(data, pointers);
			return true;
			break;
		}

		}
		break;
	}
	case FLOAT:
	{
		float data = atof(cond.value.c_str());
		switch (cond.op)
		{
		case Lt: {
			bpt_FLOAT->FindLess(data, false, pointers);
			return true;
			break;
		}
		case Le: {
			bpt_FLOAT->FindLess(data, true, pointers);
			return true;
			break;
		}
		case Gt: {
			bpt_FLOAT->FindLarger(data, false, pointers);
			return true;
			break;
		}
		case Ge: {
			bpt_FLOAT->FindLarger(data, true, pointers);
			return true;
			break;
		}
		case Eq: {
			Pointer pointer;
			if (bpt_FLOAT->Find(data, pointer))
				pointers.push_back(pointer);
			return true;
			break;
		}
		case Ne: {
			bpt_FLOAT->FindNonEqual(data, pointers);
			return true;
			break;
		}

		}
		break;
	}
	case CHAR:
	{
		std::string data = cond.value;
		switch (cond.op)
		{
		case Lt: {
			bpt_STRING->FindLess(data, false, pointers);
			return true;
			break;
		}
		case Le: {
			bpt_STRING->FindLess(data, true, pointers);
			return true;
			break;
		}
		case Gt: {
			bpt_STRING->FindLarger(data, false, pointers);
			return true;
			break;
		}
		case Ge: {
			bpt_STRING->FindLarger(data, true, pointers);
			return true;
			break;
		}
		case Eq: {
			Pointer pointer;
			if (bpt_STRING->Find(data, pointer))
				pointers.push_back(pointer);
			return true;
			break;
		}
		case Ne: {
			bpt_STRING->FindNonEqual(data, pointers);
			return true;
			break;
		}

		}
		break;
	}

	}
}

Pointer IndexManager::FindPointer(std::string& value, int type)
{
	switch (type)
	{
	case INT: {
		if (bpt_INT == NULL) return 0;
		int data = atoi(value.c_str());
		return bpt_INT->FindPointer(data);
		break;
	}
	case FLOAT: {
		if (bpt_FLOAT == NULL) return 0;
		float data = atof(value.c_str());
		return bpt_FLOAT->FindPointer(data);
		break;
	}
	case CHAR: {
		if (bpt_STRING == NULL) return 0;
		return bpt_STRING->FindPointer(value);
		break;
	}
	}
	return 0;
}

bool IndexManager::Find(std::string& value, int type)
{
	Pointer pointer;
	switch (type)
	{
	case INT: {
		if (bpt_INT == NULL) return 0;
		int data = atoi(value.c_str());
		return bpt_INT->Find(data, pointer);
		break;
	}
	case FLOAT: {
		if (bpt_FLOAT == NULL) return 0;
		float data = atof(value.c_str());
		return bpt_FLOAT->Find(data, pointer);
		break;
	}
	case CHAR: {
		if (bpt_STRING == NULL) return 0;
		return bpt_STRING->Find(value, pointer);
		break;
	}
	}
	return 0;
}

IndexManager::~IndexManager()
{
	ResetBptInt(true);
	ResetBptFloat(true);
	ResetBptString(true);
}

