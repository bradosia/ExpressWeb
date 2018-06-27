# Compiler and Dependency paths
# HOST_OS the host OS
# TARGET_OS the target OS
# TARGET_ARCH the target architecture
# COMPILER the host compiler
ifeq ($(HOST_OS),WIN)
	# PREPEND -I"
	# APPEND "
	LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = C:/boost_1_67_0/include
	# PREPEND -L"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS = 
	# PREPEND -l"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS = ws2_32 wsock32
	ifeq ($(COMPILER),MINGW)
		LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += C:/boost_1_67_0/lib-mgw
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
		LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += C:/boost_1_67_0/lib-msvc
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
		NDK_PATH = C:/android-ndk-r16b-windows-x86_64/ndroid-ndk-r16b
		LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS += $(NDK_PATH)/sysroot/usr/include
		ifeq ($(TARGET_ARCH),armeabi)
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += C:/boost_1_64_0/armeabi/lib
			GCC = arm-linux-androideabi-g++
			AR = arm-linux-androideabi-ar
		endif
		ifeq ($(TARGET_ARCH),armeabi-v7a)
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += C:/boost_1_64_0/armeabi-v7a/lib
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
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += 
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
			LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS += C:/boost_1_64_0/x86/lib
			GCC = i686-linux-android-g++
			AR = i686-linux-android-ar
		endif
	endif
	# program is same
	PROGRAM_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = $(LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_DIR_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_FILE_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS)
endif