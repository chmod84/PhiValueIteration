#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=icc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=ICC-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/communication_module.o \
	${OBJECTDIR}/echo.o \
	${OBJECTDIR}/input_file_module.o \
	${OBJECTDIR}/parallel_vi_module.o \
	${OBJECTDIR}/uthash/libut.o \
	${OBJECTDIR}/uthash/ringbuf.o \
	${OBJECTDIR}/uthash/utmm.o \
	${OBJECTDIR}/uthash/utvector.o \
	${OBJECTDIR}/utils.o \
	${OBJECTDIR}/vi_module.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/echo

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/echo: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/echo ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/communication_module.o: communication_module.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/communication_module.o communication_module.c

${OBJECTDIR}/echo.o: echo.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/echo.o echo.c

${OBJECTDIR}/input_file_module.o: input_file_module.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/input_file_module.o input_file_module.c

${OBJECTDIR}/parallel_vi_module.o: parallel_vi_module.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parallel_vi_module.o parallel_vi_module.c

${OBJECTDIR}/uthash/libut.o: uthash/libut.c 
	${MKDIR} -p ${OBJECTDIR}/uthash
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uthash/libut.o uthash/libut.c

${OBJECTDIR}/uthash/ringbuf.o: uthash/ringbuf.c 
	${MKDIR} -p ${OBJECTDIR}/uthash
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uthash/ringbuf.o uthash/ringbuf.c

${OBJECTDIR}/uthash/utmm.o: uthash/utmm.c 
	${MKDIR} -p ${OBJECTDIR}/uthash
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uthash/utmm.o uthash/utmm.c

${OBJECTDIR}/uthash/utvector.o: uthash/utvector.c 
	${MKDIR} -p ${OBJECTDIR}/uthash
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uthash/utvector.o uthash/utvector.c

${OBJECTDIR}/utils.o: utils.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils.o utils.c

${OBJECTDIR}/vi_module.o: vi_module.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/vi_module.o vi_module.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/echo

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
