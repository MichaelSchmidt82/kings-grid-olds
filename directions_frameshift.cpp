#include "globals.h"

#include "directions_frameshift.h"
#include "types.h"
#include "node.h"

using std::deque;

int Directions::south(int idx, int col, int size, bool& is_prime) {
	if (idx < size - col) {
		is_prime = false;
		return idx + col;
	} else {
		is_prime = true;
		return idx % col;
	}
}

int Directions::north(int idx, int cols, int size, bool& is_prime) {
	if (idx < cols) {
		is_prime = true;
		return size - cols + idx;
	} else {
		is_prime = false;
		return idx - cols;
	}
}

int Directions::east(int idx, int col, int rows, int size, bool& is_prime) {
	if (idx % col == col - 1) {
		is_prime = true;
		return idx - col + 1;
	} else {
		is_prime = false;
		return idx + 1;
	}
}

int Directions::west(int idx, int col, int rows, int size, bool& is_prime) {
	if (idx % col) {
		is_prime = false;
		return idx - 1;
	} else {
		is_prime = true;
		return idx + col - 1;
	}
}

int Directions::north_east(int idx, int col, int rows, int size, bool& is_prime) {
	bool step_flag = false;
	bool final_flag = false;
	int step = Directions::north(idx, col, size, step_flag);
	int final = Directions::east(step, col, rows, size, final_flag);

	is_prime = (step_flag || final_flag);
	return final;
}

int Directions::south_east(int idx, int col, int rows, int size, bool& is_prime) {
	bool step_flag = false;
	bool final_flag = false;
	int step = Directions::south(idx, col, size, step_flag);
	int final = Directions::east(step, col, rows, size, final_flag);

	is_prime = (step_flag || final_flag);
	return final;
}

int Directions::north_west(int idx, int col, int rows, int size, bool& is_prime) {
	bool step_flag = false;
	bool final_flag = false;
	int step = Directions::north(idx, col, size, step_flag);
	int final = Directions::west(step, col, rows, size, final_flag);

	is_prime = (step_flag || final_flag);
	return final;
}

int Directions::south_west(int idx, int col, int rows, int size, bool& is_prime) {
	bool step_flag = false;
	bool final_flag = false;
	int step = Directions::south(idx, col, size, step_flag);
	int final = Directions::west(step, col, rows, size, final_flag);

	is_prime = (step_flag || final_flag);
	return final;
}

void FrameShift::right(Graph & graph, int cols, int rows, int size) {
	deque<NodePtr> rotator;

	int j;
	int k;
	for (int i = 0; i < size; i += cols) {
		k = 1;
		for (j = i + 1; k < cols; j++, k++)
			rotator.push_back(graph[j]);
		
		rotator.push_back(graph[i]);
		
		k = 0;
		for (j = i; k < cols; k++, j++) {
			graph[j] = rotator.front();
			rotator.pop_front();
		}
	}
	FrameShift::reorder(graph);
}

void FrameShift::left(Graph & graph, int cols, int rows, int size) {
	deque<NodePtr> rotator;

	int j;
	for (int i = 0; i < size; i += cols) {
		for (j = i; j < cols + i - 1; j++)
			rotator.push_back(graph[j]);

		rotator.push_front(graph[j]);
		j -= (cols - 1);

		for (int k = 0; k < cols; k++) {
			graph[j] = rotator.front();
			rotator.pop_front();
			j++;
		}
	}
	FrameShift::reorder(graph);
}

void FrameShift::up(Graph & graph, int size, int cols) {
	vector<NodePtr> temp;
	temp.reserve(cols);

	for (int i = size - cols; i < size; i++)
		temp.push_back(graph[i]);

	for (int k = 0; k < cols; k++) {
		for (int i = size - cols + k; i > k; i -= cols)
			graph[i] = graph[i - cols];
		graph[k] = temp[k];
	}

	FrameShift::reorder(graph);
}

void FrameShift::down(Graph & graph, int size, int cols) {
	vector<NodePtr> temp;
	temp.reserve(cols);

	for (int i = 0; i < cols; i++)
		temp.push_back(graph[i]);

	for (int k = cols, j = 0; k > 0; k--, j++) {
		for (int i = 0 + j; i + cols < size + j; i += cols)
			graph[i] = graph[i + cols];
		graph[size - k] = temp[j];
	}

	FrameShift::reorder(graph);
}

inline void FrameShift::reorder(Graph & graph) {
	int i = 0;
	
	for (NodePtr n : graph) {
		n->setId(i);
		i++;
	}
}

// Returns number of nodes covering this node
// Generates the set the node is covered by.
size_t Kingsgrid::isCoveredBy(const Graph& graph, SetPtr set, int id, int rows, int cols, int size) {

	bool is_prime;
	int idx = Directions::north(id, cols, size, is_prime);

	Element temp;

	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::north_east(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::east(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::south_east(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::south(id, cols, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::south_west(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::west(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::north_west(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	if (set->size() > 0) {
		std::sort(set->begin(), set->end());
		return set->size();
	}
	else
		return 0;
}

// Returns number of nodes covering this node
// Generates the set the node is covered by.
size_t Squaregrid::isCoveredBy(const Graph& graph, SetPtr set, int id, int rows, int cols, int size) {

	bool is_prime;
	Element temp;

	int idx = Directions::north(id, cols, size, is_prime);

	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::east(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::south(id, cols, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	idx = Directions::west(id, cols, rows, size, is_prime);
	if (graph[idx]->isMarked()) {
		temp.id = graph[idx]->getId();
		temp.prime = is_prime;
		set->push_back(temp);
	}

	if (set->size() > 0) {
		std::sort(set->begin(), set->end());
		return set->size();
	}
	else
		return 0;
}