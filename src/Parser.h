/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_PARSER_H
#define RFU_PARSER_H

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
#include "ResourceFile.h"

using namespace boost;

namespace ResourceFileUtility {

class Asset;
class Directory;
class ResourceFile;

class Parser {
public:
	Parser() {

	}
	virtual ~Parser() {
	}
	static int readDirectoryJSON(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int readDirectory(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int getSize(ResourceFile& directoryObj);
	static int getSize(Asset* assetPtr);
	static int estimate(Asset* assetPtr);
	static int writeDirectory(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int insertAsset(filesystem::fstream& resourceFile, Asset& assetObj);
	static int removeAsset(filesystem::fstream& resourceFile, Asset& assetObj);
	static unsigned char* ullToBytes(unsigned long long val);
	static char* ullToBytesSigned(unsigned long long val);
	static char* ullToBytesSigned(unsigned long long val,
			unsigned long long size);
	static unsigned long long bytesToUll(unsigned char* val);
	static unsigned long long bytesToUll(char* val);
	static void ofstreamWrite(nowide::ofstream& outStream,
			unsigned long long val, unsigned long long size);
	static int assetListToDirectory(std::vector<Asset*>& assetList,
			Directory& directory);
};

}

#endif
