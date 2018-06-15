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

include makefile.mk

# CPP EXAMPLE PROGRAMS
ifndef LIBRARY_EXAMPLE_PARENT_DIR
	LIBRARY_EXAMPLE_PARENT_DIR = .
endif
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
LIBRARY_ONE_HEADER_COMBINE_DIR = include
LIBRARY_CPP_VERSION = gnu++11
# CPP CONTRIB PROGRAMS
ifndef LIBRARY_CONTRIB_PARENT_DIR
	LIBRARY_CONTRIB_PARENT_DIR = .
endif
ifndef PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS
	PROGRAM_CPP_CONTRIB_LINK_LIB_FLAGS = -lssl -lcrypto
endif
ifndef PROGRAM_CPP_EXAMPLE_LINK_LIB_FLAGS
	PROGRAM_CPP_EXAMPLE_LINK_LIB_FLAGS = -l$(LIBRARY_CPP_NAME)
endif
ifndef PROGRAM_CPP_INCLUDE_EXTRA
	PROGRAM_CPP_INCLUDE_EXTRA = 
endif

# One Header
ifndef LIBRARY_CPP_ONE_HEADER_PARENT_FLAG
	LIBRARY_CPP_ONE_HEADER_PARENT_FLAG = 0
endif
ifndef LIBRARY_CPP_ONE_HEADER_COMBINE_DIR
	LIBRARY_CPP_ONE_HEADER_COMBINE_DIR = $(LIBRARY_ONE_HEADER_COMBINE_DIR)
endif
ifeq ($(LIBRARY_CPP_ONE_HEADER_PARENT_FLAG),1)
	LIBRARY_CPP_ONE_HEADER_COMBINE_DIR = $(LIBRARY_CONTRIB_PARENT_DIR)/$(LIBRARY_ONE_HEADER_COMBINE_DIR)
endif
LIBRARY_CPP_ONE_HEADER_COMBINE = $(LIBRARY_CPP_ONE_HEADER_COMBINE_DIR)/$(LIBRARY_CPP_NAME).h
LIBRARY_CPP_ONE_HEADER = $(LIBRARY_SRC_DIR)/OneHeader.h

# default builds
ifndef LIBRARY_CPP_STATIC_BIN_FLAG
	LIBRARY_CPP_STATIC_BIN_FLAG = 1
endif
ifndef LIBRARY_CPP_SHARED_BIN_FLAG
	LIBRARY_CPP_SHARED_BIN_FLAG = 1
endif
ifndef LIBRARY_CPP_EXE_BIN_FLAG
	LIBRARY_CPP_EXE_BIN_FLAG = 0
endif
ifndef PROGRAM_ONE_HEADER_BUILD_FLAG
	PROGRAM_ONE_HEADER_BUILD_FLAG = 1
endif
ifndef PROGRAM_ONE_HEADER_USE_FLAG
	PROGRAM_ONE_HEADER_USE_FLAG = 1
endif
ifndef PROGRAM_CPP_CONTRIB_FLAG
	PROGRAM_CPP_CONTRIB_FLAG = 1
endif
ifndef PROGRAM_CPP_EXAMPLE_FLAG
	PROGRAM_CPP_EXAMPLE_FLAG = 1
endif
ifndef PROGRAM_CPP_EXAMPLE_LIST
	PROGRAM_CPP_EXAMPLE_LIST = 
endif
ifndef PROGRAM_CPP_CONTRIB_LIST
	PROGRAM_CPP_CONTRIB_LIST = 
endif
ifndef LIBRARY_CPP_NAME
	LIBRARY_CPP_NAME = 
endif
LIBRARY_CPP_STATIC_BIN_ENABLE = 
ifeq ($(LIBRARY_CPP_STATIC_BIN_FLAG),1)
	LIBRARY_CPP_STATIC_BIN_ENABLE = LIBRARY_CPP_STATIC_BIN
endif
LIBRARY_CPP_SHARED_BIN_ENABLE = 
ifeq ($(LIBRARY_CPP_SHARED_BIN_FLAG),1)
	LIBRARY_CPP_SHARED_BIN_ENABLE = LIBRARY_CPP_SHARED_BIN
