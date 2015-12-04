#ifndef NODESTRUCT
#define NODESTRUCT

#include "typedef.h"

struct Node {

	Node *_left, *_right;
	char _data;
	int _freq;

	Node (char data, int freq) {
		_data = data;
		_freq = freq;
		_left = _right = NULL;
	}

	Node (Node *l, Node *r) {
		_data = '\0';
		_freq = l->_freq + r->_freq;
		_left = l;
		_right = r;
	}

	void dump(int level=0) {
		for (int i = 0; i < level; ++i)
			if (i == level-1)
				std::cout << "|--";
			else std::cout << "|  ";
		if (empty())
			std::cout << "NULL" << std::endl;
		else
			std::cout << _data << std::endl;
		if (_left != NULL)
			_left->dump(level+1);
		if (_right != NULL)
			_right->dump(level+1);
	}

	huffmanMap *makeMap(huffmanMap *map = NULL, std::vector<bool> *v = NULL) {
		if (map == NULL)
			map = new huffmanMap();
		if (empty()) {
			if (v == NULL)
				v = new std::vector<bool>;
			if (_left != NULL) {
				v->push_back(0);
				_left->makeMap(map, v);
				v->pop_back();
			}
			if (_right != NULL) {
				v->push_back(1);
				_right->makeMap(map, v);
				v->pop_back();
			}
		} else {
			(*map)[_data] = *v;
		}
		return map;
	}

	bool empty() { return _left != NULL || _right != NULL; }

	void writeToFile() {

	}
};

#endif