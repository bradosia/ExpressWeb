/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_ASSET_H
#define RFU_ASSET_H

#include <iostream>
#include <string>
#include <chrono>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>

using namespace boost;

namespace ResourceFileUtility {

class Asset {
private:
	bool fileExist, fileWritten, fileProcessing;
	unsigned long long filePosCurrent, filePosNew, fileLenCurrent, fileLenNew,
			processBytes, fileBytes, fileReadBytesLast;
	std::chrono::microseconds processTime, fileReadTimeLast,
			fileReadTimePerByteLast;
	std::string handle, inType, outType;
	filesystem::path filePath;
	uint64_t crc64;
public:
	Asset();
	Asset(std::string handle_, filesystem::path filePath_, std::string inType_,
			std::string outType_);
	void init();
	std::string getHandle();
	filesystem::path getFilePath();
	std::string getInType();
	std::string getOutType();
	void setExist(bool flag);
	void setProcess(bool flag);
	bool getProcess();
	unsigned long long getProcessBytes();
	unsigned long long getFileBytes();
	void setProcessBytes(unsigned long long val);
	void setFileBytes(unsigned long long val);
	void setProcessTime(std::chrono::microseconds val);
	uint64_t getCRC64();
	void setCRC64(uint64_t val);
};

}

#endif
