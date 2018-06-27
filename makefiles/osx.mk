# Compiler and Dependency paths
# HOST_OS the host OS
# TARGET_OS the target OS
# TARGET_ARCH the target architecture
# COMPILER the host compiler
ifeq ($(HOST_OS),OSX)
	# PREPEND -I"
	# APPEND "
	LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = /opt/boost/include
	# PREPEND -L"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS = 
	# PREPEND -l"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS = 
	ifeq ($(COMPILER),CLANG)
		LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += /opt/boost/lib
		BOOST_LIBS_POST = -clang-darwin42-mt-x64-1_67
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
		LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += /opt/boost/lib/ios
		BOOST_LIBS_POST = 
		GCC = clang++
		AR = ar
		IPHONE_SDK_PATH = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
		ifeq ($(TARGET_ARCH),armv7)
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += lib/ios
			IOS_ARCH = armv7
		endif
		ifeq ($(TARGET_ARCH),armv7s)
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += lib/ios
			IOS_ARCH = armv7s
		endif
		ifeq ($(TARGET_ARCH),arm64)
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += lib/ios
			IOS_ARCH = arm64
		endif
	endif
	# program is same
	PROGRAM_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = $(LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_DIR_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_FILE_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS)
endif