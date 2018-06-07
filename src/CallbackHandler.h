/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <iostream>
#include <string>
#include <fstream>

namespace ResourceFileUtility {

class CallbackHandler {
public:
	CallbackHandler() {

	}
	virtual ~CallbackHandler() {
	}
	virtual int fileComplete(char* filePath);
	virtual int packComplete(char* filePath);
	virtual int estimateFileComplete(int fileID);
};

}

#endif
