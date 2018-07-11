#include "globals.h"

#include "node.h"
#include "types.h"
#include "directions_frameshift.h"

int Node::m_Size = 0;
int Node::m_Cols = 0;
int Node::m_Rows = 0;

using std::string;
using std::mutex;

using Directions::north;
using Directions::south;
using Directions::east;
using Directions::west;
using Directions::north_east;
using Directions::north_west;
using Directions::south_east;
using Directions::south_west;

void Node::clearSet() {
	this->m_Set.clear();
}

string Node::getSetString() {
	string ret;

	Set::const_iterator Sc_iter;

	if (m_Set.size() == 0)
		return string();
	else {
		for (Element e : m_Set) {
			ret += std::to_string(e.id);

			if (e.prime)
				ret += '\'';

			ret += ", ";
		}

		return ret;
	}
}

SetPtr Node::getSet() {
	return &m_Set;
}