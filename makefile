# Architecture detection
ifndef HOST_OS
	ifeq ($(OS),Windows_NT)
	HOST_OS = WIN
		ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
			TARGET_ARCH = x86_64
		else
			ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
             	TARGET_ARCH = x86_64
			endif
			ifeq ($(PROCESSOR_ARCHITECTURE),x86)
             	TARGET_ARCH = x86
			endif
		endif
	else
    	UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Linux)
			HOST_OS = LINUX
		endif
		ifeq ($(UNAME_S),Darwin)
			HOST_OS = OSX
		endif
    	UNAME_P := $(shell uname -p)
		ifeq ($(UNAME_P),x86_64)
			TARGET_ARCH = x86_64
		endif
		ifneq ($(filter %86,$(UNAME_P)),)
			TARGET_ARCH = x86
		endif
    	ifneq ($(filter arm%,$(UNAME_P)),)
			TARGET_ARCH = ARM
    	endif
	endif
endif

#default host options
ifeq ($(HOST_OS),WIN)
	ifndef COMPILER
		COMPILER = MINGW
	endif
	ifndef TARGET_OS
		TARGET_OS = WIN
	endif
endif
ifeq ($(HOST_OS),OSX)
	ifndef COMPILER
		COMPILER = CLANG
	endif
	ifndef TARGET_OS
		TARGET_OS = OSX
	endif
endif
ifeq ($(HOST_OS),LINUX)
	ifndef COMPILER
		COMPILER = GCC
	endif
	ifndef TARGET_OS
		TARGET_OS = LINUX
	endif
endif

# CUSTOM PATHS START
# CUSTOM PATHS START
# CUSTOM PATHS START
# these paths are custom to your installation
ifeq ($(HOST_OS),WIN)
	ifeq ($(COMPILER),MINGW)
		BOOST_INCLUDE_DIR = C:/boost_1_67_0/include
		BOOST_LIBS_DIR = C:/boost_1_67_0/lib-mgw
		ifeq ($(TARGET_ARCH),x86)
			BOOST_LIBS_POST = -mgw73-mt-x32-1_67
			GCC = x86_64-w64-mingw32-g++
			AR = llvm-ar
		endif
		ifeq ($(TARGET_ARCH),x86_64)
			BOOST_LIBS_POST = -mgw73-mt-x64-1_67
			GCC = g++
			AR = ar
		endif
	endif
	ifeq ($(COMPILER),MSVC)
		BOOST_INCLUDE_DIR = C:/boost_1_67_0/include
		BOOST_LIBS_DIR = C:/boost_1_67_0/lib-msvc
		ifeq ($(TARGET_ARCH),x86)
			BOOST_LIBS_POST = -vc141-mt-x32-1_67
			GCC = cl
			AR = lib
		endif
		ifeq ($(TARGET_ARCH),x86_64)
			BOOST_LIBS_POST = -vc141-mt-x64-1_67
			GCC = cl
			AR = lib
		endif
	endif
	ifeq ($(TARGET_OS),ANDROID)
		BOOST_INCLUDE_DIR = C:/boost_1_64_0/include
		BOOST_LIBS_POST = 
		NDK_PATH = C:/android-ndk-r16b-windows-x86_64/ndroid-ndk-r16b
		ifeq ($(TARGET_ARCH),armeabi)
			BOOST_LIBS_DIR = C:/boost_1_64_0/armeabi/lib
			GCC = arm-linux-androideabi-g++
			AR = arm-linux-androideabi-ar
		endif
		ifeq ($(TARGET_ARCH),armeabi-v7a)
			BOOST_LIBS_DIR = C:/boost_1_64_0/armeabi-v7a/lib
			GCC = arm-linux-androideabi-g++
			AR = arm-linux-androideabi-ar
		endif
		ifeq ($(TARGET_ARCH),arm64-v8a)
			BOOST_LIBS_DIR = 
			GCC = aarch64-linux-android-g++
			AR = aarch64-linux-android-ar
		endif
		ifeq ($(TARGET_ARCH),x86)
			BOOST_LIBS_DIR = C:/boost_1_64_0/x86/lib
			GCC = i686-linux-android-g++
			AR = i686-linux-android-ar
		endif
	endif