endif
LIBRARY_CPP_EXE_BIN_ENABLE = 
ifeq ($(LIBRARY_CPP_EXE_BIN_FLAG),1)
	LIBRARY_CPP_EXE_BIN_ENABLE = LIBRARY_CPP_EXE_BIN
endif
PROGRAM_ONE_HEADER_BUILD_ENABLE = 
ifeq ($(PROGRAM_ONE_HEADER_BUILD_FLAG),1)
	PROGRAM_ONE_HEADER_BUILD_ENABLE = one_header_build
endif
PROGRAM_ONE_HEADER_USE_ENABLE = 
ifeq ($(PROGRAM_ONE_HEADER_USE_FLAG),1)
	PROGRAM_ONE_HEADER_USE_ENABLE = one_header_use
endif
PROGRAM_CPP_CONTRIB_ENABLE = 
ifeq ($(PROGRAM_CPP_CONTRIB_FLAG),1)
	PROGRAM_CPP_CONTRIB_ENABLE = program_cpp_contrib
endif
PROGRAM_CPP_EXAMPLE_ENABLE = 
ifeq ($(PROGRAM_CPP_EXAMPLE_FLAG),1)
	PROGRAM_CPP_EXAMPLE_ENABLE = program_cpp_example
endif

#compiler flags
ifeq ($(COMPILER),CLANG)
	COMPILER_NAME = clang
	LIBRARY_COMPILE_INCLUDE_PATH_STANDARD = -I"$(LIBRARY_INCLUDE_DIR)"
	LIBRARY_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -shared
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	LIBRARY_LINK_LIBS_PATH_STANDARD = 
	LIBRARY_LINK_LIBS_STANDARD = -lstdc++
	PROGRAM_CPP_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall 
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	PROGRAM_LINK_LIBS_STANDARD = 
endif
ifeq ($(COMPILER),MINGW)
	COMPILER_NAME = mingw
	LIBRARY_COMPILE_INCLUDE_PATH_STANDARD = -I"$(LIBRARY_INCLUDE_DIR)"
	LIBRARY_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall -fmessage-length=0 
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -shared
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	LIBRARY_LINK_LIBS_PATH_STANDARD = 
	LIBRARY_LINK_LIBS_STANDARD = 
	PROGRAM_CPP_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fmessage-length=0 
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	PROGRAM_LINK_LIBS_STANDARD = 
endif
ifeq ($(COMPILER),MSVC)
	COMPILER_NAME = msvc
	LIBRARY_COMPILE_INCLUDE_PATH_STANDARD = -I"$(LIBRARY_INCLUDE_DIR)"
	LIBRARY_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall
	LIBRARY_SHARED_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -shared
	LIBRARY_EXE_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++
	LIBRARY_LINK_LIBS_PATH_STANDARD = 
	LIBRARY_LINK_LIBS_STANDARD = -lstdc++
	PROGRAM_CPP_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall
	PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
	PROGRAM_LINK_LIBS_STANDARD = -lstdc++
endif
ifeq ($(COMPILER),GCC)
	COMPILER_NAME = gcc
	ifeq ($(TARGET_ARCH),x86)
		LIBRARY_COMPILE_INCLUDE_PATH_STANDARD = -I"$(LIBRARY_INCLUDE_DIR)"
		LIBRARY_COMPILE_FLAGS_STANDARD = -m32 -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall -fPIC
		LIBRARY_SHARED_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -shared
		LIBRARY_EXE_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
		LIBRARY_LINK_LIBS_PATH_STANDARD = 
		LIBRARY_LINK_LIBS_STANDARD = -lstdc++
		PROGRAM_CPP_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
		PROGRAM_LINK_LIBS_STANDARD = -lstdc++
	endif
	ifeq ($(TARGET_ARCH),x86_64)
		LIBRARY_COMPILE_INCLUDE_PATH_STANDARD = -I"$(LIBRARY_INCLUDE_DIR)"
		LIBRARY_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(LIBRARY_CPP_VERSION) -Wall -fPIC
		LIBRARY_SHARED_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -shared
		LIBRARY_EXE_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
		LIBRARY_LINK_LIBS_PATH_STANDARD = 
		LIBRARY_LINK_LIBS_STANDARD = -lstdc++
		PROGRAM_CPP_COMPILE_FLAGS_STANDARD = -O3 -g3 -std=$(PROGRAM_CPP_VERSION) -Wall -fPIC
		PROGRAM_CPP_LINK_FLAGS_STANDARD = -static-libgcc -static-libstdc++ -static
		PROGRAM_LINK_LIBS_STANDARD = -lstdc++
	endif
