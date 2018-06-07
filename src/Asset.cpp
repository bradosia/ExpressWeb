#include "Asset.h"

namespace ResourceFileUtility {

Asset::Asset() {
	init();
}

Asset::Asset(std::string handle_, filesystem::path filePath_,
		std::string inType_, std::string outType_) {
	init();
	handle = handle_;
	filePath = filePath_;
	inType = inType_;
	outType = outType_;
}

void Asset::init() {
	fileExist = fileWritten = fileProcessing = false;
	filePosCurrent = filePosNew = fileLenCurrent = fileLenNew = processBytes =
			fileBytes = fileReadBytesLast = 0;
	processTime = fileReadTimeLast = fileReadTimePerByteLast =
			std::chrono::microseconds(0);
	handle = inType = outType = "";
	crc64 = 0;
}

std::string Asset::getHandle() {
	return handle;
}
filesystem::path Asset::getFilePath() {
	return filePath;
}

std::string Asset::getInType() {
	return inType;
}
std::string Asset::getOutType() {
	return outType;
}

void Asset::setExist(bool flag) {
	fileExist = flag;
}

void Asset::setProcess(bool flag) {
	fileProcessing = flag;
}

bool Asset::getProcess() {
	return fileProcessing;
}

unsigned long long Asset::getProcessBytes() {
	return processBytes;
}
unsigned long long Asset::getFileBytes() {
	return fileBytes;
}
void Asset::setProcessBytes(unsigned long long val) {
	processBytes = val;
}
void Asset::setFileBytes(unsigned long long val) {
	fileBytes = val;
}
void Asset::setProcessTime(std::chrono::microseconds val) {
	processTime = val;
}
uint64_t Asset::getCRC64() {
	return crc64;
}
void Asset::setCRC64(uint64_t val) {
	crc64 = val;
}

}