endif
ifeq ($(HOST_OS),OSX)
	ifeq ($(COMPILER),CLANG)
		BOOST_INCLUDE_DIR = /usr/include
		BOOST_LIBS_DIR = /usr/lib
		BOOST_LIBS_POST = 
		ifeq ($(TARGET_ARCH),x86)
			GCC = clang++
			AR = ar
		endif
		ifeq ($(TARGET_ARCH),x86_64)
			GCC = clang++
			AR = ar
		endif
	endif
	ifeq ($(TARGET_OS),IOS)
		BOOST_INCLUDE_DIR = /usr/include
		BOOST_LIBS_DIR = /usr/lib-ios
		BOOST_LIBS_POST = 
		IPHONE_SDK_PATH = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
	endif
endif
ifeq ($(HOST_OS),LINUX)
	ifeq ($(COMPILER),GCC)
		BOOST_INCLUDE_DIR = /usr/local/include
		BOOST_LIBS_DIR = /usr/local/lib
		BOOST_LIBS_POST =
		GCC = g++
		AR = ar 
	endif
endif
# CUSTOM PATHS END
# CUSTOM PATHS END
# CUSTOM PATHS END

# target OS names
ifeq ($(TARGET_OS),WIN)
	TARGET_OS_NAME = win
endif
ifeq ($(TARGET_OS),android)
	TARGET_OS_NAME = android
endif
ifeq ($(TARGET_OS),OSX)
	TARGET_OS_NAME = osx
endif
ifeq ($(TARGET_OS),IOS)
	TARGET_OS_NAME = ios
endif
ifeq ($(TARGET_OS),LINUX)
	TARGET_OS_NAME = linux
endif

#paths
PROGRAM_BIN_DIR = example
PROGRAM_SHARED_LIB_DIR = example
PROGRAM_OBJ_DIR = example/obj
PROGRAM_SRC_DIR = example/src
PROGRAM_CSHARP_EXE_ENABLE = 
PROGRAM_CSHARP_EXE = $(PROGRAM_BIN_DIR)/example_csharp.exe
PROGRAM_CSHARP_BUNDLE = $(PROGRAM_BIN_DIR)/example_csharp
PROGRAM_CPP_EXE = $(PROGRAM_BIN_DIR)/example_cpp.exe
# mac app
PROGRAM_CPP_APP_NAME = example_cpp
PROGRAM_CPP_APP = $(PROGRAM_BIN_DIR)/example_cpp.app
PROGRAM_CPP_APP_ENABLE = 
PROGRAM_CPP_APP_DEL_CMD = 
# static and dynamic library
LIBRARY_INCLUDE_DIR = include
LIBRARY_CONTRIB_DIR = contrib
LIBRARY_DIR = bin
LIBRARY_SRC_DIR = src
LIBRARY_CPP_NAME = ResourceFileUtility
LIBRARY_CPP_HEADER = $(LIBRARY_INCLUDE_DIR)/$(LIBRARY_CPP_NAME).h
LIBRARY_CPP_ONE_HEADER = $(LIBRARY_SRC_DIR)/OneHeader.h
LIBRARY_CPP_VERSION = gnu++11
LIBRARY_SHARED_LINK_INCLUDES = -lboost_filesystem$(BOOST_LIBS_POST) -lboost_thread$(BOOST_LIBS_POST) -lboost_locale$(BOOST_LIBS_POST) -lboost_system$(BOOST_LIBS_POST) -lboost_iostreams$(BOOST_LIBS_POST)
# OneHeader c++ program
PROGRAM_ONE_HEADER_SRC = contrib/OneHeader/src
PROGRAM_ONE_HEADER_OBJ = contrib/OneHeader/obj
PROGRAM_ONE_HEADER_BIN = contrib/OneHeader/bin
PROGRAM_ONE_HEADER_EXE_ENABLE = 
PROGRAM_ONE_HEADER_EXE = $(PROGRAM_ONE_HEADER_BIN)/OneHeader.exe
PROGRAM_ONE_HEADER_MAIN_NAME = main
# example c++ program
ALL_CPP_COMPILE_INCLUDES = -I"$(LIBRARY_CONTRIB_DIR)" -I"$(LIBRARY_INCLUDE_DIR)"
PROGRAM_EXAMPLE_CPP_LIBS_FLAG = -lResourceFileUtility $(LIBRARY_SHARED_LINK_INCLUDES)

