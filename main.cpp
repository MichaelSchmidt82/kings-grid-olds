#include "globals.h"
#include "BigIntegerLibrary.hh"
#include "types.h"
#include "node.h"
#include "directions_frameshift.h"

// Prototypes //
int countMarkedNodes(Pattern arr,
	int size,
	int min);

void increment(Pattern pattern,
	int size,
	int idx = 0);

void decrement(Pattern pattern,
	int size,
	int idx = 0);

void logSolution(const Graph& graph,
	int cols,
	int rows,
	int size,
	int trd,
	int count);

bool isSolution(Graph& graph,
	Mode mode,
	Distinguish callbackMethod,
	CoveredbyMethod coveredby_cb,
	int cols,
	int rows,
	int size,
	int count,
	int& minimum,
	bool initial = true);

inline void clearSets(Graph & graph);

bool checkDistinguishingSTD(Graph& graph,
	int size);

bool checkDistinguishingRED(Graph& graph,
	int size);

//NYI
bool checkDistinguishingDET(Graph& graph,
	int size);

size_t compareSets(const SetPtr left,
	const SetPtr right);

inline void printGraph(std::ostream& out,
	const Graph& graph,
	int cols,
	int rows,
	int size);

void initialize(
	Pattern& patBegInc,
	Pattern& patMidDec,
	Pattern& patMidInc,
	Pattern& patEndDec,
	ParamPtr& detBegInc,
	ParamPtr& detMidDec,
	ParamPtr& detMidInc,
	ParamPtr& detEndDec,
	int& cols, int& rows, int& size);

void attacker(ParamPtr details,
	BigInteger& rounds,
	Arithmetic arithmetic,
	int& minimum);

// Globals
mutex roundsFrontLock;
mutex roundsBackLock;
mutex minLock;
mutex solutionLock;

bool trdBegInc_Done;
bool trdMidDec_Done;
bool trdMidInc_Done;
bool trdEndDec_Done;

int main() {
	int				cols;
	int				rows;
	int				size;
	int				minimum;
	BigInteger		roundsFront = 0;
	BigInteger		roundsBack = 0;
	BigInteger		maxRounds;

	Pattern			patBegInc = nullptr;  // Warning: Parameter objects also references these.
	Pattern			patMidDec = nullptr;
	Pattern			patMidInc = nullptr;
	Pattern			patEndDec = nullptr;

	ParamPtr		detBegInc = nullptr;
	ParamPtr		detMidDec = nullptr;
	ParamPtr		detMidInc = nullptr;
	ParamPtr		detEndDec = nullptr;

	initialize(patBegInc, patMidDec, patMidInc, patEndDec, detBegInc, detMidDec, detMidInc, detEndDec, cols, rows, size);

	minimum = size;

	thread attacker1 (attacker, cref(detBegInc), ref(roundsFront), increment, ref(minimum));
	thread attacker2 (attacker, cref(detMidDec), ref(roundsFront), decrement, ref(minimum));
	thread attacker3 (attacker, cref(detMidInc), ref(roundsBack), increment, ref(minimum));
	thread attacker4 (attacker, cref(detEndDec), ref(roundsBack), decrement, ref(minimum));

	maxRounds = detBegInc->maxRounds;
	while (!trdBegInc_Done || !trdMidDec_Done || !trdMidInc_Done || !trdEndDec_Done) {
		std::this_thread::sleep_for(3s);
		cout << roundsFront + roundsBack << " of " << maxRounds * 2 << " completed...\r";
	}
	attacker1.join();
	attacker2.join();
	attacker3.join();
	attacker4.join();

	cout << endl << "Total rounds: " << roundsFront + roundsBack << " processed";

	// Tidy up
	delete[]	patBegInc;
	delete[]	patMidInc;
	delete[]	patMidDec;
	delete[]	patEndDec;

	delete		detBegInc;
	delete		detMidDec;
	delete		detMidInc;
	delete		detEndDec;

	detBegInc = nullptr;
	detMidDec = nullptr;
	detMidInc = nullptr;
	detEndDec = nullptr;

	return 0;
}

