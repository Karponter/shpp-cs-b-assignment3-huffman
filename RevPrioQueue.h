#include "typedef.h"
#include "nodestruct.h"

class RevPrioQueue {

private:
	std::list<Node*> *_data;

public:
	RevPrioQueue() { _data = new std::list<Node*>(); }

	void enqueue(Node* node) {
		if (_data->empty()) {
			_data->push_back(node);
		} else {
			std::list<Node*>::iterator it = _data->begin();
			while(it != _data->end()) {
				if ((*it)->_freq <= node->_freq) {
					_data->insert(it, node);
					return;
				}
				it++;
			}
			_data->push_back(node);
		}
		return;
	}

	Node* dequeue() {
		Node* retVal = _data->back();
		_data->pop_back();
		return retVal;
	}

	bool empty() { return _data->empty(); }

	int size() { return _data->size(); }

	~RevPrioQueue() { delete _data; }
};