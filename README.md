# ResourceFileUtility
A select port of node.js and hapi to c++. 

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
## Creating a server

### node.js + hapi.js
```nodejs
const Hapi = require('hapi');
const server = new Hapi.Server();
server.connection({ port: 3000 });
server.start((err) => {
    if (err) {
        throw err;
    }
    console.log(`Server running at: ${server.info.uri}`);
});
```

### c++
```cpp
#include <ExpressWeb.h>

auto server = ExpressWeb::Server;
server.connection({ "port", 3000 });
server.start((err) => {
    if (err) {
        throw err;
    }
    console.log(`Server running at: ${server.info.uri}`);
});
```

## Creating a server

### node.js + hapi.js
```nodejs
const Hapi = require('hapi');

const server = Hapi.server({
    port: 3000,
    host: 'localhost'
});

server.route({
    method: 'GET',
    path: '/',
    handler: (request, h) => {

        return 'Hello, world!';
    }
});

server.route({
    method: 'GET',
    path: '/{name}',
    handler: (request, h) => {

        return 'Hello, ' + encodeURIComponent(request.params.name) + '!';
    }
});

const init = async () => {

    await server.start();
    console.log(`Server running at: ${server.info.uri}`);
};

process.on('unhandledRejection', (err) => {

    console.log(err);
    process.exit(1);
});

init();
```

### c++
```cpp
#include <ExpressWeb.h>

auto server = ExpressWeb::Server({{ "port", 3000 }, { "host", "localhost" }});
server.route({
    {method: 'GET'},
    {path: '/'},
    {handler: [](ExpressWeb::Request& request, ExpressWeb::Response& response) => {
        return "Hello, world!";
    }
});

server.start((err) => {
    if (err) {
        throw err;
    }
    console.log(`Server running at: ${server.info.uri}`);
});
```


## Dependency
RapidJSON<BR>
License: MIT<BR>
https://github.com/Tencent/rapidjson<BR>
<BR>
Boost<BR>
License:  Boost Software License 1.0<BR>
https://www.boost.org<BR>

# License
The library is licensed under the MIT License: <BR>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.