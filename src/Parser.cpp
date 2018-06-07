#include "Parser.h"

namespace ResourceFileUtility {

int Parser::readDirectoryJSON(filesystem::fstream& fileJSON,
		ResourceFile& resourceFileObj) {
	nlohmann::json j;
	std::string handle;
	std::string filePath;
	std::string inType;
	std::string outType;
	unsigned int i, n;
	bool parseFlag = false;
	nlohmann::json files;
	int status = -1;
	if (fileJSON.is_open()) {
		try {
			fileJSON >> j;
			parseFlag = true;
		} catch (...) {
			std::cout << "Failed to parse JSON!" << std::endl;
		}
	} else {
		status = 1;
	}
	/* the user defined json file is very unsafe
	 * we wrap all the attribute accessing in try{}catch(...){}
	 */
	if (parseFlag) {
		parseFlag = false;
		try {
			files = j.at("files");
			parseFlag = true;
		} catch (...) {
			std::cout << "JSON does not have the \"files\" attribute"
					<< std::endl;
		}
	}
	/* Too many nested try {} statements will confuse me so this part
	 * gets its own if block
	 */
	if (parseFlag) {
		n = files.size();
		for (i = 0; i < n; i++) {
			std::string handle = "";
			std::string filePath = "";
			std::string inType = "";
			std::string outType = "";
			try {
				handle = files[i].at("handle").get<std::string>();
			} catch (...) {
				std::cout
						<< "JSON file object does not have the \"handle\" attribute"
						<< std::endl;
			}
			try {
				filePath = files[i].at("path").get<std::string>();
			} catch (...) {
				std::cout
						<< "JSON file object does not have the \"path\" attribute"
						<< std::endl;
			}
			try {
				inType = files[i].at("inType").get<std::string>();
			} catch (...) {
				std::cout
						<< "JSON file object does not have the \"inType\" attribute"
						<< std::endl;
			}
			try {
				outType = files[i].at("outType").get<std::string>();
			} catch (...) {
				std::cout
						<< "JSON file object does not have the \"outType\" attribute"
						<< std::endl;
			}
			resourceFileObj.addFile(handle, filePath, inType, outType);
		}
		status = 0; //okay
	}
	return status;
}

int Parser::getSize(ResourceFile& directoryObj) {
	unsigned int i, n;
	n = directoryObj.assetListSize();
	for (i = 0; i < n; i++) {
		getSize(directoryObj.asset(i));
	}
	return 0;
}

int Parser::getSize(Asset* assetPtr) {
	nowide::ifstream fileAsset;
	int retStatus = -1;
	try {
		fileAsset.open(assetPtr->getFilePath().string(), std::ios::binary);
	} catch (...) {
		retStatus = 1;
	}
	if (fileAsset.is_open()) {
		assetPtr->setExist(true);
		// std::cout << "open " << assetPtr->getFilePath() << std::endl;
		fileAsset.seekg(0, std::ios::end); // set the pointer to the end
		assetPtr->setFileBytes(fileAsset.tellg()); // get the length of the file
		try {
			fileAsset.close();
		} catch (...) {
			// nothing
		}
	} else {
		assetPtr->setExist(false);
		retStatus = 2;
	}
	return retStatus;
}

int Parser::estimate(Asset* assetPtr) {
	unsigned long long fileSize, processBytes;
	nowide::ifstream fileAsset;
	int retStatus = -1;
	try {
		fileAsset.open(assetPtr->getFilePath().string(), std::ios::binary);
		assetPtr->setExist(true);
	} catch (...) {
		retStatus = 1;
		assetPtr->setExist(false);
	}
	if (fileAsset.is_open()) {
		fileAsset.seekg(0, std::ios::end); // set the pointer to the end
		fileSize = fileAsset.tellg(); // get the length of the file
		fileAsset.seekg(0, std::ios::beg); // set the pointer to the beginning
		processBytes = 0;
		unsigned long long j, buffer;
		uint64_t crcHash;
		buffer = 256;
		uint8_t bufferBytes[buffer];
		j = crcHash = 0;
		while (j < fileSize) {
			if (j + buffer > fileSize) {
				buffer = fileSize - j;
			}
			fileAsset.seekg(j, std::ios::beg);
			fileAsset.read((char*) bufferBytes, buffer);
			crcHash = hashExt::crc64(crcHash, bufferBytes, (uint64_t) buffer);
			// update status
			processBytes += buffer;
			j += buffer;
			assetPtr->setProcessBytes(processBytes);
		}
		assetPtr->setProcessBytes(fileSize);
		assetPtr->setCRC64(crcHash);
	} else {
		retStatus = 2;
	}
	return retStatus;
}

unsigned char* Parser::ullToBytes(unsigned long long val) {
	unsigned long long *temp = new unsigned long long(val);
	return (unsigned char*) temp;
}

char* Parser::ullToBytesSigned(unsigned long long val) {
	unsigned long long *temp = new unsigned long long(val);
	return (char*) (unsigned char*) temp;
}

char* Parser::ullToBytesSigned(unsigned long long val,
		unsigned long long size) {
	char* temp = new char[size];
	memset((void*) temp, 0, size);
	memcpy((void*) temp, static_cast<void*>(&val), size);
	return temp;
}

unsigned long long Parser::bytesToUll(unsigned char* val) {
	unsigned long long *temp = (unsigned long long*) val;
	return *temp;
}

unsigned long long Parser::bytesToUll(char* val) {
	unsigned long long *temp = (unsigned long long*) (unsigned char*) val;
	return *temp;
}

void Parser::ofstreamWrite(nowide::ofstream& outStream, unsigned long long val,
		unsigned long long size) {
	char* temp = new char[size];
	memset((void*) temp, 0, size);
	memcpy((void*) temp, static_cast<void*>(&val), size);
	outStream.write(temp, size);
	delete temp;
}

int Parser::assetListToDirectory(std::vector<Asset*>& assetList,
		Directory& directory) {
	int returnValue = 0;
	unsigned int i, n;
	n = assetList.size();
	for (i = 0; i < n; i++) {
		directory.addFromAsset(*assetList[i]);
	}
	return returnValue;
}

}