void attacker (ParamPtr params, BigInteger& rounds, Arithmetic arithmetic, int& minimum) {
	int				numOfNodes = 0;
	int				rows = params->rows;
	int				cols = params->cols;
	int				size = params->size;

	BigInteger		maxRounds = params->maxRounds;
	Position		threadNum = params->pos;
	Pattern			pattern = params->pattern;
	Position		pos = params->pos;
	Distinguish		distinguishingCallback = params->distinguishingCallback;
	CoveredbyMethod coveredByCallback = params->gridCoveredByCallback;

	Mode			mode = params->mode;
	NodePtr			node_ptr = nullptr;
	Graph			graph;
	
	mutex& roundsLock = (pos == BEGINNING || pos == MIDDLE_DEC) ? roundsFrontLock : roundsBackLock;

	for (int i = 0; i < size; ++i) {
		node_ptr = new Node(i, cols, rows, size);
		graph.push_back(node_ptr);
	}

	node_ptr = nullptr;

	do {
		arithmetic(pattern, size, 0);
		numOfNodes = countMarkedNodes(pattern, size, minimum);

		// Critical Section for rounds
		roundsLock.lock();
		rounds++;
		roundsLock.unlock();
		// End CS

		// Don't check if node count is greater or equal to the current minimum
		if (numOfNodes >= minimum)
			continue;

		// Create the graph
		for (int i = 0; i < size; i++) {
			if (pattern[i])
				graph[i]->mark();
			else
				graph[i]->unmark();
		}

		// Check if it's a solution
		if (isSolution(graph, mode, distinguishingCallback, coveredByCallback, cols, rows, size, numOfNodes, minimum)) {
			logSolution(graph, cols, rows, size, int(pos + 1), minimum);
		}
			

	} while (rounds < maxRounds);

	// Signal thread completition
	if (pos == BEGINNING)
		trdBegInc_Done = true;
	else if (pos == MIDDLE_DEC)
		trdMidDec_Done = true;
	else if (pos == MIDDLE_INC)
		trdMidInc_Done = true;
	else
		trdEndDec_Done = true;
}