#compiler flags
ifeq ($(COMPILER),CLANG)
	COMPILER_NAME = clang
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
endif
ifeq ($(COMPILER),MINGW)
	COMPILER_NAME = mingw
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
endif
ifeq ($(COMPILER),MSVC)
	COMPILER_NAME = msvc
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
endif
ifeq ($(COMPILER),GCC)
	COMPILER_NAME = gcc
	ifeq ($(TARGET_ARCH),x86)
		LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -m32 -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
		EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	endif
	ifeq ($(TARGET_ARCH),x86_64)
		LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
		EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	endif
endif

# target OS setup
ifeq ($(TARGET_OS),WIN)
	LIBRARY_CPP_SHARED_NAME = $(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
endif
ifeq ($(TARGET_OS),android)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
endif
ifeq ($(TARGET_OS),OSX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
endif
ifeq ($(TARGET_OS),IOS)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
endif
ifeq ($(TARGET_OS),LINUX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
endif
VERSION_NAME = $(TARGET_OS_NAME)-$(TARGET_ARCH)-$(COMPILER_NAME)
# paths
LIBRARY_PLATFORM_DIR = $(LIBRARY_DIR)/$(VERSION_NAME)
LIBRARY_CPP_SHARED_BIN = $(LIBRARY_PLATFORM_DIR)/$(LIBRARY_CPP_SHARED_NAME)
LIBRARY_CPP_STATIC_BIN = $(LIBRARY_PLATFORM_DIR)/$(LIBRARY_CPP_STATIC_NAME)
LIBRARY_SRC_DIR = src
LIBRARY_TEMP_DIR = $(VERSION_NAME)
LIBRARY_OBJ_DIR = $(LIBRARY_TEMP_DIR)/src

# host setup
ifeq ($(HOST_OS),WIN)
	HOST_SHELL = WIN
endif
ifeq ($(HOST_OS),OSX)
	HOST_SHELL = UNIX
endif
ifeq ($(HOST_OS),LINUX)
	HOST_SHELL = UNIX
endif

# target OS setup
ifeq ($(TARGET_OS),WIN)
	# cpp library commands and flags
	LIBRARY_OBJ_COMPILE_FLAGS = -I"$(BOOST_INCLUDE_DIR)" $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD)
	LIBRARY_SHARED_LINK_FLAGS = -L"$(BOOST_LIBS_DIR)" -lstdc++ -shared
	LIBRARY_STATIC_LINK_FLAGS = 
	# program c#
	PROGRAM_CSHARP_EXE_ENABLE = $(PROGRAM_CSHARP_EXE)
	CSC = csc
	CSC_FLAGS = /nologo /optimize /langversion:latest
	BUNDLE_CMD = 
	# program c++
	PROGRAM_CPP_COMPILE = -I"$(BOOST_INCLUDE_DIR)" $(EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD)
	PROGRAM_CPP_LINK = -static-libgcc -static-libstdc++ -L"$(LIBRARY_PLATFORM_DIR)" -L"$(BOOST_LIBS_DIR)"
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = -O3 -g3 -std=gnu++11 -Wall -fmessage-length=0
	PROGRAM_ONE_HEADER_LINK_FLAGS = -static-libgcc -static-libstdc++
endif
ifeq ($(TARGET_OS),android)
	ifeq ($(TARGET_ARCH),armeabi-v7a)
		ARCH_OBJ_COMPILE_INCLUDES =  -I"$(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include" \
			-I"$(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.9/include" \
			-I"$(NDK_PATH)/sysroot/usr/include" \
			-I"$(NDK_PATH)/sysroot/usr/include/arm-linux-androideabi"
		NDK_SYSROOT_PATH = $(NDK_PATH)/platforms/android-21/arch-arm
	endif
	ifeq ($(TARGET_ARCH),arm64-v8a)
		ARCH_OBJ_COMPILE_INCLUDES =  -I"$(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v8a/include" \
			-I"$(NDK_PATH)/sources/cxx-stl/gnu-libstdc++/4.9/include" \
			-I"$(NDK_PATH)/sysroot/usr/include" \
			-I"$(NDK_PATH)/sysroot/usr/include/arm-linux-androideabi"
		NDK_SYSROOT_PATH = $(NDK_PATH)/platforms/android-21/arch-arm64
	endif
	# cpp library commands and flags
	LIBRARY_OBJ_COMPILE_FLAGS = $(ALL_CPP_COMPILE_INCLUDES) $(ARCH_OBJ_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall -c -fmessage-length=0 
	LIBRARY_SHARED_LINK_FLAGS = --sysroot="$(NDK_SYSROOT_PATH)" -static-libgcc -static-libstdc++ -static -shared
	LIBRARY_STATIC_LINK_FLAGS = 
	# program c#
	CSC = csc
	CSC_FLAGS = /nologo /optimize /langversion:latest
	BUNDLE_CMD = 
	# program c++
	PROGRAM_CPP_COMPILE = $(ALL_CPP_COMPILE_INCLUDES) $(ARCH_OBJ_COMPILE_INCLUDES) -I"$(LIBRARY_INCLUDE_DIR)" -I"$(NDK_PATH)/sysroot/usr/include" -O3 -g3 -std=gnu++11 -Wall -c -fmessage-length=0
	PROGRAM_CPP_LINK = --sysroot="$(NDK_SYSROOT_PATH)" -static-libgcc -static-libstdc++ -static -L"$(LIBRARY_PLATFORM_DIR)"
endif
ifeq ($(TARGET_OS),LINUX)
	# cpp library commands and flags
	LIBRARY_OBJ_COMPILE_FLAGS = -I"$(BOOST_INCLUDE_DIR)" -fPIC $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD)
	LIBRARY_SHARED_LINK_FLAGS = -L"$(BOOST_LIBS_DIR)" -fPIC -lstdc++ -shared $(LIBRARY_SHARED_LINK_INCLUDES)
	LIBRARY_STATIC_LINK_FLAGS = 
	# program c#
	PROGRAM_CSHARP_EXE_ENABLE = $(PROGRAM_CSHARP_EXE)
	CSC = csc
	CSC_FLAGS = /nologo /optimize /langversion:latest
	BUNDLE_CMD = 
	# program c++
	PROGRAM_CPP_COMPILE = -I"$(BOOST_INCLUDE_DIR)" $(EXAMPLE_PROGRAM_OBJ_COMPILE_FLAGS_STANDARD)
	PROGRAM_CPP_LINK = -static-libgcc -static-libstdc++ -static -pthread -L"$(LIBRARY_PLATFORM_DIR)" -L"$(BOOST_LIBS_DIR)" $(PROGRAM_EXAMPLE_CPP_LIBS_FLAG)
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = -O3 -g3 -std=gnu++11 -Wall -fmessage-length=0
	PROGRAM_ONE_HEADER_LINK_FLAGS = -static-libgcc -static-libstdc++ -static -pthread
endif
ifeq ($(TARGET_OS),OSX)
	# cpp library commands and flags
	GCC = g++
	LIBRARY_OBJ_COMPILE_FLAGS = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -fPIC -std=$(LIBRARY_CPP_VERSION) -Wall -fvisibility=hidden -c -fmessage-length=0 -mmacosx-version-min=10.9
	LIBRARY_SHARED_LINK_FLAGS = -dynamiclib -fPIC -std=gnu++11 -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -mmacosx-version-min=10.9
	AR = ar
	LIBRARY_STATIC_LINK_FLAGS = 
	# program c#
	CSC = csc
	CSC_FLAGS = /nologo /optimize /langversion:latest /lib:example/src
	BUNDLE_CMD = mkbundle -o $(PROGRAM_CSHARP_BUNDLE) --simple $(PROGRAM_CSHARP_EXE) --library $(LIBRARY_CPP_SHARED_BIN)
	# program c++
	PROGRAM_CPP_COMPILE = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=gnu++11 -Wall -c -fmessage-length=0 -mmacosx-version-min=10.9
	PROGRAM_CPP_LINK = -L"$(LIBRARY_PLATFORM_DIR)" -mmacosx-version-min=10.9
	# mac app
	PROGRAM_CPP_APP_ENABLE = $(PROGRAM_CPP_APP)
endif
ifeq ($(TARGET_OS),IOS)
	ifeq ($(TARGET_ARCH),armv7)
		IPHONE_SDK_LIBS_DIR = lib/ios
		IOS_ARCH = armv7
	endif
	ifeq ($(TARGET_ARCH),armv7s)
		IPHONE_SDK_LIBS_DIR = lib/ios
		IOS_ARCH = armv7s
	endif
	ifeq ($(TARGET_ARCH),arm64)
		VERSION_NAME = ios-arm64
		IPHONE_SDK_LIBS_DIR = lib/ios
		IOS_ARCH = arm64
	endif
	# cpp library commands and flags
	GCC = clang++
	ALL_CPP_COMPILE_INCLUDES =  
	LIBRARY_OBJ_COMPILE_FLAGS = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -stdlib=libc++ -Wall -c -fmessage-length=0 \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	LIBRARY_SHARED_LINK_FLAGS = -std=gnu++11 -stdlib=libc++ -static -shared -undefined dynamic_lookup -arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	LIBRARY_STATIC_LINK_FLAGS =
	# program c#
	CSC = csc
	CSC_FLAGS = /nologo /optimize /langversion:latest
	BUNDLE_CMD = 
	# program c++
	PROGRAM_CPP_COMPILE_INCLUDES = 
	PROGRAM_CPP_COMPILE = $(ALL_CPP_COMPILE_INCLUDES) -O3 -g3 -std=gnu++11 -stdlib=libc++ -Wall -c -fmessage-length=0 \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	PROGRAM_CPP_LINK = -std=gnu++11 -stdlib=libc++ -L"$(LIBRARY_PLATFORM_DIR)" -L"$(IPHONE_SDK_LIBS_DIR)" -undefined dynamic_lookup \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
endif

#shell commands
ifeq ($(HOST_SHELL),WIN)
	# backslash
	LIBRARY_OBJ_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_OBJ_DIR))
	LIBRARY_TEMP_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_TEMP_DIR))
	LIBRARY_PLATFORM_DIR_BACKSLASH= $(subst /,\\,$(LIBRARY_PLATFORM_DIR))
	LIBRARY_CPP_SHARED_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_SHARED_BIN))
	LIBRARY_CPP_STATIC_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_STATIC_BIN))
	PROGRAM_CSHARP_EXE_BACKSLASH= $(subst /,\\,$(PROGRAM_CSHARP_EXE))
	PROGRAM_CSHARP_BUNDLE_BACKSLASH= $(subst /,\\,$(PROGRAM_CSHARP_BUNDLE))
	PROGRAM_CPP_EXE_BACKSLASH=$(subst /,\\,$(PROGRAM_CPP_EXE))
	PROGRAM_OBJ_DIR_BACKSLASH=$(subst /,\\,$(PROGRAM_OBJ_DIR))
	LIBRARY_CPP_HEADER_BACKSLASH=$(subst /,\\,$(LIBRARY_CPP_HEADER))
	PROGRAM_ONE_HEADER_EXE_BACKSLASH=$(subst /,\\,$(PROGRAM_ONE_HEADER_EXE))
	# commands
	LIBRARY_OBJ_DIR_CMD = if not exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" mkdir $(LIBRARY_OBJ_DIR_BACKSLASH)
	LIBRARY_PLATFORM_DIR_CMD = if not exist "$(LIBRARY_PLATFORM_DIR_BACKSLASH)" mkdir $(LIBRARY_PLATFORM_DIR_BACKSLASH)
	PROGRAM_OBJ_DIR_CMD = if not exist "$(PROGRAM_OBJ_DIR_BACKSLASH)" mkdir $(PROGRAM_OBJ_DIR_BACKSLASH)
	COPY_SHARED_LIBRARY_CMD = copy $(LIBRARY_CPP_SHARED_BIN_BACKSLASH) $(PROGRAM_BIN_DIR) /Y
	LIBRARY_TEMP_DIR_DEL = if exist "$(LIBRARY_TEMP_DIR_BACKSLASH)" rd /s /q "$(LIBRARY_TEMP_DIR_BACKSLASH)"
	PROGRAM_OBJ_DIR_DEL = if exist "$(PROGRAM_OBJ_DIR_BACKSLASH)" rd /s /q "$(PROGRAM_OBJ_DIR_BACKSLASH)"
	SHARED_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)"
	STATIC_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)"
	PROGRAM_CSHARP_DEL_CMD = if exist "$(PROGRAM_CSHARP_EXE_BACKSLASH)" del /F /Q "$(PROGRAM_CSHARP_EXE_BACKSLASH)"
	PROGRAM_CSHARP_DEL_POST_CMD = 
	BUNDLE_CSHARP_DEL_CMD = if exist "$(PROGRAM_CSHARP_BUNDLE_BACKSLASH)" del /F /Q "$(PROGRAM_CSHARP_BUNDLE_BACKSLASH)"
	PROGRAM_CPP_DEL_CMD = if exist "$(PROGRAM_CPP_EXE_BACKSLASH)" del /F /Q "$(PROGRAM_CPP_EXE_BACKSLASH)"
	#SHARED_LIBRARY_HEADER_CMD = type $(LIBRARY_SRC_DIR)\mingw_threads.h >> $(LIBRARY_CPP_HEADER_BACKSLASH) 2>NUL & \
	#	type $(LIBRARY_SRC_DIR)\ResourceFile.h >> $(LIBRARY_CPP_HEADER_BACKSLASH) 2>NUL & \
	#	type $(LIBRARY_SRC_DIR)\*.h >> $(LIBRARY_CPP_HEADER_BACKSLASH) 2>NUL
	SHARED_LIBRARY_HEADER_DEL_CMD = if exist "$(LIBRARY_CPP_HEADER_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_HEADER_BACKSLASH)"
	PROGRAM_ONE_HEADER_EXE_DEL_CMD = if exist "$(PROGRAM_ONE_HEADER_EXE_BACKSLASH)" del /F /Q "$(PROGRAM_ONE_HEADER_EXE_BACKSLASH)"
