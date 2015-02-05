// Function to extract words from a string.
#include <cctype>	// included for isspace()
#include <string>
#include <vector>

#include "split.h"

using std::vector;	using std::string;

bool is_char(char ca, char cb)
{
	if (ca == cb)
		return true;
	return false;
}

vector<string> split(const string& s, char delimiter) {
	vector<string> ret;
	typedef string::size_type string_size;
	string_size i = 0;
	// invariant: we have processed characters [original value of i,i)
	while (i != s.size()) {
		// ignore leading blanks
		// invariant: characters in range [original i, current i)
		while (i != s.size() && (s[i] == delimiter))
			++i;

		//find the next word
		string_size j = i;
		//invariant none of the characters in range [original j, current j)

		// && operator evaluates left hand operators first
		// and if the left operator is false, !() is not evaluated
		while(j != s.size() && (s[j] != delimiter))
			j++;

		// if we found some nonwhitespace characters
		if (i != j) {
			// copy from 's' starting at i and taking j - i chars
			ret.push_back(s.substr(i, j - i));
			i=j;
		}
	}
	return ret;
}

