#pragma once

#include <string>
using std::string;

class NumberChecker {
public:
	static int DigestPureNumber(const char *startpos, int offset) {
		while (startpos[offset] >= '0' && startpos[offset] <= '9')
			offset++;
		return offset;
	}

	static int DigestSmallNumber(const char *startpos, int offset) {
		if (startpos[offset] == '.')
			return DigestPureNumber(startpos, offset + 1);
		else
			return offset;
	}

	static int DigestSpace(const char *startpos, int offset) {
		while (startpos[offset] == ' ')
			offset++;
		return offset;
	}

	static int DigestNumber(const char*startpos, int offset) {
		if (startpos[offset] == '-' || startpos[offset] == '+')
			offset += 1;
		int newoff = DigestSmallNumber(startpos, offset);
		if (newoff == offset + 1)
			return -1;
		else if (newoff > offset + 1)
		{
			offset = newoff;
			if (startpos[offset] == '\0')
				return offset;
		}
		else {
			newoff = DigestPureNumber(startpos, offset);
			newoff = DigestSmallNumber(startpos, newoff);
			if (newoff <= offset)
				return newoff;
			offset = newoff;
			if (startpos[offset] == '\0')
				return offset;
		}

		if (startpos[offset] == 'e' || startpos[offset] == 'E')
		{
			offset++;
			if (startpos[offset] == '+' || startpos[offset] == '-')
				offset++;
			newoff = DigestPureNumber(startpos, offset);
			if (newoff == offset)
				return -1;
			return newoff;
		}
		else
			return offset;

	}

	static bool isNumber(string s) {
		const char * data = s.data();
		int offset = DigestSpace(data, 0);
		int newoff = DigestNumber(data, offset);
		if (newoff == offset)
			return false;
		int i = offset;
		offset = DigestSpace(data, newoff);
		if (offset == s.size())
			return true;
		return false;
	}
};