endif

# VERSION NAME WITH TARGET OS-ARCHITECTURE-COMPILER
VERSION_NAME = $(TARGET_OS_NAME)-$(TARGET_ARCH)-$(COMPILER_NAME)

# target OS setup
ifeq ($(TARGET_OS),WIN)
	LIBRARY_CPP_SHARED_NAME = $(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME).exe
	PROGRAM_SUFFIX = .exe
endif
ifeq ($(TARGET_OS),android)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).dll
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
	PROGRAM_SUFFIX =
endif
ifeq ($(TARGET_OS),OSX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
	PROGRAM_SUFFIX =
endif
ifeq ($(TARGET_OS),IOS)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
	PROGRAM_SUFFIX =
endif
ifeq ($(TARGET_OS),LINUX)
	LIBRARY_CPP_SHARED_NAME = lib$(LIBRARY_CPP_NAME).so
	LIBRARY_CPP_STATIC_NAME = lib$(LIBRARY_CPP_NAME).a
	PROGRAM_CPP_EXE_NAME = $(LIBRARY_CPP_NAME)
	PROGRAM_SUFFIX =
endif
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
PROGRAM_ONE_HEADER_NAME = OneHeader
PROGRAM_ONE_HEADER_DIR = contrib/$(PROGRAM_ONE_HEADER_NAME)
PROGRAM_ONE_HEADER_BIN = $(PROGRAM_ONE_HEADER_DIR)/bin
PROGRAM_ONE_HEADER_BIN_VERSION = $(PROGRAM_ONE_HEADER_BIN)/$(VERSION_NAME)
ifndef PROGRAM_ONE_HEADER_EXE
	PROGRAM_ONE_HEADER_EXE = $(PROGRAM_ONE_HEADER_BIN_VERSION)/$(PROGRAM_ONE_HEADER_NAME)$(PROGRAM_SUFFIX)
endif
ifndef LIBRARY_CPP_ONE_HEADER_PARENT_FLAG
	LIBRARY_CPP_ONE_HEADER_PARENT_FLAG = 0
endif
PROGRAM_ONE_HEADER_EXE_FINAL = $(PROGRAM_ONE_HEADER_EXE)
ifeq ($(LIBRARY_CPP_ONE_HEADER_PARENT_FLAG),1)
	PROGRAM_ONE_HEADER_EXE_FINAL = $(LIBRARY_CONTRIB_PARENT_DIR)/$(PROGRAM_ONE_HEADER_EXE)
endif

# Library Compile Include Paths
LIBRARY_CPP_COMPILE_INCLUDE_PATH_I := $(patsubst %, -I"%", $(LIBRARY_CPP_COMPILE_INCLUDE_PATH))
LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I := $(patsubst %, -I"%", $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM))
# Library Link Library Paths
LIBRARY_CPP_LINK_LIB_PATHS_L := $(patsubst %, -L"%", $(LIBRARY_CPP_LINK_LIB_PATHS))
LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L := $(patsubst %, -L"%", $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM))
LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L := $(patsubst %, -L"%/$(VERSION_NAME)", $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION))
# Library Link Libraries
LIBRARY_CPP_LINK_LIBS_l := $(patsubst %, -l"%", $(LIBRARY_CPP_LINK_LIBS))
LIBRARY_CPP_LINK_LIB_PLATFORM_l := $(patsubst %, -l"%", $(LIBRARY_CPP_LINK_LIB_PLATFORM))
LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l := $(patsubst %, -l"%$(BOOST_LIBS_POST)", $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST))
# Program Link Library Paths
PROGRAM_CPP_LINK_LIB_PATHS_L := $(patsubst %, -L"%", $(PROGRAM_CPP_LINK_LIB_PATHS))
PROGRAM_CPP_LINK_LIB_PATHS_VERSION_L := $(patsubst %, -L"%/$(VERSION_NAME)", $(PROGRAM_CPP_LINK_LIB_PATHS_VERSION))
# Program Link Libraries
PROGRAM_CPP_LINK_LIBS_l := $(patsubst %, -l"%", $(PROGRAM_CPP_LINK_LIBS))

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
#ios architecture setup
ifeq ($(TARGET_ARCH),armv7)
	IOS_ARCH = armv7
