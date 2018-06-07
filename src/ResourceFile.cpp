#include "ResourceFile.h"

namespace ResourceFileUtility {

Directory::Directory() {
	spaceLast = offsetPosition = 0;
	entryReserve = 0;
}

unsigned char* Directory::toBytes(unsigned int& size) {
	unsigned int i, n, offset;
	unsigned char* returnValue;
	n = entryReserve;
	if (entryList.size() > n)
		n = entryList.size();
	size = 128 * n;
	returnValue = new unsigned char[size];
	memset(&*returnValue, 0, size);
	for (i = 0; i < n; i++) {
		offset = i * 128;
		// 8 bytes = file CRC64
		memcpy(&returnValue[offset + 0],
				Parser::ullToBytesSigned(entryList[i]->CRC64), 8);
		// 8 bytes = file start position (byte)
		memcpy(&returnValue[offset + 8],
				Parser::ullToBytesSigned(
						entryList[i]->assetPosition + offsetPosition), 8);
		// 8 bytes = file length (byte)
		memcpy(&returnValue[offset + 16],
				Parser::ullToBytesSigned(entryList[i]->assetLength), 8);
		// 8 bytes = insert time
		memcpy(&returnValue[offset + 24],
				Parser::ullToBytesSigned(entryList[i]->assetInsertTime), 8);
		// 8 bytes = file type (8 digit ascii)
		memcpy(&returnValue[offset + 32], entryList[i]->type, 8);
		// 32 bytes = file handle (32 digit ascii)
		memcpy(&returnValue[offset + 40], entryList[i]->handle, 8);
	}
	return &*returnValue;
}

int Directory::addFromAsset(Asset& assetObject) {
	int returnValue = 0;
	Entry* entryTemp = new Entry();
	entryTemp->CRC64 = assetObject.getCRC64();
	entryTemp->assetLength = (uint64_t) assetObject.getFileBytes();
	entryTemp->assetPosition = findSpace(entryTemp->assetLength);
	// the insert time will be re-written when the actual packing occurs
	entryTemp->assetInsertTime =
			(uint64_t) std::chrono::time_point_cast<std::chrono::seconds>(
					std::chrono::system_clock::now()).time_since_epoch().count();
	memcpy(&entryTemp->type, assetObject.getOutType().substr(0, 7).c_str(), 8);
	memcpy(&entryTemp->handle, assetObject.getHandle().substr(0, 31).c_str(),
			32);
	entryTemp->assetPtr = &assetObject;
	entryList.push_back(entryTemp);
	// increment if last space was used
	if (entryTemp->assetPosition == spaceLast) {
		spaceLast += entryTemp->assetLength;
	}
	return returnValue;
}

uint64_t Directory::findSpace(uint64_t length) {
	unsigned int i, n;
	n = spaceList.size();
	uint64_t returnValue = spaceLast;
	if (n > 0) {
		for (i = 0; i < n; i++) {
			// pair->first = space starting position, pair->second = space length
			if (length <= spaceList[i]->second) {
				returnValue = spaceList[i]->first;
				break;
			}
		}
	}
	return returnValue;
}

void Directory::setOffsetPosition(uint64_t pos) {
	offsetPosition = pos;
}

void Directory::setEntryReserve(unsigned int val) {
	entryReserve = val;
}

/* ResourceFile implementation */
ResourceFile::ResourceFile() {
	versionStatic = 1526521021;
	compatibilityVersionStatic = 1526521021;
	version = compatibilityVersion = 0;
	directoryEntryReserve = 0;
	metaSize = directoryEntrySize = 128;
}

void ResourceFile::addFile(std::string handle, std::string filePathStringUTF8,
		std::string inType, std::string outType) {
	unsigned int i, n;
	// boost::nowide being used instead of wide strings
	/*std::wstring filePathStringUTF16 = boost::locale::conv::utf_to_utf<wchar_t,
	 char>(filePathStringUTF8);*/
	assetList.push_back(
			new Asset(handle, filesystem::path(filePathStringUTF8), inType,
					outType));
}
unsigned int ResourceFile::assetListSize() {
	return (unsigned int) assetList.size();
}
Asset* ResourceFile::asset(unsigned int assetID) {
	Asset* assetObj = nullptr;
	try {
		assetObj = assetList[assetID];
	} catch (...) {

	}
	return assetObj;
}

unsigned long long ResourceFile::getProcessingBytesTotal() {
	unsigned int i, n;
	unsigned long long size;
	n = assetList.size();
	size = 0;
	for (i = 0; i < n; i++) {
		if (assetList[i]->getProcess()) {
			size += assetList[i]->getFileBytes();
		}
	}
	return size;
}

unsigned long long ResourceFile::getProcessingBytes() {
	unsigned int i, n;
	unsigned long long size;
	n = assetList.size();
	size = 0;
	for (i = 0; i < n; i++) {
		if (assetList[i]->getProcess()) {
			size += assetList[i]->getProcessBytes();
		}
	}
	return size;
}

unsigned long long ResourceFile::getSizeTotal() {
	unsigned int i, n;
	unsigned long long size;
	n = assetList.size();
	size = 0;
	for (i = 0; i < n; i++) {
		size += assetList[i]->getFileBytes();
	}
	return size;
}

unsigned long long ResourceFile::getVersion() {
	return version;
}
unsigned long long ResourceFile::getCompatibilityVersion() {
	return compatibilityVersion;
}

std::string ResourceFile::infoToString() {
	std::stringstream stringStream;
	stringStream << std::left << std::setw(20) << "Handle" << std::setw(10)
			<< "In Type" << std::setw(10) << "Out Type" << "Path" << std::endl;
	unsigned int i, n;
	n = assetList.size();
	for (i = 0; i < n; i++) {
		stringStream << std::left << std::setw(20) << assetList[i]->getHandle()
				<< std::setw(10) << assetList[i]->getInType() << std::setw(10)
				<< assetList[i]->getOutType()
				<< assetList[i]->getFilePath().string() << std::endl;
	}
	return stringStream.str();
}

std::string ResourceFile::estimateToString() {
	std::stringstream stringStream;
	stringStream << std::left << std::setw(20) << "Handle" << std::setw(10)
			<< "Bytes" << std::setw(32) << "CRC 64" << std::endl;
	unsigned int i, n;
	n = assetList.size();
	for (i = 0; i < n; i++) {
		stringStream << std::left << std::setw(20) << assetList[i]->getHandle()
				<< std::setw(10) << assetList[i]->getFileBytes()
				<< std::setw(32) << assetList[i]->getCRC64() << std::endl;
	}
	return stringStream.str();
}

void ResourceFile::setDirectory(filesystem::path path) {
	directoryPath = path;
}

unsigned int ResourceFile::open(std::string resourceFileName) {
	filesystem::path resourceFilePath = filesystem::path(resourceFileName);
	return open(resourceFilePath);
}
unsigned int ResourceFile::open(std::wstring resourceFileName) {
	filesystem::path resourceFilePath = filesystem::path(resourceFileName);
	return open(resourceFilePath);
}
unsigned int ResourceFile::open(filesystem::path resourceFilePath) {
	int retStatus = -1;
	nowide::ifstream resourceFileStream;
	try {
		resourceFileStream.open(resourceFilePath.string(), std::ios::binary);
	} catch (...) {
		retStatus = 1;
	}
	if (resourceFileStream.is_open()) {
		char bufferBytes[8];
		retStatus = 0;
		// 8 bytes = file version
		resourceFileStream.seekg(0, std::ios::beg); // set the pointer to the beginning
		resourceFileStream.read(bufferBytes, 8);
		version = Parser::bytesToUll(bufferBytes);
		// 8 bytes = file compatibility version
		resourceFileStream.seekg(8, std::ios::beg);
		resourceFileStream.read(bufferBytes, 8);
		compatibilityVersion = Parser::bytesToUll(bufferBytes);
		std::cout << "version = " << version << "\n";
		std::cout << "compatibilityVersion = " << compatibilityVersion << "\n";
		if (compatibilityVersionStatic != compatibilityVersion) {
			retStatus = 2;
		} else {
			// 8 bytes = last write time (seconds since unix epoch)
			resourceFileStream.seekg(16, std::ios::beg);
			resourceFileStream.read(bufferBytes, 8);
			writeTimeLast = std::chrono::time_point<std::chrono::system_clock>(
					std::chrono::seconds(Parser::bytesToUll(bufferBytes)));
			std::cout << "writeTimeLast = "
					<< std::chrono::time_point_cast<std::chrono::seconds>(
							writeTimeLast).time_since_epoch().count() << "\n";
			// 8 bytes = directory start position (byte)
			resourceFileStream.seekg(24, std::ios::beg);
			resourceFileStream.read(bufferBytes, 8);
			directoryStartByte = Parser::bytesToUll(bufferBytes);
			std::cout << "directoryStartByte = " << directoryStartByte << "\n";
		}
	}
	return retStatus;
}

unsigned int ResourceFile::write(std::string resourceFileName) {
	filesystem::path resourceFilePath = filesystem::path(resourceFileName);
	return write(resourceFilePath);
}
unsigned int ResourceFile::write(std::wstring resourceFileName) {
	filesystem::path resourceFilePath = filesystem::path(resourceFileName);
	return write(resourceFilePath);
}
unsigned int ResourceFile::write(filesystem::path resourceFilePath) {
	int retStatus = -1;
	nowide::ofstream resourceFileStream;
	try {
		resourceFileStream.open(resourceFilePath.string(), std::ios::binary);
	} catch (...) {
		retStatus = 1;
	}
	if (resourceFileStream.is_open()) {
		// 8 bytes = file version
		resourceFileStream.seekp(0, std::ios::beg); // set the pointer to the beginning
		Parser::ofstreamWrite(resourceFileStream, versionStatic, 8);
		// 8 bytes = file compatibility version
		resourceFileStream.seekp(8, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, compatibilityVersionStatic,
				8);
		// 8 bytes = last write time (seconds since unix epoch)
		writeTimeLast = std::chrono::system_clock::now();
		unsigned long long t =
				std::chrono::time_point_cast<std::chrono::seconds>(
						writeTimeLast).time_since_epoch().count();
		resourceFileStream.seekp(16, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, t, 8);
		// 8 bytes = directory start position (byte)
		resourceFileStream.seekp(24, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, metaSize, 8);
		// 8 bytes = directory length (byte)
		unsigned int directoryBytesSize = directoryEntryReserve
				* directoryEntrySize;
		unsigned int dataPositionStart = metaSize + directoryBytesSize;
		directory.setOffsetPosition(dataPositionStart);
		directory.setEntryReserve(directoryEntryReserve);
		unsigned char* directoryBytes = directory.toBytes(directoryBytesSize);
		resourceFileStream.seekp(32, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, directoryBytesSize, 8);
		// 8 bytes = directory CRC64
		uint64_t directoryCrcHash = hashExt::crc64(0, directoryBytes,
				(uint64_t) directoryBytesSize);
		resourceFileStream.seekp(40, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, directoryCrcHash, 8);
		// 8 bytes = data start position (byte)
		resourceFileStream.seekp(48, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, dataPositionStart, 8);
		// 8 bytes = data length (byte)
		resourceFileStream.seekp(56, std::ios::beg);
		Parser::ofstreamWrite(resourceFileStream, 50000, 8);
		// write directory
		resourceFileStream.seekp(metaSize, std::ios::beg);
		resourceFileStream.write((char*) directoryBytes, directoryBytesSize);
		// write data
		resourceFileStream.seekp(dataPositionStart, std::ios::beg);
		unsigned char test[50000] = "Mauris vel consectetur diam, sit amet bibendum nisi. In commodo, tortor id tempor auctor, purus tellus facilisis ex, vel placerat mi sem eget arcu. Proin nec dolor consectetur, lacinia turpis a, ullamcorper ligula. Mauris pulvinar enim magna, nec dapibus urna efficitur id. Aenean nec lorem nunc. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus viverra felis vel urna ullamcorper, vitae placerat lorem viverra. Duis sollicitudin et enim iaculis pharetra. Maecenas ligula orci, volutpat sed dictum at, dapibus sit amet purus. Proin eros magna, ultricies ut gravida in, laoreet vitae neque.";
		resourceFileStream.write((char*) test, 50000);
	}
	return retStatus;
}

int ResourceFile::buildDirectory() {
	return Parser::assetListToDirectory(assetList, directory);
}

}
