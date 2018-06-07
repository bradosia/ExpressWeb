/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <string>
#include "Asset.h"
#include "ResourceFile.h"
#include "Parser.h"

namespace ResourceFileUtility {

class ResourceFile;
class Asset;
class Loader {
private:
	ResourceFile resourceFileObj;
public:
	Loader();
	virtual ~Loader() {
	}
	/*
	 * Opens the resource file and populates the Resource File Object
	 */
	unsigned int data(std::string resourceFileName);
	Asset* info(std::string assetHandle);
	/*
	 * Opens the resource file asset byte array
	 * @return asset byte array
	 */
	unsigned char* open(std::string assetHandle);
};

}

#endif
