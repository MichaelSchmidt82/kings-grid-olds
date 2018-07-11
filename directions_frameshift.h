#ifndef DIRECTIONS_FRAMESHIFT_H
#define DIRECTIONS_FRAMESHIFT_H

#include "globals.h"

#include "node.h"
#include "types.h"

namespace Directions {
	int north(int idx, int col, int size, bool & is_prime);
	int south(int idx, int col, int size, bool & is_prime);
	int east(int idx, int col, int rows, int size, bool & is_prime);
	int west(int idx, int col, int rows, int size, bool & is_prime);
	int north_east(int idx, int col, int rows, int size, bool & is_prime);
	int south_east(int idx, int col, int rows, int size, bool & is_prime);
	int north_west(int idx, int col, int rows, int size, bool & is_prime);
	int south_west(int idx, int col, int rows, int size, bool & is_prime);
}

namespace FrameShift {
	void right(Graph & graph, int cols, int rows, int size);
	void left(Graph & graph, int cols, int rows, int size);
	void up(Graph & graph, int size, int cols);
	void down(Graph & graph, int size, int cols);

	inline void reorder(Graph & graph);
}

namespace Kingsgrid {
	// Returns number of nodes covering this node
	// Generates the set the node is covered by.
	size_t isCoveredBy(const Graph & graph, SetPtr set, int id, int rows, int cols, int size);
}

namespace Squaregrid {
	size_t isCoveredBy(const Graph & graph, SetPtr set, int id, int rows, int cols, int size);
}
#endif // !DIRECTIONS_FRAMESHIFT_H