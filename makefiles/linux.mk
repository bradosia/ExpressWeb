# Compiler and Dependency paths
# HOST_OS the host OS
# TARGET_OS the target OS
# TARGET_ARCH the target architecture
# COMPILER the host compiler
ifeq ($(HOST_OS),LINUX)
	# PREPEND -I"
	# APPEND "
	LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = /usr/include /usr/local/include
	# PREPEND -L"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS = /usr/lib /usr/local/lib
	# PREPEND -l"
	# APPEND "
	LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS = 
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
	# program is same
	PROGRAM_CPP_COMPILE_INCLUDE_DIR_TARGET_OS = $(LIBRARY_CPP_COMPILE_INCLUDE_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_DIR_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_DIR_TARGET_OS)
	PROGRAM_CPP_LINK_LIB_FILE_TARGET_OS = $(LIBRARY_CPP_LINK_LIB_FILE_TARGET_OS)
endif