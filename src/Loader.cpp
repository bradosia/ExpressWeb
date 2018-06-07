#include "Loader.h"

namespace ResourceFileUtility {

Loader::Loader() {

}

unsigned int Loader::data(std::string resourceFileName) {
	return resourceFileObj.open(resourceFileName);
}

Asset* Loader::info(std::string assetHandle) {
	return NULL;
}

unsigned char* Loader::open(std::string assetHandle) {
	unsigned char test[] = "this is some data";
	unsigned char* temp = new unsigned char();
	strcpy((char*) temp, (const char*) test);
	return temp;
}

}
