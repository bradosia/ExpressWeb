#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <functional>			// std::function
using namespace std;

#define STREAM_SCANNER_BUFFER_SIZE 4096

namespace OneHeader {

/**
 * @class StreamScanner
 * A reusable class to scan and parse a stream \n
 */
class StreamScanner {
public:
	static bool scanStream(istream& streamIn,
			std::function<void(string&, bool)> bufferHandleCallback);
};

/**
 * @class Document
 * utilities for the reading the One Header document \n
 */
class Document {
public:
	Document() {

	}
	~Document() {

	}
	bool stream(ifstream &streamIn, vector<string> &includeList);
	bool streamBufferHandle(string& streamBuffer, bool final,
			vector<string> &includeList);
	bool copy(ifstream &streamIn, ofstream &streamOut);
	bool copyBufferHandle(string& streamBuffer, bool final,
			ofstream &streamOut);
};

bool StreamScanner::scanStream(istream& streamIn,
		std::function<void(string&, bool)> bufferHandleCallback) {
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
		bufferHandleCallback(streamBuffer, false);
		// advance buffer
		filePos += bufferSize;
	}
	// handle the remaining buffer
	bufferHandleCallback(streamBuffer, true);
	return true;
}

bool Document::stream(ifstream &streamIn, vector<string> &includeList) {
	return StreamScanner::scanStream(streamIn,
			[this, &includeList](string& streamBuffer, bool final) {
				streamBufferHandle(streamBuffer, final, includeList);
			});
}

bool Document::streamBufferHandle(string& streamBuffer, bool final,
		vector<string> &includeList) {
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

bool Document::copy(ifstream &streamIn, ofstream &streamOut) {
	return StreamScanner::scanStream(streamIn,
			[this, &streamOut](string& streamBuffer, bool final) {
				copyBufferHandle(streamBuffer, final, streamOut);
			});
}

bool Document::copyBufferHandle(string& streamBuffer, bool final,
		ofstream &streamOut) {
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