endif
ifeq ($(TARGET_ARCH),armv7s)
	IOS_ARCH = armv7s
endif
ifeq ($(TARGET_ARCH),arm64)
	IOS_ARCH = arm64
endif
# target OS setup
ifeq ($(TARGET_OS),WIN)
	# cpp library commands and flags
	LIBRARY_COMPILE_INCLUDE_PATH = $(LIBRARY_COMPILE_INCLUDE_PATH_STANDARD) -I"$(BOOST_INCLUDE_DIR)" \
		$(LIBRARY_CPP_COMPILE_INCLUDE_PATH_I) $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I)
	LIBRARY_COMPILE_FLAGS = $(LIBRARY_COMPILE_FLAGS_STANDARD)
	
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD)
	LIBRARY_EXE_LINK_LIB_PATH = $(LIBRARY_LINK_LIBS_PATH_STANDARD) $(LIBRARY_CPP_LINK_LIB_PATHS_L) $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L) \
		-L"$(BOOST_LIBS_DIR)" $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L)
	LIBRARY_EXE_LINK_LIBS = $(LIBRARY_LINK_LIBS_STANDARD) $(LIBRARY_CPP_LINK_LIBS_l) $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l) \
		$(LIBRARY_CPP_LINK_LIB_PLATFORM_l)
	
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS) -shared
	LIBRARY_SHARED_LINK_LIB_PATH = $(LIBRARY_EXE_LINK_LIB_PATH)
	LIBRARY_SHARED_LINK_LIBS = $(LIBRARY_EXE_LINK_LIBS)
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_COMPILE_FLAGS_STANDARD)
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD)
endif
ifeq ($(TARGET_OS),android)
	LIBRARY_CPP_LINK_LIB_PATHS_ANDROID_I := $(patsubst %, -I"$(NDK_PATH)%", $(LIBRARY_CPP_LINK_LIB_PATHS_ANDROID))
	# cpp library commands and flags
	LIBRARY_COMPILE_INCLUDE_PATH = $(LIBRARY_COMPILE_INCLUDE_PATH_STANDARD) -I"$(BOOST_INCLUDE_DIR)" -I"$(NDK_PATH)/sysroot/usr/include" \
		$(LIBRARY_CPP_COMPILE_INCLUDE_PATH_I) $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I)
	LIBRARY_COMPILE_FLAGS = $(LIBRARY_COMPILE_FLAGS_STANDARD)
	
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD) --sysroot="$(NDK_SYSROOT_PATH)"
	LIBRARY_EXE_LINK_LIB_PATH = $(LIBRARY_LINK_LIBS_PATH_STANDARD) $(LIBRARY_CPP_LINK_LIB_PATHS_L) $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L) \
		-L"$(BOOST_LIBS_DIR)" $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L)
	LIBRARY_EXE_LINK_LIBS = $(LIBRARY_LINK_LIBS_STANDARD) $(LIBRARY_CPP_LINK_LIBS_l) $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l) \
		$(LIBRARY_CPP_LINK_LIB_PLATFORM_l)
	
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS) -shared
	LIBRARY_SHARED_LINK_LIB_PATH = $(LIBRARY_EXE_LINK_LIB_PATH)
	LIBRARY_SHARED_LINK_LIBS = $(LIBRARY_EXE_LINK_LIBS)
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_COMPILE_FLAGS_STANDARD) $(ARCH_OBJ_COMPILE_INCLUDES) -I"$(NDK_PATH)/sysroot/usr/include"
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) --sysroot="$(NDK_SYSROOT_PATH)"
endif
ifeq ($(TARGET_OS),LINUX)
	# cpp library commands and flags
	LIBRARY_COMPILE_INCLUDE_PATH = $(LIBRARY_COMPILE_INCLUDE_PATH_STANDARD) -I"$(BOOST_INCLUDE_DIR)" \
		$(LIBRARY_CPP_COMPILE_INCLUDE_PATH_I) $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I)
	LIBRARY_COMPILE_FLAGS = $(LIBRARY_COMPILE_FLAGS_STANDARD)
	
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD) -pthread
	LIBRARY_EXE_LINK_LIB_PATH = $(LIBRARY_LINK_LIBS_PATH_STANDARD) $(LIBRARY_CPP_LINK_LIB_PATHS_L) $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L) \
		-L"$(BOOST_LIBS_DIR)" $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L)
	LIBRARY_EXE_LINK_LIBS = $(LIBRARY_LINK_LIBS_STANDARD) $(LIBRARY_CPP_LINK_LIBS_l) $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l) \
		$(LIBRARY_CPP_LINK_LIB_PLATFORM_l)
	
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_SHARED_LINK_FLAGS_STANDARD) -shared
	LIBRARY_SHARED_LINK_LIB_PATH = $(LIBRARY_EXE_LINK_LIB_PATH)
	LIBRARY_SHARED_LINK_LIBS = $(LIBRARY_EXE_LINK_LIBS)
	LIBRARY_STATIC_LINK_FLAGS = 
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_COMPILE_FLAGS_STANDARD)
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) -pthread
endif
ifeq ($(TARGET_OS),OSX)
	# cpp library commands and flags
	LIBRARY_COMPILE_INCLUDE_PATH = $(LIBRARY_COMPILE_INCLUDE_PATH_STANDARD) -I"$(BOOST_INCLUDE_DIR)" \
		$(LIBRARY_CPP_COMPILE_INCLUDE_PATH_I) $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I)
	LIBRARY_COMPILE_FLAGS = $(LIBRARY_COMPILE_FLAGS_STANDARD) -mmacosx-version-min=10.9
	
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD) \
		-dynamiclib -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -mmacosx-version-min=10.9
	LIBRARY_EXE_LINK_LIB_PATH = $(LIBRARY_LINK_LIBS_PATH_STANDARD) $(LIBRARY_CPP_LINK_LIB_PATHS_L) $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L) \
		-L"$(BOOST_LIBS_DIR)" $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L)
	LIBRARY_EXE_LINK_LIBS = $(LIBRARY_LINK_LIBS_STANDARD) $(LIBRARY_CPP_LINK_LIBS_l) $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l) \
		$(LIBRARY_CPP_LINK_LIB_PLATFORM_l)
	
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS) -shared
	LIBRARY_SHARED_LINK_LIB_PATH = $(LIBRARY_EXE_LINK_LIB_PATH)
	LIBRARY_SHARED_LINK_LIBS = $(LIBRARY_EXE_LINK_LIBS)
	LIBRARY_STATIC_LINK_FLAGS = 
	# mac app
	PROGRAM_CPP_APP_ENABLE = $(PROGRAM_CPP_APP)
	# OneHeader c++
	PROGRAM_ONE_HEADER_COMPILE_FLAGS = $(PROGRAM_CPP_COMPILE_FLAGS_STANDARD) -mmacosx-version-min=10.9
	PROGRAM_ONE_HEADER_LINK_FLAGS = $(PROGRAM_CPP_LINK_FLAGS_STANDARD) \
		-dynamiclib -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -mmacosx-version-min=10.9
