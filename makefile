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

# CPP EXAMPLE PROGRAMS
PROGRAM_CPP_EXAMPLE_PARENT_DIR = ../../
PROGRAM_CPP_EXAMPLE_LINK_LIB_FLAGS = 
# mac app
PROGRAM_CPP_APP_NAME = example_cpp
PROGRAM_CPP_APP = $(PROGRAM_BIN_DIR)/example_cpp.app
PROGRAM_CPP_APP_ENABLE = 
PROGRAM_CPP_APP_DEL_CMD = 
# static and dynamic library
LIBRARY_INCLUDE_DIR = include
LIBRARY_CONTRIB_DIR = contrib
LIBRARY_EXAMPLE_DIR = example
LIBRARY_CPP_NAME = ExpressWeb
LIBRARY_CPP_HEADER = $(LIBRARY_INCLUDE_DIR)/$(LIBRARY_CPP_NAME).h
LIBRARY_CPP_ONE_HEADER = $(LIBRARY_SRC_DIR)/OneHeader.h
LIBRARY_CPP_VERSION = gnu++11
LIBRARY_LINK_LIBS = -lboost_filesystem$(BOOST_LIBS_POST) -lboost_thread$(BOOST_LIBS_POST) \
								-lboost_locale$(BOOST_LIBS_POST) -lboost_system$(BOOST_LIBS_POST) -lboost_iostreams$(BOOST_LIBS_POST)
# example c++ program
PROGRAM_CPP_COMPILE_INCLUDES = -I"$(BOOST_INCLUDE_DIR)" -I"$(LIBRARY_INCLUDE_DIR)"
PROGRAM_EXAMPLE_CPP_LIBS_FLAG = -lResourceFileUtility $(LIBRARY_LINK_LIBS)
# CPP CONTRIB PROGRAMS
PROGRAM_CPP_CONTRIB_PARENT_DIR = ../../
ifndef PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS
	PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS = -l$(LIBRARY_CPP_NAME)
endif
ifndef PROGRAM_CPP_INCLUDE_EXTRA
	PROGRAM_CPP_INCLUDE_EXTRA = 
endif
# add extra includes from the parent
PROGRAM_CPP_COMPILE_INCLUDES = $(PROGRAM_CPP_COMPILE_INCLUDES) $(PROGRAM_CPP_INCLUDE_EXTRA)

# default builds
ifndef LIBRARY_CPP_STATIC_BIN_ENABLE
	LIBRARY_CPP_STATIC_BIN_ENABLE = 1
endif
ifndef LIBRARY_CPP_SHARED_BIN_ENABLE
	LIBRARY_CPP_SHARED_BIN_ENABLE = 1
endif
ifndef LIBRARY_CPP_EXE_BIN_ENABLE
	LIBRARY_CPP_EXE_BIN_ENABLE = 0
endif
ifndef PROGRAM_ONE_HEADER_ENABLE
	PROGRAM_ONE_HEADER_ENABLE = 1
endif
ifndef PROGRAM_CPP_CONTRIB_ENABLE
	PROGRAM_CPP_CONTRIB_ENABLE = 1
endif
ifndef PROGRAM_CPP_EXAMPLE_ENABLE
	PROGRAM_CPP_EXAMPLE_ENABLE = 1
endif
ifndef PROGRAM_CPP_EXAMPLE_LIST
	PROGRAM_CPP_EXAMPLE_LIST = beastHTTPClient
endif
ifndef PROGRAM_CPP_CONTRIB_LIST
	PROGRAM_CPP_EXAMPLE_LIST = uWS
endif
LIBRARY_CPP_STATIC_BIN_ENABLE =
ifeq ($(LIBRARY_CPP_STATIC_BIN_ENABLE),1)
	LIBRARY_CPP_STATIC_BIN_ENABLE = LIBRARY_CPP_STATIC_BIN
endif
LIBRARY_CPP_SHARED_BIN_ENABLE =
ifeq ($(LIBRARY_CPP_SHARED_BIN_ENABLE),1)
	LIBRARY_CPP_SHARED_BIN_ENABLE = LIBRARY_CPP_SHARED_BIN
endif
LIBRARY_CPP_EXE_BIN_ENABLE =
ifeq ($(LIBRARY_CPP_EXE_BIN_ENABLE),1)
	LIBRARY_CPP_EXE_BIN_ENABLE = LIBRARY_CPP_EXE_BIN
endif
PROGRAM_ONE_HEADER_ENABLE = 
ifeq ($(PROGRAM_ONE_HEADER_ENABLE),1)
	PROGRAM_ONE_HEADER_ENABLE = one_header
