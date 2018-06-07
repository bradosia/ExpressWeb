/*
 * Creates a monolithic header
 * <pre>
 * OneHeader.exe ../../src/OneHeader.h ../../include/OneHeaderOutput.h
 * </pre>
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
using namespace std;

bool copyProcess(ifstream &streamIn, ofstream &streamOut) {
	unsigned int fileSize, filePos, mode, bufferSize, directiveStartPos,
			directiveEndPos;
	bufferSize = 256;
	char bufferInChar[bufferSize + 1];
	string streamBuffer;
	unsigned int positionNone = (unsigned int) string::npos;
	mode = filePos = 0;
	streamIn.seekg(0, ios::end); // set the pointer to the end
	fileSize = streamIn.tellg(); // get the length of the file
	streamIn.seekg(0, ios::beg); // set the pointer to the beginning
	while (filePos < fileSize) {
		streamIn.seekg(filePos, ios::beg);
		if (filePos + bufferSize > fileSize) {
			bufferSize = fileSize - filePos;
		}
		memset(bufferInChar, 0, sizeof(bufferInChar));
		streamIn.read(bufferInChar, bufferSize);
		streamBuffer.append(bufferInChar);
		while (true) {
			if (mode == 0) {
				directiveStartPos = (unsigned int) streamBuffer.find(
						"#include \"");
				if (directiveStartPos != positionNone) {
					cout << "[OneHeader] FOUND #include \" AT "
							<< directiveStartPos << endl;
					mode = 1;
					// write data before #
					streamOut << streamBuffer.substr(0, directiveStartPos);
					streamBuffer.erase(0, directiveStartPos);
				} else {
					/*
					 * 2018-05-31 Update
					 * Do not write and erase stream buffer if token not found.
					 * Maybe buffer ended in the middle of the token.
					 */
					//streamOut << streamBuffer;
					//streamBuffer.clear();
					break;
				}
			} else if (mode == 1) {
				directiveEndPos = (unsigned int) streamBuffer.substr(10).find(
						'"');
				if (directiveEndPos != positionNone) {
					cout << "[OneHeader] FOUND \" AT " << directiveEndPos
							<< endl;
					mode = 0;
					// erase data at '"'
					cout << "[OneHeader] ERASING "
							<< streamBuffer.substr(0, directiveEndPos + 11)
							<< endl;
					streamBuffer.erase(0, directiveEndPos + 11);
				} else {
					break;
				}
			}
		}
		// advance buffer
		filePos += bufferSize;
	}
	// nothing to do, just write
	streamOut << streamBuffer;
	return true;
}

bool oneHeaderToIncludeList(ifstream &streamIn, vector<string> &includeList) {
	unsigned int fileSize, filePos, mode, bufferSize, directiveStartPos,
			directiveEndPos;
	bufferSize = 256;
	char bufferInChar[bufferSize + 1];
	string streamBuffer;
	bufferInChar[bufferSize] = 0;
	mode = filePos = 0;
	streamIn.seekg(0, ios::end); // set the pointer to the end
	fileSize = streamIn.tellg(); // get the length of the file
	streamIn.seekg(0, ios::beg); // set the pointer to the beginning
	while (filePos < fileSize) {
		streamIn.seekg(filePos, ios::beg);
		if (filePos + bufferSize > fileSize) {
			bufferSize = fileSize - filePos;
			char bufferInCharLast[bufferSize + 1];
			bufferInChar[bufferSize] = 0;
			streamIn.read(bufferInCharLast, bufferSize);
			streamBuffer.append(bufferInCharLast);
		} else {
			streamIn.read(bufferInChar, bufferSize);
			streamBuffer.append(bufferInChar);
		}
		while (true) {
			if (mode == 0) {
				directiveStartPos = (unsigned int) streamBuffer.find(
						"#include \"");
				if (directiveStartPos != (unsigned int) string::npos) {
					mode = 1;
					// erase data before #
					streamBuffer.erase(0, directiveStartPos);
				} else {
					// nothing to do
					streamBuffer.clear();
					break;
				}
			} else if (mode == 1) {
				directiveEndPos = (unsigned int) streamBuffer.substr(10).find(
						'"');
				if (directiveEndPos != (unsigned int) string::npos) {
					includeList.push_back(
							streamBuffer.substr(10, directiveEndPos));
					// erase data at '"'
					streamBuffer.erase(0, directiveEndPos + 11);
					mode = 0;
				} else {
					break;
				}
			}
		}
		// advance buffer
		filePos += bufferSize;
	}
	return true;
}

int main(int argc, char** argv) {
	char *fileInName, *fileOutputName;
	bool lexorFlag;
	lexorFlag = false;
	vector<string> includeList;
	ifstream streamIn;
	ofstream fileOutput;
	if (argc < 3) {
		cout
				<< "Use the program like this:\nOneHeader.exe ../../src/OneHeader.h ../../include/OneHeaderOutput.h\n";
	} else {
		fileInName = argv[1];
		fileOutputName = argv[2];
		cout << "[OneHeader] Input File: " << fileInName << endl;
		cout << "[OneHeader] Output File: " << fileOutputName << endl;
		streamIn.open(fileInName, ios::binary);
		fileOutput.open(fileOutputName, ios::binary);
		if (!streamIn.is_open()) {
			cout << "Could not open the input header";
		} else if (!fileOutput.is_open()) {
			cout << "Could not open the output header";
		} else {
			if (oneHeaderToIncludeList(streamIn, includeList)) {
				lexorFlag = true;
			}
			streamIn.close();
		}
	}
	if (lexorFlag) {
		string includeFilePath;
		string relativeDirectory = "";
		string fileInNameStr = string(fileInName);
		unsigned int slashLastPosition =
				(unsigned int) fileInNameStr.find_last_of('/');
		if (slashLastPosition != (unsigned int) string::npos) {
			relativeDirectory = fileInNameStr.substr(0, slashLastPosition + 1);
		}
		unsigned int i, n;
		n = includeList.size();
		for (i = 0; i < n; i++) {
			string includeFilePath;
			ifstream headerStreamIn;
			includeFilePath.append(relativeDirectory).append(includeList[i]);
			headerStreamIn.open(includeFilePath, ios::binary);
			if (headerStreamIn.is_open()) {
				copyProcess(headerStreamIn, fileOutput);
				fileOutput
						<< "\n\n//--------------------------------------------\n//--------------------------------------------\n\n";
				headerStreamIn.close();
			}
		}
		fileOutput.close();
	}
	return 0;
}
