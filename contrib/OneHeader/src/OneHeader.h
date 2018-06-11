#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdarg.h>			// std::va_list
using namespace std;

#define STREAM_SCANNER_BUFFER_SIZE 4096

namespace OneHeader {

/**
 * @class StreamScanner
 * A reusable class to scan and parse a stream \n
 */
class StreamScanner {
public:
	StreamScanner() {

	}
	virtual ~StreamScanner() {

	}
	bool scanStream(istream& streamIn, ...);
	virtual bool bufferHandle(string& streamBuffer, bool final, va_list& args) {
		return false;
	}
};

/**
 * @class Document
 * utilities for the reading the One Header document \n
 */
class Document: public StreamScanner {
public:
	Document() {

	}
	~Document() {

	}
	bool stream(ifstream &streamIn, vector<string> &includeList);
	bool bufferHandle(string& streamBuffer, bool final, va_list& args);
};

/**
 * @class Combine
 * utilities for the combining One Header document \n
 */
class Combine: public StreamScanner {
public:
	Combine() {

	}
	~Combine() {

	}
	bool copy(ifstream &streamIn, ofstream &streamOut);
	bool bufferHandle(string& streamBuffer, bool final, va_list& args);
};

bool StreamScanner::scanStream(istream& streamIn, ...) {
	unsigned int fileSize, filePos, bufferSize, mode;
	string streamBuffer;
	bufferSize = STREAM_SCANNER_BUFFER_SIZE;
	fileSize = filePos = mode = 0;
	streamBuffer = "";
	char bufferInChar[STREAM_SCANNER_BUFFER_SIZE];
	streamIn.seekg(0, ios::end); // set the pointer to the end
	fileSize = static_cast<unsigned int>(streamIn.tellg()); // get the length of the file
	streamIn.seekg(0, ios::beg); // set the pointer to the beginning
	while (filePos < fileSize) {
		streamIn.seekg(filePos, ios::beg); // seek new position
		if (filePos + bufferSize > fileSize) {
			bufferSize = fileSize - filePos;
		}
		memset(bufferInChar, 0, sizeof(bufferInChar)); // zero out buffer
		streamIn.read(bufferInChar, bufferSize);
		streamBuffer.append(bufferInChar, bufferSize);
		va_list args;
		va_start(args, &streamIn);
		bufferHandle(streamBuffer, false, args);
		va_end(args);
		// advance buffer
		filePos += bufferSize;
	}
	// handle the remaining buffer
	va_list args;
	va_start(args, &streamIn);
	bufferHandle(streamBuffer, true, args);
	va_end(args);
	return true;
}

bool Document::stream(ifstream &streamIn, vector<string> &includeList) {
	return scanStream(streamIn, &includeList);
}

bool Document::bufferHandle(string& streamBuffer, bool final, va_list& args) {
	vector<string>& includeList = va_arg(args, vector<string>);
	unsigned int mode;
	size_t directiveStartPos, directiveEndPos;
	mode = directiveStartPos = directiveEndPos = 0;
	while (true) {
		if (mode == 0) {
			directiveStartPos = streamBuffer.find("#include \"");
			if (directiveStartPos != string::npos) {
				mode = 1;
				// erase data before #
				streamBuffer.erase(0, directiveStartPos);
			} else {
				if (streamBuffer.size() > 20) {
					streamBuffer.erase(0, streamBuffer.size() - 20);
				}
				break;
			}
		} else if (mode == 1) {
			directiveEndPos = streamBuffer.substr(10).find('"');
			if (directiveEndPos != string::npos) {
				includeList.push_back(streamBuffer.substr(10, directiveEndPos));
				// erase data at '"'
				cout << "[OneHeader] FOUND "
						<< streamBuffer.substr(0, directiveEndPos + 11) << "\n";
				streamBuffer.erase(0, directiveEndPos + 11);
				mode = 0;
			} else {
				break;
			}
		}
	}
	if (final) {
		streamBuffer.clear();
	}
	return true;
}

bool Combine::copy(ifstream &streamIn, ofstream &streamOut) {
	return scanStream(streamIn, &streamOut);
}

bool Combine::bufferHandle(string& streamBuffer, bool final, va_list& args) {
	ostream& streamOut = va_arg(args, ostream);
	unsigned int mode;
	size_t directiveStartPos, directiveEndPos;
	mode = directiveStartPos = directiveEndPos = 0;
	while (true) {
		if (mode == 0) {
			directiveStartPos = streamBuffer.find("#include \"");
			if (directiveStartPos != string::npos) {
				mode = 1;
				// write data before #
				streamOut << streamBuffer.substr(0, directiveStartPos);
				streamBuffer.erase(0, directiveStartPos);
			} else {
				// don't write whole thing because we may have ended in the middle of the token
				if (streamBuffer.size() > 20) {
					streamOut
							<< streamBuffer.substr(0, streamBuffer.size() - 20);
					streamBuffer.erase(0, streamBuffer.size() - 20);
				}
				break;
			}
		} else if (mode == 1) {
			directiveEndPos = streamBuffer.substr(10).find('"');
			if (directiveEndPos != string::npos) {
				// erase data at '"'
				cout << "[OneHeader] FOUND"
						<< streamBuffer.substr(0, directiveEndPos + 11) << "\n";
				streamBuffer.erase(0, directiveEndPos + 11);
				mode = 0;
			} else {
				break;
			}
		}
	}
	if (final) {
		streamOut << streamBuffer;
		streamBuffer.clear();
	}
	return true;
}

}
