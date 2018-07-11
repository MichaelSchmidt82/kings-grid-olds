#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <chrono>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::thread;
using std::mutex;
using std::ref;
using std::cref;
using std::string;
using std::vector;
using std::abs;

using namespace std::chrono_literals;

enum Position { BEGINNING, MIDDLE_DEC, MIDDLE_INC, END };
enum Mode { STANDARD, SYMMETRIC, ASYMMETRIC };

// Forward Declaritions;
struct Parameters;
struct Element;
class Node;

typedef Parameters* ParamPtr;

typedef		Node*				NodePtr;
typedef		vector<NodePtr>		Graph;
typedef		vector<Element>		Set;
typedef		vector<Element>*	SetPtr;
typedef		bool*				Pattern;
typedef		void (*Arithmetic) (Pattern pattern, int size, int idx);
typedef		bool (*Distinguish) (Graph& graph, int size);
typedef		size_t (*CoveredbyMethod) (const Graph& graph, SetPtr set, int id, int rows, int cols, int size);

#endif // Globals