endif
PROGRAM_CPP_CONTRIB_ENABLE = 
ifeq ($(PROGRAM_CPP_CONTRIB_ENABLE),1)
	PROGRAM_CPP_CONTRIB_ENABLE = program_cpp_contrib
endif
PROGRAM_CPP_EXAMPLE_ENABLE = 
ifeq ($(PROGRAM_CPP_EXAMPLE_ENABLE),1)
	PROGRAM_CPP_EXAMPLE_ENABLE = program_cpp_example
endif

#compiler flags
ifeq ($(COMPILER),CLANG)
	COMPILER_NAME = clang
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	LIBRARY_LINK_LIBS_STANDARD = $(LIBRARY_LINK_LIBS) -lstdc++
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++ -shared
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),MINGW)
	COMPILER_NAME = mingw
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall -fmessage-length=0 
	LIBRARY_LINK_LIBS_STANDARD = $(LIBRARY_LINK_LIBS) -lstdc++
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++ -shared
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fmessage-length=0 
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),MSVC)
	COMPILER_NAME = msvc
	LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	LIBRARY_LINK_LIBS_STANDARD = $(LIBRARY_LINK_LIBS) -lstdc++
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++ -shared
	PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
endif
ifeq ($(COMPILER),GCC)
	COMPILER_NAME = gcc
	ifeq ($(TARGET_ARCH),x86)
		LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -m32 -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
		LIBRARY_LINK_LIBS_STANDARD = $(LIBRARY_LINK_LIBS) -lstdc++
		LIBRARY_EXE_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++
		LIBRARY_SHARED_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++ -shared
		PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	endif
	ifeq ($(TARGET_ARCH),x86_64)
		LIBRARY_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
		LIBRARY_LINK_LIBS_STANDARD = $(LIBRARY_LINK_LIBS) -lstdc++
		LIBRARY_EXE_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++
		LIBRARY_SHARED_LINK_FLAGS_STANDARD = -L"$(BOOST_LIBS_DIR)" -static-libgcc -static-libstdc++ -shared
		PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD = $(PROGRAM_CPP_COMPILE_INCLUDES) -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	endif
endif

# target OS setup
ifeq ($(TARGET_OS),WIN)
	LIBRARY_CPP_SHARED_NAME = $(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME).exe
endif
ifeq ($(TARGET_OS),android)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
endif
ifeq ($(TARGET_OS),OSX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
endif
ifeq ($(TARGET_OS),IOS)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
endif
ifeq ($(TARGET_OS),LINUX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
endif
VERSION_NAME = $(TARGET_OS_NAME)-$(TARGET_ARCH)-$(COMPILER_NAME)
# paths
LIBRARY_SRC_DIR = src
LIBRARY_OBJ_DIR = obj
LIBRARY_BIN_DIR = bin
LIBRARY_BIN_VERSION_DIR = $(LIBRARY_BIN_DIR)/$(VERSION_NAME)
LIBRARY_CPP_SHARED_BIN = $(LIBRARY_BIN_VERSION_DIR)/$(LIBRARY_CPP_SHARED_NAME)
LIBRARY_CPP_STATIC_BIN = $(LIBRARY_BIN_VERSION_DIR)/$(LIBRARY_CPP_STATIC_NAME)
LIBRARY_CPP_EXE_BIN = $(LIBRARY_BIN_VERSION_DIR)/$(PROGRAM_CPP_EXE_NAME)
LIBRARY_TEMP_DIR = $(VERSION_NAME)

# OneHeader c++ program
PROGRAM_ONE_HEADER_DIR = contrib/OneHeader
PROGRAM_ONE_HEADER_SRC = $(PROGRAM_ONE_HEADER_DIR)/src
PROGRAM_ONE_HEADER_OBJ = $(PROGRAM_ONE_HEADER_DIR)/obj
PROGRAM_ONE_HEADER_BIN = $(PROGRAM_ONE_HEADER_DIR)/bin
PROGRAM_ONE_HEADER_BIN_VERSION = $(PROGRAM_ONE_HEADER_BIN)/$(VERSION_NAME)
PROGRAM_ONE_HEADER_EXE = $(PROGRAM_ONE_HEADER_BIN_VERSION)/OneHeader.exe

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
	LIBRARY_OBJ_COMPILE_FLAGS = $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD)
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD)
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_SHARED_LINK_FLAGS_STANDARD)
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD)
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD)
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
	LIBRARY_OBJ_COMPILE_FLAGS = $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) -I"$(NDK_PATH)/sysroot/usr/include"
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD)
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_SHARED_LINK_FLAGS_STANDARD) --sysroot="$(NDK_SYSROOT_PATH)"
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) -I"$(NDK_PATH)/sysroot/usr/include"
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) --sysroot="$(NDK_SYSROOT_PATH)"
endif
ifeq ($(TARGET_OS),LINUX)
	# cpp library commands and flags
	LIBRARY_OBJ_COMPILE_FLAGS = $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD)
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_SHARED_LINK_FLAGS_STANDARD) -pthread
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD)
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) -pthread
endif
ifeq ($(TARGET_OS),OSX)
	# cpp library commands and flags
	GCC = g++
	LIBRARY_OBJ_COMPILE_FLAGS = $(LIBRARY_OBJ_COMPILE_FLAGS_STANDARD) -mmacosx-version-min=10.9
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_SHARED_LINK_FLAGS_STANDARD) -dynamiclib -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -mmacosx-version-min=10.9
	LIBRARY_STATIC_LINK_FLAGS = 
	# mac app
	PROGRAM_CPP_APP_ENABLE = $(PROGRAM_CPP_APP)
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) -mmacosx-version-min=10.9
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) -dynamiclib -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -mmacosx-version-min=10.9
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
	PROGRAM_CPP_COMPILE_INCLUDES =  
	LIBRARY_OBJ_COMPILE_FLAGS = $(PROGRAM_CPP_OBJ_COMPILE_FLAGS_STANDARD) -stdlib=libc++ \
		-arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	LIBRARY_EXE_LINK_FLAGS = -stdlib=libc++ -static -undefined dynamic_lookup -arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS) -shared
	LIBRARY_STATIC_LINK_FLAGS =
