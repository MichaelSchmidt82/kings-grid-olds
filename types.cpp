#include "types.h"

bool Element::operator == (const Element & rhs) const {
	return (this->id == rhs.id && this->prime == rhs.prime); 
}

bool Element::operator != (const Element & rhs) const {
	return (this->id != rhs.id || this->prime != rhs.prime);
}

bool Element::operator < (const Element & rhs) const{
	if (this->id < rhs.id)
		return true; 
	else
		return false;
}