endif
ifeq ($(TARGET_OS),IOS)
	# cpp library commands and flags
	LIBRARY_COMPILE_INCLUDE_PATH = $(LIBRARY_COMPILE_INCLUDE_PATH_STANDARD) -I"$(BOOST_INCLUDE_DIR)" \
		$(LIBRARY_CPP_COMPILE_INCLUDE_PATH_I) $(LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM_I)
	LIBRARY_COMPILE_FLAGS = $(LIBRARY_COMPILE_FLAGS_STANDARD) \
		-stdlib=libc++ -arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	
	LIBRARY_EXE_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS_STANDARD) \
		-stdlib=libc++ -undefined dynamic_lookup -arch $(IOS_ARCH) -mios-version-min=5.0 -isysroot $(IPHONE_SDK_PATH)
	LIBRARY_EXE_LINK_LIB_PATH = $(LIBRARY_LINK_LIBS_PATH_STANDARD) $(LIBRARY_CPP_LINK_LIB_PATHS_L) $(LIBRARY_CPP_LINK_LIB_PATHS_VERSION_L) \
		-L"$(BOOST_LIBS_DIR)" $(LIBRARY_CPP_LINK_LIB_PATH_PLATFORM_L)
	LIBRARY_EXE_LINK_LIBS = $(LIBRARY_LINK_LIBS_STANDARD) $(LIBRARY_CPP_LINK_LIBS_l) $(LIBRARY_CPP_LINK_LIBS_VERSION_BOOST_l) \
		$(LIBRARY_CPP_LINK_LIB_PLATFORM_l)
	
	LIBRARY_SHARED_LINK_FLAGS = $(LIBRARY_EXE_LINK_FLAGS) -shared
	LIBRARY_SHARED_LINK_LIB_PATH = $(LIBRARY_EXE_LINK_LIB_PATH)
	LIBRARY_SHARED_LINK_LIBS = $(LIBRARY_EXE_LINK_LIBS)
	LIBRARY_STATIC_LINK_FLAGS = 