endif
ifeq ($(HOST_SHELL),UNIX)
	# commands
	LIBRARY_OBJ_DIR_CMD = mkdir -p $(LIBRARY_OBJ_DIR)
	LIBRARY_PLATFORM_DIR_CMD = mkdir -p $(LIBRARY_PLATFORM_DIR)
	PROGRAM_OBJ_DIR_CMD = mkdir -p $(PROGRAM_OBJ_DIR)
	COPY_SHARED_LIBRARY_CMD_UNUSED = yes | cp -rf $(LIBRARY_CPP_SHARED_BIN) $(PROGRAM_BIN_DIR)
	COPY_SHARED_LIBRARY_CMD = 
	LIBRARY_TEMP_DIR_DEL = yes | rm -rf $(LIBRARY_TEMP_DIR)
	PROGRAM_OBJ_DIR_DEL = yes | rm -rf $(PROGRAM_OBJ_DIR)
	SHARED_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_SHARED_BIN)"
	STATIC_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_STATIC_BIN)"
	PROGRAM_CSHARP_DEL_CMD = yes | rm -f "$(PROGRAM_CSHARP_EXE)"
	PROGRAM_CSHARP_DEL_POST_CMD = $(PROGRAM_CSHARP_DEL_CMD)
	BUNDLE_CSHARP_DEL_CMD = yes | rm -f "$(PROGRAM_CSHARP_BUNDLE)"
	PROGRAM_CPP_DEL_CMD = yes | rm -f "$(PROGRAM_CPP_EXE)"
	PROGRAM_CPP_APP_DEL_CMD = yes | rm -rf "$(PROGRAM_CPP_APP)"
	#SHARED_LIBRARY_HEADER_CMD = cat $(LIBRARY_SRC_DIR)/*.h >> $(LIBRARY_CPP_HEADER)
	SHARED_LIBRARY_HEADER_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_HEADER)"
	PROGRAM_ONE_HEADER_EXE_DEL_CMD = yes | rm -f "$(PROGRAM_ONE_HEADER_EXE)"
