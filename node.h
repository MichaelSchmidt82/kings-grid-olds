#ifndef NODE_H
#define NODE_H

#include "globals.h"
#include "types.h"

class Node {
public:
	Node() : m_Mark(false) {};
	Node(int _id, int cols, int rows, int size ) : 
		m_Mark(false), id(_id) {
		Node::m_Cols = cols;
		Node::m_Rows = rows;
		Node::m_Size = size;
	};

	~Node() {}

	// Observers
	bool isMarked() const { return m_Mark; }
	int getId() const { return id; }
	size_t getSetSize() const { return m_Set.size(); }

	// Mutators
	void setId(int _id) { id = _id; }
	void mark() { m_Mark = true; }
	void unmark() { m_Mark = false; }

	void clearSet();

	string getSetString();
	SetPtr getSet();

private:
	static int m_Size;
	static int m_Cols;
	static int m_Rows;

	bool m_Mark;
	int id;

	Set m_Set;
};
#endif