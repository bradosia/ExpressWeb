# Name of the libraries and executable
LIBRARY_CPP_NAME = example
# Compiler and Dependency paths
# HOST_OS the host OS
# TARGET_OS the target OS
# TARGET_ARCH the target architecture
# COMPILER the host compiler
ifeq ($(HOST_OS),WIN)
	# platform specific include path
	# -I will be automatically prepended
	LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM =
	# platform specific library path
	# -L will be automatically prepended
	LIBRARY_CPP_LINK_LIB_PATH_PLATFORM = 
	# platform specific libraries
	# -l will be automatically prepended
	LIBRARY_CPP_LINK_LIB_PLATFORM = ws2_32
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
			# the NDK_PATH directory will be prepended to each item on this list 
			# -L will be automatically prepended
			# -L"$(NDK_PATH)%"
			LIBRARY_CPP_LINK_LIB_PATHS_ANDROID = /sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include \
				/sources/cxx-stl/gnu-libstdc++/4.9/include \
				/sysroot/usr/include \
				/sysroot/usr/include/arm-linux-androideabi
			NDK_SYSROOT_PATH = $(NDK_PATH)/platforms/android-21/arch-arm
		endif
		ifeq ($(TARGET_ARCH),arm64-v8a)
			BOOST_LIBS_DIR = 
			GCC = aarch64-linux-android-g++
			AR = aarch64-linux-android-ar
			# the NDK_PATH directory will be prepended to each item on this list 
			# -L will be automatically prepended
			# -L"$(NDK_PATH)%"
			LIBRARY_CPP_LINK_LIB_PATHS_ANDROID = /sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v8a/include \
				/sources/cxx-stl/gnu-libstdc++/4.9/include \
				/sysroot/usr/include \
				/sysroot/usr/include/arm-linux-androideabi
			NDK_SYSROOT_PATH = $(NDK_PATH)/platforms/android-21/arch-arm64
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
		GCC = clang++
		AR = ar
		IPHONE_SDK_PATH = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
		ifeq ($(TARGET_ARCH),armv7)
			IPHONE_SDK_LIBS_DIR = lib/ios
			IOS_ARCH = armv7
		endif
		ifeq ($(TARGET_ARCH),armv7s)
			IPHONE_SDK_LIBS_DIR = lib/ios
			IOS_ARCH = armv7s
		endif
		ifeq ($(TARGET_ARCH),arm64)
			IPHONE_SDK_LIBS_DIR = lib/ios
			IOS_ARCH = arm64
		endif
	endif
endif
ifeq ($(HOST_OS),LINUX)
	# platform specific include path
	# -I will be automatically prepended
	LIBRARY_CPP_COMPILE_INCLUDE_PATH_PLATFORM = /usr/include
	# platform specific library path
	# -L will be automatically prepended
	LIBRARY_CPP_LINK_LIB_PATH_PLATFORM = /usr/lib
	# platform specific libraries
	# -l will be automatically prepended
	LIBRARY_CPP_LINK_LIB_PLATFORM = 
	BOOST_INCLUDE_DIR = /usr/local/include
	BOOST_LIBS_DIR = /usr/local/lib
	ifeq ($(COMPILER),GCC)
		GCC = g++
		AR = ar 
		ifeq ($(TARGET_ARCH),x86)
			BOOST_LIBS_POST = -gcc49-mt-x32-1_67
		endif
		ifeq ($(TARGET_ARCH),x86_64)
			BOOST_LIBS_POST = -gcc49-mt-x64-1_67
		endif
	endif
endif
PROGRAM_CPP_CONTRIB_LIST = 
PROGRAM_CPP_EXAMPLE_LIST = 
LIBRARY_CONTRIB_PARENT_DIR = ../..
LIBRARY_EXAMPLE_PARENT_DIR = ../..
# -L will be automatically prepended
LIBRARY_CPP_LINK_LIB_PATHS = 
# the version directory will be appended to each item on this list
# -L will be automatically prepended
LIBRARY_CPP_LINK_LIB_PATHS_VERSION = $(LIBRARY_CONTRIB_PARENT_DIR)/lib
# -l will be automatically prepended
LIBRARY_CPP_LINK_LIBS = ssl crypto
# the boost version will be appended to each item on this list
# -l will be automatically prepended
LIBRARY_CPP_LINK_LIBS_VERSION_BOOST = boost_filesystem boost_thread boost_locale boost_system boost_iostreams

# -L will be automatically prepended
PROGRAM_CPP_LINK_LIB_PATHS = 
# the version directory will be appended to each item on this list
# -L will be automatically prepended
PROGRAM_CPP_LINK_LIB_PATHS_VERSION = 
# -l will be automatically prepended
PROGRAM_CPP_LINK_LIBS = 

# Use OneHeader utility to create a single include header
PROGRAM_ONE_HEADER_FLAG=0
# Make Static Library
LIBRARY_CPP_STATIC_BIN_FLAG=0
# Make Shared Library
LIBRARY_CPP_SHARED_BIN_FLAG=0
# Make Executable
LIBRARY_CPP_EXE_BIN_FLAG=1
# Make contributed libraries (typically they are dependencies too)
PROGRAM_CPP_CONTRIB_FLAG=0
# Make example programs
PROGRAM_CPP_EXAMPLE_FLAG=0