endif

#shell commands
ifeq ($(HOST_SHELL),WIN)
	# backslash
	LIBRARY_CPP_ONE_HEADER_COMBINE_BACKSLASH=$(subst /,\\,$(LIBRARY_CPP_ONE_HEADER_COMBINE))
	LIBRARY_TEMP_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_TEMP_DIR))
	LIBRARY_OBJ_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_OBJ_DIR))
	LIBRARY_BIN_DIR_BACKSLASH = $(subst /,\\,$(LIBRARY_BIN_DIR))
	LIBRARY_CPP_SHARED_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_SHARED_BIN))
	LIBRARY_CPP_STATIC_BIN_BACKSLASH= $(subst /,\\,$(LIBRARY_CPP_STATIC_BIN))
	LIBRARY_BIN_VERSION_DIR_BACKSLASH= $(subst /,\\,$(LIBRARY_BIN_VERSION_DIR))
	# commands
	SHARED_LIBRARY_HEADER_DEL_CMD = if exist "$(LIBRARY_CPP_ONE_HEADER_COMBINE_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_ONE_HEADER_COMBINE_BACKSLASH)"
	LIBRARY_TEMP_DIR_DEL = if exist "$(LIBRARY_TEMP_DIR_BACKSLASH)" rd /s /q "$(LIBRARY_TEMP_DIR_BACKSLASH)"
	LIBRARY_OBJ_DIR_DEL = if exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" rd /s /q "$(LIBRARY_OBJ_DIR_BACKSLASH)"
	SHARED_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_SHARED_BIN_BACKSLASH)"
	STATIC_LIBRARY_DEL_CMD = if exist "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)" del /F /Q "$(LIBRARY_CPP_STATIC_BIN_BACKSLASH)"
	
	LIBRARY_OBJ_DIR_CMD = if not exist "$(LIBRARY_OBJ_DIR_BACKSLASH)" mkdir $(LIBRARY_OBJ_DIR_BACKSLASH)
	LIBRARY_BIN_DIR_CMD = if not exist "$(LIBRARY_BIN_DIR_BACKSLASH)" mkdir $(LIBRARY_BIN_DIR_BACKSLASH)
	LIBRARY_BIN_VERSION_DIR_CMD = if not exist "$(LIBRARY_BIN_VERSION_DIR_BACKSLASH)" mkdir $(LIBRARY_BIN_VERSION_DIR_BACKSLASH)
	
	COPY_SHARED_LIBRARY_CMD = copy $(LIBRARY_CPP_SHARED_BIN_BACKSLASH) $(PROGRAM_BIN_DIR) /Y
	# divider
	CLI_MULTIPLE = &
endif
ifeq ($(HOST_SHELL),UNIX)
	# commands
	SHARED_LIBRARY_HEADER_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_ONE_HEADER_COMBINE)"
	LIBRARY_TEMP_DIR_DEL = yes | rm -rf $(LIBRARY_TEMP_DIR)
	LIBRARY_OBJ_DIR_DEL = yes | rm -rf $(LIBRARY_OBJ_DIR)
	SHARED_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_SHARED_BIN)"
	STATIC_LIBRARY_DEL_CMD = yes | rm -f "$(LIBRARY_CPP_STATIC_BIN)"
	
	LIBRARY_OBJ_DIR_CMD = mkdir -p $(LIBRARY_OBJ_DIR)
	LIBRARY_BIN_DIR_CMD = mkdir -p $(LIBRARY_BIN_DIR)
	LIBRARY_BIN_VERSION_DIR_CMD = mkdir -p $(LIBRARY_BIN_VERSION_DIR)
	# divider
	CLI_MULTIPLE = ;