void initialize (
	Pattern& patBegInc,
	Pattern& patMidDec,
	Pattern& patMidInc,
	Pattern& patEndDec,
	ParamPtr& paramsBegInc,
	ParamPtr& paramsMidDec,
	ParamPtr& paramsMidInc,
	ParamPtr& paramsEndDec,
	int& cols, int& rows, int& size) {

	char choice;
	Distinguish distinguishingCallback = nullptr;
	CoveredbyMethod coveredByCallback = nullptr;

	cout << "Enter rows: ";
	cin >> rows;

	cout << "Enter columns: ";
	cin >> cols;

	cout << "Grid: " << endl;
	cout << "\ts) SQUARE" << endl;
	cout << "\tk) KINGS" << endl;
	cout << ">>> ";
	cin >> choice;

	switch (choice) {
		case 's':
		case 'S':
			coveredByCallback = Squaregrid::isCoveredBy;
			break;
		case 'k':
		case 'K':
			coveredByCallback = Kingsgrid::isCoveredBy;
			break;
	default:
		coveredByCallback = Kingsgrid::isCoveredBy;
		break;
	}

	cout << "Mode: " << endl;
	cout << "\tn) NORMAL" << endl;
	cout << "\tr) REDUNDANT (Experimental)" << endl;
	cout << "\td) DETECTOR (NYI)" << endl;
	cout << ">>> ";
	cin >> choice;

	switch (choice) {
		case 'n':
		case 'N':
			distinguishingCallback = checkDistinguishingSTD;
			break;
		case 'r':
		case 'R':
			distinguishingCallback = checkDistinguishingRED;
			break;
		case 'd':
		case 'D':
			distinguishingCallback = checkDistinguishingDET;
			break;
		default:
			distinguishingCallback = checkDistinguishingSTD;
			break;
	}

	size = cols * rows;

	patBegInc = new bool[size];
	patMidDec = new bool[size];
	patMidInc = new bool[size];
	patEndDec = new bool[size];

	for (int i = 0; i < size; ++i) {
		patBegInc[i] = false;
		patMidDec[i] = true;
		patMidInc[i] = false;
		patEndDec[i] = true;
	}

	patMidDec[size - 1] = false;
	patMidInc[size - 1] = true;
	
	paramsBegInc = new Parameters();
	paramsMidDec = new Parameters();
	paramsMidInc = new Parameters();
	paramsEndDec = new Parameters();

	paramsBegInc->pattern = patBegInc;
	paramsMidDec->pattern = patMidDec;
	paramsMidInc->pattern = patMidInc;
	paramsEndDec->pattern = patEndDec;

	paramsBegInc->distinguishingCallback = distinguishingCallback;
	paramsMidDec->distinguishingCallback = distinguishingCallback;
	paramsMidInc->distinguishingCallback = distinguishingCallback;
	paramsEndDec->distinguishingCallback = distinguishingCallback;

	paramsBegInc->gridCoveredByCallback = coveredByCallback;
	paramsMidDec->gridCoveredByCallback = coveredByCallback;
	paramsMidInc->gridCoveredByCallback = coveredByCallback;
	paramsEndDec->gridCoveredByCallback = coveredByCallback;

	paramsBegInc->rows = rows;
	paramsMidDec->rows = rows;
	paramsMidInc->rows = rows;
	paramsEndDec->rows = rows;

	paramsBegInc->cols = cols;
	paramsMidDec->cols = cols;
	paramsMidInc->cols = cols;
	paramsEndDec->cols = cols;

	paramsBegInc->size = size;
	paramsMidDec->size = size;
	paramsMidInc->size = size;
	paramsEndDec->size = size;

	// Determine the max rounds
	paramsBegInc->maxRounds = 2;
	paramsMidDec->maxRounds = 2;
	paramsMidInc->maxRounds = 2;
	paramsEndDec->maxRounds = 2;

	for (int i = size - 2; i > 0; i--) {
		paramsBegInc->maxRounds *= 2;
		paramsMidDec->maxRounds *= 2;
		paramsMidInc->maxRounds *= 2;
		paramsEndDec->maxRounds *= 2;
	}

	paramsBegInc->pos = BEGINNING;
	paramsMidDec->pos = MIDDLE_DEC;
	paramsMidInc->pos = MIDDLE_INC;
	paramsEndDec->pos = END;
	
	trdBegInc_Done = false;
	trdMidDec_Done = false;
	trdMidInc_Done = false;
	trdEndDec_Done = false;
}

 bool isSolution (Graph& graph,
	Mode mode,
	Distinguish distCallback,
	CoveredbyMethod coveredByCallback,
	int cols,
	int rows,
	int size,
	int count,
	int& minimum,
	bool initial) {
	bool edgeCaseFlag, hasDistinguishedSets;

	// Check board and dominatorns
	// NOTE: isCoveredBy regenerates the covering set.
	clearSets(graph);
	for (int i = 0; i < size; i++)
		if (coveredByCallback(graph, graph[i]->getSet(),i,rows,cols,size) > 0)
			continue;
		else
			return false;
			
	hasDistinguishedSets = distCallback(graph, size);
	if (!hasDistinguishedSets)
		return false;

	// Cases where tile is covered, but the repetition of the tile
	// results in non-distuinged sets.
	if (initial) {
		// Shift viewing frame to check edge cases
		FrameShift::up(graph, size, cols);
		edgeCaseFlag = isSolution(graph, mode, distCallback, coveredByCallback, cols, rows, size, count, minimum, false);
		FrameShift::down(graph, size, cols);
		if (!edgeCaseFlag)
			return false;

		FrameShift::down(graph, size, cols);
		edgeCaseFlag = isSolution(graph, mode, distCallback, coveredByCallback, cols, rows, size, count, minimum, false);
		FrameShift::up(graph, size, cols);
		if (!edgeCaseFlag)
			return false;

		FrameShift::left(graph, cols, rows, size);
		edgeCaseFlag = isSolution(graph, mode, distCallback, coveredByCallback, cols, rows, size, count, minimum, false);
		FrameShift::right(graph, cols, rows, size);
		if (!edgeCaseFlag)
			return false;

		FrameShift::right(graph, cols, rows, size);
		edgeCaseFlag = isSolution(graph, mode, distCallback, coveredByCallback, cols, rows, size, count, minimum, false);
		FrameShift::left(graph, cols, rows, size);
		if (!edgeCaseFlag)
			return false;

		// Regenerate Sets for original tile.
		clearSets(graph);
		for (int i = 0; i < size; i++)
			Kingsgrid::isCoveredBy(graph, graph[i]->getSet(), i, rows, cols, size);
	}

	if (initial && count < minimum) {
		// Critital Section to change mimimum
		minLock.lock();
		minimum = count;
		minLock.unlock();
		// End CS
		return true;
	}

	return true;
}

