#ifndef TYPES_H
#define TYPES_H

#include "globals.h"
#include "BigInteger.hh"
#include "node.h"

using std::ofstream;
using std::vector;
using std::string;

struct Element {
	Element() : prime(false) {};

	int id;
	bool prime;

	bool operator == (const Element & rhs) const;
	bool operator != (const Element & lhs) const;
	bool operator < (const Element & rhs) const;
};

struct Parameters {
	bool* pattern;
	Position pos;
	Distinguish distinguishingCallback;
	CoveredbyMethod	gridCoveredByCallback;
	BigInteger maxRounds;
	Mode mode;

	int rows;
	int cols;
	int size;
};

#endif // !TYPES_H
