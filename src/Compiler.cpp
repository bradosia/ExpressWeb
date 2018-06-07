#include "Compiler.h"

namespace ResourceFileUtility {
Compiler::Compiler() {
	callbackFileComplete = 0;
	callbackPackComplete = 0;
	callbackHandlerPtr = 0;
	metaDirectorySize = 128; // 128 entries
	tryToUpdate = true;
}

void Compiler::info(std::string filePathString) {
	filesystem::path filePath { filePathString };
	std::cout << "Opening \"" << filePath.string()
			<< "\" as the resource info file." << std::endl;
	filesystem::fstream fileIn;
	int readDirectoryJSONStatus;
	fileIn.open(filePath);
	readDirectoryJSONStatus = Parser::readDirectoryJSON(fileIn,
			resourceFileObj);
	if (readDirectoryJSONStatus) {
		if (readDirectoryJSONStatus == 1) {
			std::cout << "Failed opening \"" << filePath.string()
					<< "\" as the resource info file." << std::endl;
		} else if (readDirectoryJSONStatus == -1) {
			std::cout << "Unknown error in \"" << filePath.string()
					<< "\" as the resource info file." << std::endl;
		}
	} else {
		resourceFileObj.setDirectory(filePath.parent_path());
		Parser::getSize(resourceFileObj);
		std::cout << "Success opening \"" << filePath.string()
				<< "\" as the resource info file." << std::endl;
	}
}
void Compiler::estimate() {
	Compiler::estimate([](ResourceFile* resourceFilePtr) {});
}
void Compiler::estimate(CBvoidResourceFile handler_) {
	unsigned int i, n;
	// re check sizes
	Parser::getSize(resourceFileObj);
	// begin estimate threads and set processing flag
	n = resourceFileObj.assetListSize();
	for (i = 0; i < n; i++) {
		resourceFileObj.asset(i)->setProcess(true);
		estimateThreadList.push_back(
				new thread(Parser::estimate, resourceFileObj.asset(i)));
	}
	// custom handler
	estimateThreadList.push_back(new thread(handler_, &resourceFileObj));
	// join threads
	n = estimateThreadList.size();
	for (i = 0; i < n; i++) {
		estimateThreadList[i]->join();
	}
	// end the processing flag
	n = resourceFileObj.assetListSize();
	for (i = 0; i < n; i++) {
		resourceFileObj.asset(i)->setProcess(false);
	}
}
unsigned int Compiler::pack(std::string resourceFileName) {
	return Compiler::pack(resourceFileName, [](ResourceFile* resourceFilePtr) {});
}
unsigned int Compiler::pack(std::string resourceFileName, CBvoidResourceFile handler_) {
	unsigned int returnValue = resourceFileObj.write(resourceFileName);
	if (callbackFileComplete != 0) {
		char test[] = "packed a file test !!";
		callbackFileComplete(test);
	}
	return returnValue;
}

void Compiler::setCallbackFileComplete(CBintString handler_) {
	callbackFileComplete = handler_;
}

void Compiler::setCallbackPackComplete(CBintString handler_) {
	callbackPackComplete = handler_;
}

void Compiler::setCallback(CallbackHandler* handler_) {
	callbackHandlerPtr = handler_;
}

ResourceFile* Compiler::getResourceFile() {
	return &resourceFileObj;
}

//namespace end
}
