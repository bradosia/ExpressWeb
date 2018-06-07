# ResourceFileUtility
A utility for compiling media assets into a single resource file. Includes a driver for c#.

# Table of Contents

<!--ts-->
   * [Build Instructions](#build-instructions)
   * [Supported](#supported)
   * [Usage Instructions](#usage-instructions)
      * [C++](#c)
         * [Compile Resources](#compile-resources)
         * [Load Resources](#load-resources)
         * [Stream Resources](#stream-resources)
      * [C#](#c-1)
         * [Compile Resources](#compile-resources-1)
         * [Load Resources](#load-resources-1)
         * [Stream Resources](#stream-resources-1)
   * [Dependency](#dependency)
   * [License](#license)
<!--te-->

# Build Instructions

## Prerequisites
Install boost libraries<BR>
You can manually edit the makefile or make with boost include and library paths like so:
```sh
make OS_DET=WIN TARGET_ARCH=x86_64 BOOST_INCLUDE_DIR=C:/boost/include/boost-1_67 BOOST_LIBS_DIR=C:/boost/lib-mgw BOOST_LIBS_POST=-mgw73-mt-x64-1_67
```

## Windows
Only Windows host<BR>
Install MinGW-w64<BR>
Add ```<MinGW_PATH>\bin``` to PATH<BR>

## Mac OSX
Only Mac OSX host<BR>
Install clang

## Linux Ubuntu
Install GCC
For C language:
```shell
sudo apt-get install gcc-multilib
```
For C++ language:
```shell
sudo apt-get install g++-multilib
```

## Android
Using Windows x86_64 host for this example:<BR>
Install the android NDK<BR>
Add ```<NDK_PATH>\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
Add ```<NDK_PATH>\toolchains\aarch64-linux-android-4.9\prebuilt\windows-x86_64\bin``` to PATH<BR>
You can manually edit the makefile or make with android ndk path like so:
```sh
make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86_64 NDK_PATH=C:\android-ndk-r16b-windows-x86_64\android-ndk-r16b
```

## IOS
Only Mac OSX host<BR>
Install clang<BR>
You can manually edit the makefile or make with iphone sdk path like so:
```sh
make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=x86_64 IPHONE_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
```

## Supported
| Host OS | Target OS | Architecture | Support |  Command |
| :---    | :---      | :---         | :---    | :---     |
| Windows | Windows   | x86          | X    | ``` make HOST_OS=WIN TARGET_ARCH=x86 ``` |
| Windows | Windows   | x86_64       | X    | ``` make HOST_OS=WIN TARGET_ARCH=x86_64 ``` |
| Windows | Android   | armeabi      | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=armeabi ``` |
| Windows | Android   | armeabi-v7a  | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=armeabi-v7a ``` |
| Windows | Android   | arm64-v8a    | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=arm64-v8a ``` |
| Windows | Android   | x86          | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86 ``` |
| Windows | Android   | x86_64       | O    | ``` make HOST_OS=WIN TARGET_OS=android TARGET_ARCH=x86_64 ``` |
| macOS   | macOS     | x86          | O    | ``` make HOST_OS=OSX TARGET_ARCH=x86 ``` |
| macOS   | macOS     | x86_64       | X    | ``` make HOST_OS=OSX TARGET_ARCH=x86_64 ``` |
| macOS   | iOS       | armv7        | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=armv7 ``` |
| macOS   | iOS       | armv7s       | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=armv7s ``` |
| macOS   | iOS       | arm64        | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=arm64 ``` |
| macOS   | iOS       | x86_64       | O    | ``` make HOST_OS=OSX TARGET_OS=IOS TARGET_ARCH=x86_64 ``` |
| linux ubuntu | linux ubuntu | x86     | O    | ``` make HOST_OS=LINUX TARGET_ARCH=x86 ``` |
| linux ubuntu | linux ubuntu | x86_64  | O    | ``` make HOST_OS=LINUX TARGET_ARCH=x86_64 ``` |
 
# Usage Instructions
## C++
there is only a single include needed when statically linking against this library
```cpp
#include "include/ResourceFileUtility.h"
```

### Compile Resources
Pack all resources in your json file to a single resource file
```cpp
ResourceFileUtility::Compiler RFUCompiler = new ResourceFileUtility::Compiler();
RFUCompiler.info("resources.json");
RFUCompiler.pack("assets.data");
```

### Load Resources
Load a resource to memory and handle it
```cpp
ResourceFileUtility::Loader RFULoader = new ResourceFileUtility::Loader();
ResourceFileUtility::Info coneInfo = RFULoader.info("cone");
char modelBytes = RFULoader.open("cone"); //loads all cone data to memory
if(coneInfo.inType == "FILE_FORMAT"){
	// handle FILE_FORMAT
}
```

### Stream Resources
Stream a resource
```cpp
ResourceFileUtility::Loader RFULoader = new ResourceFileUtility::Loader();
ResourceFileUtility::Info songInfo = RFULoader.info("song");
ResourceFileUtility::Stream songStream = RFULoader.stream("song"); // stream data
songStream.pos(0); // set stream position to beginning of file
if(songInfo.inType == "FILE_FORMAT"){
	// handle FILE_FORMAT
	// get 4 bytes from selector, selector increments by 4
	char modelBytes4 = songStream.get(4); // first 4 bytes
	modelBytes4 = songStream.get(4); // next 4 bytes
}
```

### Compile Resources With Progress
Pack all resources in your json file to a single resource file
```cpp
unsigned fileNumber, sizeTotal, sizeCurrent, loadBarNumber;
ResourceFileUtility::Compiler RFUCompiler = new ResourceFileUtility::Compiler();
RFUCompiler.info("resources.json");
ResourceFileUtility::FileList fileList = RFUCompiler.getFileList();
fileNumber = fileList.size();
RFUCompiler.estimate();
sizeTotal = RFUCompiler.sizePending();
while (true) {
	sizeCurrent = 0;
	for (int i = 0; i < fileNumber; i++) {
		sizeCurrent += fileList[i]->processSize();
	}
	loadBarNumber = (unsigned int) (sizeCurrent / (double) sizeTotal * 20.0);
	cout << std::string(loadBarNumber, '#') << std::string(20 - loadBarNumber, ' ') << " Progress\r";
	if (sizeCurrent >= sizeTotal)
		break;
}
RFUCompiler.pack("assets.data");
sizeTotal = RFUCompiler.sizePending();
while (true) {
	sizeCurrent = 0;
	for (int i = 0; i < fileNumber; i++) {
		sizeCurrent += fileList[i]->processSize();
	}
	loadBarNumber = (unsigned int) (sizeCurrent / (double) sizeTotal * 20.0);
	cout << std::string(loadBarNumber, '#') << std::string(20 - loadBarNumber, ' ') << " Progress\r";
	if (sizeCurrent >= sizeTotal)
		break;
}
```


## C#
add ```include/ResourceFileUtility.cs``` to your project<BR><BR>

### Compile Resources
Pack all resources in your json file to a single resource file
```csharp
ResourceFileUtility.Compiler RFUCompiler = new ResourceFileUtility.Compiler();
RFUCompiler.info("resources.json");
RFUCompiler.pack("assets.data");
```

### Load Resources
Load a resource to memory and handle it
```csharp
ResourceFileUtility.Loader RFULoader = new ResourceFileUtility.Loader();
ResourceFileUtility.Info coneInfo = RFULoader.info("cone");
char modelBytes = RFULoader.open("cone"); //loads all cone data to memory
if(coneInfo.inType == "FILE_FORMAT"){
	// handle FILE_FORMAT
}
```

### Stream Resources
Stream a resource
```csharp
ResourceFileUtility.Loader RFULoader = new ResourceFileUtility.Loader();
ResourceFileUtility.Info songInfo = RFULoader.info("song");
ResourceFileUtility.Stream songStream = RFULoader.stream("song"); // stream data
songStream.pos(0); // set stream position to beginning of file
if(songInfo.inType == "FILE_FORMAT"){
	// handle FILE_FORMAT
	// get 4 bytes from selector, selector increments by 4
	char modelBytes4 = songStream.get(4); // first 4 bytes
	modelBytes4 = songStream.get(4); // next 4 bytes
}
```

## Dependency
JSON for C++<BR>
License: MIT<BR>
https://github.com/nlohmann/json<BR>
<BR>
Boost<BR>
License:  Boost Software License 1.0<BR>
https://www.boost.org<BR>

# License
The library is licensed under the MIT License: <BR>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.