void increment (Pattern pattern, int size, int idx) {
	if (idx < size) {
		if (!pattern[idx])
			pattern[idx] = true;
		else {
			pattern[idx] = false;
			increment(pattern, size, idx + 1);
		}
	}
}

void decrement (Pattern pattern, int size, int idx) {
	if (idx < size) {
		if (pattern[idx])
			pattern[idx] = false;
		else {
			pattern[idx] = true;
			decrement(pattern, size, idx + 1);
		}
	}
}

int countMarkedNodes (Pattern pattern, int size, int min) {
	int sum = 0;

	for (int i = 0; i < size; i++)
		if (pattern[i])
			sum++;

	return sum;
}

void logSolution (const Graph& graph, int cols, int rows, int size, int trd, int count) {
	
	solutionLock.lock();
	static int file_num = 0;

	ofstream file_out;

	string filename = string();
	filename += std::to_string(count); 
	filename += "-trd-";
	filename += std::to_string(trd);
	filename += "-";
	filename += std::to_string(file_num);

	filename += ".log";
	file_out.open(filename.c_str());

	file_out << "Thread #" << trd << " found the solution:" << endl;

	// Print a small graph
	printGraph(file_out, graph, cols, rows, size);

	// Print each nodes dominator
	for (NodePtr n : graph) {
		file_out << n->getId() << ": " << n->getSetString();
		file_out << endl;
	}

	file_num++;
	solutionLock.unlock();
}

inline void clearSets (Graph& graph) {
	for (NodePtr n : graph)
		n->clearSet();
}

bool checkDistinguishingSTD (Graph& graph, int size) {
	SetPtr lhs_set;

	for (int i = 0; i < size - 1; i++) {
		lhs_set = graph[i]->getSet();
		for (int j = i + 1; j < size; j++) {
			if (*(graph[j]->getSet()) == *lhs_set)
				return false;
		}
	}
	return true;
}

// NYI
bool checkDistinguishingDET (Graph& graph, int size) {

	return false;
}

// Experimental
bool checkDistinguishingRED (Graph& graph, int size) {
	int diff;
	SetPtr L_Set;
	SetPtr R_Set;

	// Compare LEFT set to RIGHT
	for (int i = 0; i < graph.size() - 1; i++) {
		L_Set = graph[i]->getSet();
		for (int j = i + 1; j < graph.size(); j++){
			R_Set = graph[j]->getSet();

			diff = R_Set->size() - L_Set->size();
			diff = abs(diff);
			if (diff > 1)
				continue;

			if (compareSets(L_Set, R_Set) < 2)
				return false;
		}
	}
	return true;
}

size_t compareSets (const SetPtr reference_set, const SetPtr compare_set) {
	int same_count = 0;
	for (Element refElement : *reference_set) {
		for (Element cmpElement : *compare_set) {
			if (cmpElement == refElement) {
				same_count++;
			}
		}
	}

	if (reference_set->size() > compare_set->size())
		return reference_set->size() - same_count;
	else
		return compare_set->size() - same_count;
}

inline void printGraph (std::ostream& out, const Graph& graph, int cols, int rows, int size) {
	for (int i = 0; i < size; i++) {
		if (graph[i]->isMarked())
			out << 'x';
		else
			out << '-';
		out << ' ';

		if (i % cols == cols - 1)
			out << endl;
	}
}