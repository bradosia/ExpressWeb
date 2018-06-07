/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_RESOURCE_FILE_H
#define RFU_RESOURCE_FILE_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>
#include "../contrib/json.hpp"
#include "crc64.h"
#include "Asset.h"
#include "Parser.h"

using namespace boost;

namespace ResourceFileUtility {

/*
 * @class Directory
 * A pre-processed resource file directory.\n
 * Each Directory entry takes up the bytes
 * 8 bytes = file CRC64
 * 8 bytes = file start position (byte)
 * 8 bytes = file length (byte)
 * 8 bytes = insert time
 * 8 bytes = file type (8 digit ascii)
 * 32 bytes = file handle (32 digit ascii)
 * 58 bytes = custom
 * 128 bytes total
 */
class Directory {
private:
	class Entry {
	public:
		uint64_t CRC64, assetPosition // relative offset from file data start position
				, assetLength, assetInsertTime;
		char type[8], handle[32];
		Asset* assetPtr;
	};
	std::vector<Entry*> entryList;
	std::vector<std::pair<uint64_t,uint64_t>*> spaceList;
	uint64_t spaceLast, offsetPosition;
	unsigned int entryReserve;
public:
	Directory();
	unsigned char* toBytes(unsigned int& size);
	int addFromAsset(Asset& assetObject);
	uint64_t findSpace(uint64_t length);
	void setOffsetPosition(uint64_t pos);
	void setEntryReserve(unsigned int val);
};

/**
 * @class ResourceFile
 * Resource File Meta
 * 8 bytes = file version
 * 8 bytes = file compatibility version
 * 8 bytes = last write time (seconds since unix epoch)
 * 8 bytes = directory start position (byte)
 * 8 bytes = directory length (byte)
 * 8 bytes = directory CRC64
 * 8 bytes = data start position (byte)
 * 8 bytes = data length (byte)
 * 8 bytes = data CRC64
 * 72 bytes = reserved
 * 128 bytes total
 **/
class ResourceFile {
private:
	Directory directory;
	std::vector<Asset*> assetList;
	unsigned long long versionStatic, compatibilityVersionStatic, version,
			compatibilityVersion // equal compatibility versions can be read/written
			, directoryStartByte, DataStartByte;
	std::chrono::time_point<std::chrono::system_clock> writeTimeLast;
	filesystem::path directoryPath;
	std::unordered_map<char*, Asset*> hashTable;
	unsigned int metaSize, directoryEntryReserve, directoryEntrySize;
public:
	ResourceFile();
	virtual ~ResourceFile() {
	}
	void addFile(std::string handle, std::string filePathString,
			std::string inType, std::string outType);
	unsigned int assetListSize();
	Asset* asset(unsigned int assetID);
	unsigned long long getProcessingBytesTotal();
	unsigned long long getProcessingBytes();
	unsigned long long getSizeTotal();
	unsigned long long getVersion();
	unsigned long long getCompatibilityVersion();
	std::string infoToString();
	std::string estimateToString();
	void setDirectory(filesystem::path path);
	unsigned int open(std::string resourceFileName);
	unsigned int open(std::wstring resourceFileName);
	unsigned int open(filesystem::path resourceFilePath);
	unsigned int write(std::string resourceFileName);
	unsigned int write(std::wstring resourceFileName);
	unsigned int write(filesystem::path resourceFilePath);
	int buildDirectory();
};

}

#endif