endif

LIBRARY_SRC_FILES := $(wildcard $(LIBRARY_SRC_DIR)/*.cpp)
LIBRARY_OBJ_FILES := $(patsubst $(LIBRARY_SRC_DIR)/%.cpp,$(LIBRARY_OBJ_DIR)/%.o,$(LIBRARY_SRC_FILES))
PROGRAM_SRC_FILES := $(wildcard $(PROGRAM_SRC_DIR)/*.cpp)
PROGRAM_OBJ_FILES := $(patsubst $(PROGRAM_SRC_DIR)/%.cpp,$(PROGRAM_OBJ_DIR)/%.o,$(PROGRAM_SRC_FILES))

EXAMPLE_SRC = *.cs
EXTRA_SRC = ResourceFileUtility.cs

all: one_header_setup $(PROGRAM_ONE_HEADER_EXE) one_header library_setup $(LIBRARY_CPP_STATIC_BIN) $(LIBRARY_CPP_SHARED_BIN) library_clean program_cpp $(PROGRAM_CPP_EXE) program_csharp $(PROGRAM_CSHARP_EXE_ENABLE) $(PROGRAM_CPP_APP_ENABLE)

# One header compile
one_header_setup:
	$(PROGRAM_ONE_HEADER_EXE_DEL_CMD)
	
$(PROGRAM_ONE_HEADER_EXE):
	$(GCC) $(PROGRAM_ONE_HEADER_COMPILE_FLAGS) $(PROGRAM_ONE_HEADER_LINK_FLAGS) -o $@ $(PROGRAM_ONE_HEADER_SRC)/$(PROGRAM_ONE_HEADER_MAIN_NAME).cpp

# One header execute
one_header:
	$(SHARED_LIBRARY_HEADER_DEL_CMD)
	$(PROGRAM_ONE_HEADER_EXE) $(LIBRARY_CPP_ONE_HEADER) $(LIBRARY_CPP_HEADER)
	
# Library shared and static
library_setup:
	$(LIBRARY_TEMP_DIR_DEL)
	$(SHARED_LIBRARY_DEL_CMD)
	$(STATIC_LIBRARY_DEL_CMD)
	$(LIBRARY_OBJ_DIR_CMD)
	$(LIBRARY_PLATFORM_DIR_CMD)
	
$(LIBRARY_CPP_SHARED_BIN): $(LIBRARY_OBJ_FILES)
	$(GCC) $(LIBRARY_SHARED_LINK_FLAGS) -o $@ $^ $(LIBRARY_SHARED_LINK_INCLUDES)
	
$(LIBRARY_CPP_STATIC_BIN): $(LIBRARY_OBJ_FILES)
	$(AR) rcs $@ $^ $(LIBRARY_STATIC_LINK_FLAGS)

$(LIBRARY_OBJ_DIR)/%.o: $(LIBRARY_SRC_DIR)/%.cpp
	$(GCC) $(LIBRARY_OBJ_COMPILE_FLAGS) -c -o $@ $<
	
library_clean:
	$(LIBRARY_TEMP_DIR_DEL)

# Example programs
program_cpp:
	$(PROGRAM_OBJ_DIR_DEL)
	$(PROGRAM_OBJ_DIR_CMD)
	$(PROGRAM_CPP_DEL_CMD)
	
$(PROGRAM_CPP_EXE): $(PROGRAM_OBJ_FILES)
	$(GCC) $(PROGRAM_CPP_LINK) -o $@ $^ $(PROGRAM_EXAMPLE_CPP_LIBS_FLAG)
	$(PROGRAM_OBJ_DIR_DEL)
	
$(PROGRAM_OBJ_DIR)/%.o: $(PROGRAM_SRC_DIR)/%.cpp
	$(GCC) $(PROGRAM_CPP_COMPILE) -c -o $@ $<

program_csharp:
	$(PROGRAM_CSHARP_DEL_CMD)
	$(BUNDLE_CSHARP_DEL_CMD)
	
$(PROGRAM_CSHARP_EXE_ENABLE):
	$(CSC) $(CSC_FLAGS) /out:$@ /t:exe -lib:$(PROGRAM_SHARED_LIB_DIR) $(LIBRARY_INCLUDE_DIR)/$(EXTRA_SRC) $(PROGRAM_SRC_DIR)/*.cs
	$(COPY_SHARED_LIBRARY_CMD)
	$(BUNDLE_CMD)
	$(PROGRAM_CSHARP_DEL_POST_CMD)

$(PROGRAM_CPP_APP_ENABLE):
	$(PROGRAM_CPP_APP_DEL_CMD)
	yes | rm -rf "$(PROGRAM_CPP_APP)"
	mkdir -p $(PROGRAM_CPP_APP)/Contents/MacOS
	mkdir -p $(PROGRAM_CPP_APP)/Contents/Resources
	cp $(PROGRAM_SRC_DIR)/Info.plist "$(PROGRAM_CPP_APP)/Contents/"
	cp $(PROGRAM_CPP_EXE) "$(PROGRAM_CPP_APP)/Contents/MacOS/$(PROGRAM_CPP_APP_NAME)"

clean:
	$(LIBRARY_TEMP_DIR_DEL)
	$(PROGRAM_OBJ_DIR_DEL)
	$(SHARED_LIBRARY_DEL_CMD)
	$(STATIC_LIBRARY_DEL_CMD)
	$(PROGRAM_CSHARP_DEL_CMD)
	$(BUNDLE_CSHARP_DEL_CMD)
	$(PROGRAM_CPP_DEL_CMD)
