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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1457788035/io.o \
	${OBJECTDIR}/_ext/1457788035/http.o \
	${OBJECTDIR}/_ext/1457788035/analysis.o \
	${OBJECTDIR}/_ext/1457788035/mempool.o \
	${OBJECTDIR}/_ext/1457788035/misc.o \
	${OBJECTDIR}/_ext/1457788035/hash.o \
	${OBJECTDIR}/_ext/1457788035/request.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/1457788035/cgi.o


# C Compiler Flags
CFLAGS=-std=c99

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lws2_32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/web_server.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/web_server.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/web_server ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1457788035/io.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/io.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/io.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/io.c

${OBJECTDIR}/_ext/1457788035/http.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/http.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/http.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/http.c

${OBJECTDIR}/_ext/1457788035/analysis.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/analysis.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/analysis.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/analysis.c

${OBJECTDIR}/_ext/1457788035/mempool.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/mempool.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/mempool.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/mempool.c

${OBJECTDIR}/_ext/1457788035/misc.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/misc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/misc.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/misc.c

${OBJECTDIR}/_ext/1457788035/hash.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/hash.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/hash.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/hash.c

${OBJECTDIR}/_ext/1457788035/request.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/request.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/request.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/request.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/_ext/1457788035/cgi.o: /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/cgi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1457788035
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1457788035/cgi.o /F/Dropbox/Dropbox/Computer\ Networks\ and\ the\ Internet/Web_Server/lib/cgi.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/web_server.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
