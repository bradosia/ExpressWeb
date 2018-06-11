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
PROGRAM_BIN_DIR = bin
PROGRAM_OBJ_DIR = obj
PROGRAM_SRC_DIR = src
PROGRAM_CPP_VERSION = gnu++11
PROGRAM_LINK_INCLUDES = -lboost_filesystem$(BOOST_LIBS_POST) -lboost_thread$(BOOST_LIBS_POST) \
						-lboost_locale$(BOOST_LIBS_POST) -lboost_system$(BOOST_LIBS_POST) -lboost_iostreams$(BOOST_LIBS_POST)
# static and dynamic library
LIBRARY_INCLUDE_DIR = ../../include
LIBRARY_DIR = bin
LIBRARY_CPP_NAME = ExpressWeb
LIBRARY_CPP_HEADER = $(LIBRARY_INCLUDE_DIR)/$(LIBRARY_CPP_NAME).h
# c++ program
PROGRAM_CPP_COMPILE_INCLUDES = -I"$(LIBRARY_INCLUDE_DIR)" -I"$(BOOST_INCLUDE_DIR)"
PROGRAM_CPP_LINK_LIBS_FLAG = -l$(LIBRARY_CPP_NAME) $(PROGRAM_LINK_INCLUDES)

#compiler flags
ifeq ($(COMPILER),CLANG)
	COMPILER_NAME = clang
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),MINGW)
	COMPILER_NAME = mingw
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),MSVC)
	COMPILER_NAME = msvc
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),GCC)
	COMPILER_NAME = gcc
	ifeq ($(TARGET_ARCH),x86)
		PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	endif
	ifeq ($(TARGET_ARCH),x86_64)
		PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
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
LIBRARY_OBJ_DIR = $(LIBRARY_TEMP_DIR)/obj

# c++ program
PROGRAM_CPP_LINK_LIBRARYS_FLAG = -L"$(LIBRARY_PLATFORM_DIR)" -L"$(BOOST_LIBS_DIR)"
PROGRAM_CPP_BIN_VERSION_DIR = $(PROGRAM_BIN_DIR)/$(VERSION_NAME)
PROGRAM_CPP_EXE = $(PROGRAM_CPP_BIN_VERSION_DIR)/example.exe

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
	# program c++
	PROGRAM_CPP_COMPILE = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD)
	PROGRAM_CPP_LINK = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) $(PROGRAM_CPP_LINK_LIBRARYS_FLAG)
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
	# program c++
	PROGRAM_CPP_COMPILE = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) -I"$(NDK_PATH)/sysroot/usr/include"
	PROGRAM_CPP_LINK = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) $(PROGRAM_CPP_LINK_LIBRARYS_FLAG) --sysroot="$(NDK_SYSROOT_PATH)"
endif

ifeq ($(TARGET_OS),LINUX)
	# program c++
	PROGRAM_CPP_COMPILE = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES)
	PROGRAM_CPP_LINK = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) $(PROGRAM_CPP_LINK_LIBRARYS_FLAG) -pthread
endif
ifeq ($(TARGET_OS),OSX)
	# program c++
	PROGRAM_CPP_COMPILE = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) -mmacosx-version-min=10.9
	PROGRAM_CPP_LINK = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) $(PROGRAM_CPP_LINK_LIBRARYS_FLAG) -mmacosx-version-min=10.9
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
	# program c++
	PROGRAM_CPP_COMPILE = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	PROGRAM_CPP_LINK = $(PROGRAM_CPP_LINK_LIBRARYS_FLAG) -std=gnu++11 -stdlib=libc++ -L"$(IPHONE_SDK_LIBS_DIR)" -undefined dynamic_lookup \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
endif