endif

#shell commands
ifeq ($(HOST_SHELL),WIN)
	# backslash
	LIBRARY_CPP_HEADER_BACKSLASH=$(subst /,\\,$(LIBRARY_CPP_HEADER))
	LIBRARY_TEMP_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_TEMP_DIR))
	LIBRARY_OBJ_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_OBJ_DIR))
	LIBRARY_BIN_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_BIN_DIR))
	LIBRARY_CPP_SHARED_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_SHARED_BIN))
	LIBRARY_CPP_STATIC_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_STATIC_BIN))
	LIBRARY_BIN_VERSION_DIR_BACKSLASH= $(subst /,\\,$(LIBRARY_BIN_VERSION_DIR))
	# commands
	SHARED_LIBRARY_HEADER_DEL_CMD = if exist "$(LIBRARY_CPP_HEADER_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_HEADER_BACKSLASH)"
	LIBRARY_TEMP_DIR_DEL = if exist "$(LIBRARY_TEMP_DIR_BACKSLASH)" rd /s /q "$(LIBRARY_TEMP_DIR_BACKSLASH)"
	LIBRARY_OBJ_DIR_DEL = if exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" rd /s /q "$(LIBRARY_OBJ_DIR_BACKSLASH)"
	SHARED_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)"
	STATIC_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)"
	
	LIBRARY_OBJ_DIR_CMD = if not exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" mkdir $(LIBRARY_OBJ_DIR_BACKSLASH)
	LIBRARY_BIN_DIR_CMD = if not exist "$(LIBRARY_BIN_DIR_BACKSLASH)" mkdir $(LIBRARY_BIN_DIR_BACKSLASH)
	LIBRARY_BIN_VERSION_DIR_CMD = if not exist "$(LIBRARY_BIN_VERSION_DIR_BACKSLASH)" mkdir $(LIBRARY_BIN_VERSION_DIR_BACKSLASH)
	
	COPY_SHARED_LIBRARY_CMD = copy $(LIBRARY_CPP_SHARED_BIN_BACKSLASH) $(PROGRAM_BIN_DIR) /Y
endif
ifeq ($(HOST_SHELL),UNIX)
	# commands
	SHARED_LIBRARY_HEADER_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_HEADER)"
	LIBRARY_TEMP_DIR_DEL = yes | rm -rf $(LIBRARY_TEMP_DIR)
	LIBRARY_OBJ_DIR_DEL = yes | rm -rf $(LIBRARY_OBJ_DIR)
	SHARED_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_SHARED_BIN)"
	STATIC_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_STATIC_BIN)"
	
	LIBRARY_OBJ_DIR_CMD = mkdir -p $(LIBRARY_OBJ_DIR)
	LIBRARY_BIN_DIR_CMD = mkdir -p $(LIBRARY_BIN_DIR)
	LIBRARY_BIN_VERSION_DIR_CMD = mkdir -p $(LIBRARY_BIN_VERSION_DIR)
endif