endif

LIBRARY_SRC_FILES := $(wildcard $(LIBRARY_SRC_DIR)/*.cpp)
LIBRARY_OBJ_FILES := $(patsubst $(LIBRARY_SRC_DIR)/%.cpp,$(LIBRARY_OBJ_DIR)/%.o,$(LIBRARY_SRC_FILES))

all: $(PROGRAM_ONE_HEADER_BUILD_ENABLE) $(PROGRAM_ONE_HEADER_USE_ENABLE) $(PROGRAM_CPP_CONTRIB_ENABLE) \
	library_setup $(LIBRARY_CPP_STATIC_BIN_ENABLE) $(LIBRARY_CPP_SHARED_BIN_ENABLE) $(LIBRARY_CPP_EXE_BIN_ENABLE) \
	library_clean $(PROGRAM_CPP_EXAMPLE_ENABLE)

# One header build
one_header_build:
	cd $(PROGRAM_ONE_HEADER_DIR) && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH)
	
# One header use
one_header_use:
	$(SHARED_LIBRARY_HEADER_DEL_CMD)
	$(PROGRAM_ONE_HEADER_EXE_FINAL) $(LIBRARY_CPP_ONE_HEADER) $(LIBRARY_CPP_ONE_HEADER_COMBINE)
	
# Contribution Library Build
program_cpp_contrib: 
	$(foreach CONTRIB_ELEMENT,$(PROGRAM_CPP_CONTRIB_LIST), \
	cd $(LIBRARY_CONTRIB_DIR)/$(CONTRIB_ELEMENT) && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) \
	PROGRAM_ONE_HEADER_EXE=$(PROGRAM_ONE_HEADER_EXE) LIBRARY_CPP_ONE_HEADER_PARENT_FLAG=1 \
	LIBRARY_ONE_HEADER_COMBINE_DIR=$(LIBRARY_ONE_HEADER_COMBINE_DIR) \
	$(CLI_MULTIPLE) cd ../../$(CLI_MULTIPLE))

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
	$(GCC) $(LIBRARY_EXE_LINK_FLAGS) $(LIBRARY_EXE_LINK_LIB_PATH) -o $(LIBRARY_CPP_EXE_BIN) $^ $(LIBRARY_EXE_LINK_LIBS)

LIBRARY_CPP_SHARED_BIN: $(LIBRARY_OBJ_FILES)
	$(GCC) $(LIBRARY_SHARED_LINK_FLAGS) $(LIBRARY_SHARED_LINK_LIB_PATH) -o $(LIBRARY_CPP_SHARED_BIN) $^ $(LIBRARY_SHARED_LINK_LIBS)
	
LIBRARY_CPP_STATIC_BIN: $(LIBRARY_OBJ_FILES)
	$(AR) rcs $(LIBRARY_CPP_STATIC_BIN) $^ $(LIBRARY_STATIC_LINK_FLAGS)

$(LIBRARY_OBJ_DIR)/%.o: $(LIBRARY_SRC_DIR)/%.cpp
	$(GCC) $(LIBRARY_COMPILE_FLAGS) $(LIBRARY_COMPILE_INCLUDE_PATH) -c -o $@ $<
	
library_clean:
	$(LIBRARY_OBJ_DIR_DEL)

# Example programs
program_cpp_example:
	$(foreach CONTRIB_ELEMENT,$(PROGRAM_CPP_EXAMPLE_LIST), \
	cd $(LIBRARY_EXAMPLE_DIR)/$(CONTRIB_ELEMENT) && make HOST_OS=$(HOST_OS) TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) \
	$(CLI_MULTIPLE) cd ../../$(CLI_MULTIPLE))

clean:
	$(LIBRARY_TEMP_DIR_DEL)
	$(SHARED_LIBRARY_DEL_CMD)
	$(STATIC_LIBRARY_DEL_CMD)