#shell commands
ifeq ($(HOST_SHELL),WIN)
	# backslash
	PROGRAM_CPP_EXE_BACKSLASH=$(subst /,\\,$(PROGRAM_CPP_EXE))
	PROGRAM_OBJ_DIR_BACKSLASH=$(subst /,\\,$(PROGRAM_OBJ_DIR))
	PROGRAM_BIN_DIR_BACKSLASH=$(subst /,\\,$(PROGRAM_BIN_DIR))
	PROGRAM_CPP_BIN_VERSION_DIR_BACKSLASH=$(subst /,\\,$(PROGRAM_CPP_BIN_VERSION_DIR))
	# commands
	PROGRAM_OBJ_DIR_DEL = if exist "$(PROGRAM_OBJ_DIR_BACKSLASH)" rd /s /q "$(PROGRAM_OBJ_DIR_BACKSLASH)"
	LIBRARY_OBJ_DIR_CMD = if not exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" mkdir $(LIBRARY_OBJ_DIR_BACKSLASH)
	PROGRAM_CPP_DEL_CMD = if exist "$(PROGRAM_CPP_EXE_BACKSLASH)" del /F /Q "$(PROGRAM_CPP_EXE_BACKSLASH)"
	PROGRAM_BIN_DIR_CMD = if not exist "$(PROGRAM_BIN_DIR_BACKSLASH)" mkdir $(PROGRAM_BIN_DIR_BACKSLASH)
	PROGRAM_BIN_VERSION_DIR_CMD = if not exist "$(PROGRAM_CPP_BIN_VERSION_DIR_BACKSLASH)" mkdir $(PROGRAM_CPP_BIN_VERSION_DIR_BACKSLASH)
endif
ifeq ($(HOST_SHELL),UNIX)
	# commands
	PROGRAM_OBJ_DIR_DEL = yes | rm -rf $(PROGRAM_OBJ_DIR)
	PROGRAM_OBJ_DIR_CMD = mkdir -p $(PROGRAM_OBJ_DIR)
	PROGRAM_CPP_DEL_CMD = yes | rm -f "$(PROGRAM_CPP_EXE)"
	PROGRAM_BIN_DIR_CMD = mkdir -p $(PROGRAM_BIN_DIR)
	PROGRAM_BIN_VERSION_DIR_CMD = mkdir -p $(PROGRAM_CPP_BIN_VERSION_DIR)
	PROGRAM_CPP_APP_DEL_CMD = yes | rm -rf "$(PROGRAM_CPP_APP)"
endif

PROGRAM_SRC_FILES := $(wildcard $(PROGRAM_SRC_DIR)/*.cpp)
PROGRAM_OBJ_FILES := $(patsubst $(PROGRAM_SRC_DIR)/%.cpp,$(PROGRAM_OBJ_DIR)/%.o,$(PROGRAM_SRC_FILES))

EXAMPLE_SRC = *.cs
EXTRA_SRC = ResourceFileUtility.cs

all: program_cpp_setup $(PROGRAM_CPP_EXE) $(PROGRAM_CPP_APP_ENABLE)

# CPP Program
program_cpp_setup:
	$(PROGRAM_OBJ_DIR_DEL)
	$(PROGRAM_OBJ_DIR_CMD)
	$(PROGRAM_CPP_DEL_CMD)
	$(PROGRAM_BIN_DIR_CMD)
	$(PROGRAM_BIN_VERSION_DIR_CMD)
	
$(PROGRAM_CPP_EXE): $(PROGRAM_OBJ_FILES)
	$(GCC) $(PROGRAM_CPP_LINK) -o $@ $^ $(PROGRAM_CPP_LINK_LIBS_FLAG)
	$(PROGRAM_OBJ_DIR_DEL)
	
$(PROGRAM_OBJ_DIR)/%.o: $(PROGRAM_SRC_DIR)/%.cpp
	$(GCC) $(PROGRAM_CPP_COMPILE) -c -o $@ $<

$(PROGRAM_CPP_APP_ENABLE):
	$(PROGRAM_CPP_APP_DEL_CMD)
	yes | rm -rf "$(PROGRAM_CPP_APP)"
	mkdir -p $(PROGRAM_CPP_APP)/Contents/MacOS
	mkdir -p $(PROGRAM_CPP_APP)/Contents/Resources
	cp $(PROGRAM_SRC_DIR)/Info.plist "$(PROGRAM_CPP_APP)/Contents/"
	cp $(PROGRAM_CPP_EXE) "$(PROGRAM_CPP_APP)/Contents/MacOS/$(PROGRAM_CPP_APP_NAME)"