LIBRARY_SRC_FILES := $(wildcard $(LIBRARY_SRC_DIR)/*.cpp)
LIBRARY_OBJ_FILES := $(patsubst $(LIBRARY_SRC_DIR)/%.cpp,$(LIBRARY_OBJ_DIR)/%.o,$(LIBRARY_SRC_FILES))

all: $(PROGRAM_ONE_HEADER_ENABLE) $(PROGRAM_CPP_CONTRIB_ENABLE) \
	library_setup $(LIBRARY_CPP_STATIC_BIN_ENABLE) $(LIBRARY_CPP_SHARED_BIN_ENABLE) $(LIBRARY_CPP_EXE_BIN_ENABLE) \
	library_clean $(PROGRAM_CPP_EXAMPLE_ENABLE)

# One header execute
one_header:
	$(SHARED_LIBRARY_HEADER_DEL_CMD)
	cd $(PROGRAM_ONE_HEADER_DIR) && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) \
		PROGRAM_ONE_HEADER_ENABLE=0 LIBRARY_CPP_STATIC_BIN_ENABLE=0 \
		LIBRARY_CPP_SHARED_BIN_ENABLE=0 LIBRARY_CPP_EXE_BIN_ENABLE=1 \
		PROGRAM_CPP_CONTRIB_ENABLE=0 PROGRAM_CPP_EXAMPLE_ENABLE=0
	$(PROGRAM_ONE_HEADER_EXE) $(LIBRARY_CPP_ONE_HEADER) $(LIBRARY_CPP_HEADER)
	
# Contrib libraries
program_cpp_contrib: $(PROGRAM_CPP_CONTRIB_LIST)
	cd $(LIBRARY_CONTRIB_DIR)/$< && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) \
		PROGRAM_ONE_HEADER_ENABLE=0 LIBRARY_CPP_STATIC_BIN_ENABLE=1 \
		LIBRARY_CPP_SHARED_BIN_ENABLE=1 LIBRARY_CPP_EXE_BIN_ENABLE=0 \
		PROGRAM_CPP_CONTRIB_ENABLE=0 PROGRAM_CPP_EXAMPLE_ENABLE=0
	$(PROGRAM_ONE_HEADER_EXE) $(LIBRARY_CONTRIB_DIR)/$</src/OneHeader.h $(LIBRARY_INCLUDE_DIR)/$<.h
	
# Library shared and static
library_setup:
	$(LIBRARY_TEMP_DIR_DEL)
	$(LIBRARY_OBJ_DIR_DEL)
	$(SHARED_LIBRARY_DEL_CMD)
	$(STATIC_LIBRARY_DEL_CMD)
	$(LIBRARY_OBJ_DIR_CMD)
	$(LIBRARY_BIN_DIR_CMD)
	$(LIBRARY_BIN_VERSION_DIR_CMD)

LIBRARY_CPP_EXE_BIN: $(LIBRARY_OBJ_FILES)
	$(GCC) $(LIBRARY_EXE_LINK_FLAGS) -o $(LIBRARY_CPP_EXE_BIN) $^ $(LIBRARY_LINK_LIBS_STANDARD)

LIBRARY_CPP_SHARED_BIN: $(LIBRARY_OBJ_FILES)
	$(GCC) $(LIBRARY_SHARED_LINK_FLAGS) -o $(LIBRARY_CPP_SHARED_BIN) $^ $(LIBRARY_LINK_LIBS_STANDARD)
	
LIBRARY_CPP_STATIC_BIN: $(LIBRARY_OBJ_FILES)
	$(AR) rcs $(LIBRARY_CPP_STATIC_BIN) $^ $(LIBRARY_STATIC_LINK_FLAGS)

$(LIBRARY_OBJ_DIR)/%.o: $(LIBRARY_SRC_DIR)/%.cpp
	$(GCC) $(LIBRARY_OBJ_COMPILE_FLAGS) -c -o $@ $<
	
library_clean:
	$(LIBRARY_OBJ_DIR_DEL)

# Example programs
program_cpp_example: $(PROGRAM_CPP_EXAMPLE_LIST)
	cd $< && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) \
		PROGRAM_CPP_INCLUDE_EXTRA=-I$(PROGRAM_CPP_CONTRIB_PARENT_DIR)/$(LIBRARY_INCLUDE_DIR) \
		PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS=$(PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS) \
		PROGRAM_ONE_HEADER_ENABLE=0 LIBRARY_CPP_STATIC_BIN_ENABLE=0 \
		LIBRARY_CPP_SHARED_BIN_ENABLE=0 LIBRARY_CPP_EXE_BIN_ENABLE=1 \
		PROGRAM_CPP_CONTRIB_ENABLE=0 PROGRAM_CPP_EXAMPLE_ENABLE=0

clean:
	$(LIBRARY_TEMP_DIR_DEL)
	$(SHARED_LIBRARY_DEL_CMD)
	$(STATIC_LIBRARY_DEL_CMD)
