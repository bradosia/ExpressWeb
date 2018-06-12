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
#include "OneHeader.h"
using namespace std;

int main(int argc, char** argv) {
	char *fileInName, *fileOutputName;
	bool lexorFlag;
	lexorFlag = false;
	vector<string> includeList;
	ifstream streamIn;
	ofstream fileOutput;
	OneHeader::Document doc;
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
			if (doc.stream(streamIn, includeList)) {
				lexorFlag = true;
			}
			streamIn.close();
		}
	}
	for (string& str : includeList) {
		cout << "inc " << str << "\n";
	}
	if (lexorFlag) {
		string includeFilePath;
		string relativeDirectory = "";
		string fileInNameStr(fileInName);
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
				doc.copy(headerStreamIn, fileOutput);
				fileOutput
						<< "\n\n//--------------------------------------------\n//--------------------------------------------\n\n";
				headerStreamIn.close();
			}
		}
		fileOutput.close();
	}
	return 0;
}
