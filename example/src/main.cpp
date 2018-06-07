#include <string>
#include <iostream>
#include <ResourceFileUtility.h>
#include <boost/nowide/iostream.hpp>

using namespace boost;
using namespace std;

class Callback: public ResourceFileUtility::CallbackHandler {
public:
	int fileComplete(char* filePath) {
		cout << "Sucessfully Packed: " << filePath << endl;
		return 0;
	}
	int packComplete(char* filePath) {
		cout << "Sucessfully Packed all files!" << endl;
		return 0;
	}
	int estimateFileComplete(int fileID) {
		cout << "Sucessfully Packed all files!" << endl;
		return 0;
	}
};

void CBprocess(ResourceFileUtility::ResourceFile* resourceFilePtr) {
	unsigned int sizeTotal, sizeCurrent, loadBarNumber;
	sizeTotal = resourceFilePtr->getProcessingBytesTotal();
	while (true) {
		sizeCurrent = resourceFilePtr->getProcessingBytes();
		loadBarNumber =
				(unsigned int) (sizeCurrent / (double) sizeTotal * 40.0);
		cout << std::string(loadBarNumber, '#')
				<< std::string(40 - loadBarNumber, ' ') << "  " << sizeCurrent
				<< "/" << sizeTotal << "   Progress\r";
		if (sizeCurrent >= sizeTotal)
			break;
	}
	cout << "\n";
	cout << resourceFilePtr->estimateToString();
}

int main() {
	std::ios::sync_with_stdio(false);
	unsigned int fileNumber, sizeTotal, dataStatus;
	ResourceFileUtility::Compiler* RFUCompiler =
			new ResourceFileUtility::Compiler();
	RFUCompiler->info("resources.json");
	ResourceFileUtility::ResourceFile* resourceFile =
			RFUCompiler->getResourceFile();
	cout << resourceFile->infoToString();
	fileNumber = resourceFile->assetListSize();
	RFUCompiler->estimate(CBprocess);
	RFUCompiler->pack("assets.data", CBprocess);
	sizeTotal = resourceFile->getProcessingBytesTotal();

	ResourceFileUtility::Loader* RFULoader = new ResourceFileUtility::Loader();
	dataStatus = RFULoader->data("assets.data");
	if (dataStatus == 0) {
		cout << "Sucessfully opened data file!\n";
		ResourceFileUtility::Asset* coneAsset = RFULoader->info("cone");
		unsigned char* modelBytes = RFULoader->open("cone"); //loads all cone data to memory
		if (coneAsset != NULL && coneAsset->getInType() == "FILE_FORMAT") {
			// handle FILE_FORMAT
		}
	} else if (dataStatus == -1) {
		cout << "Unknown error opening data file!\n";
	} else if (dataStatus == 1) {
		cout << "Could not open resource file!\n";
	} else if (dataStatus == 2) {
		cout << "Resource file not compatible with this version!\n";
	}

	// Keep the console window open in debug mode.
	string temp;
	cout << "Press any key to exit." << endl;
	getline(cin, temp);
}
