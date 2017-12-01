##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=PreCompilerHandler
ConfigurationName      :=Debug
WorkspacePath          :=/home/fzm/confResource
ProjectPath            :=/home/fzm/confResource/PreCompilerHandler
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=fzm
Date                   :=01/12/17
CodeLitePath           :=/home/fzm/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="PreCompilerHandler.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lpthread
IncludePath            := $(IncludeSwitch)/usr/include/libxml2 $(IncludeSwitch)/usr/include/mysql/  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                :=$(LibraryPathSwitch)/usr/lib/x86_64-linux-gnu/  $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/config.c$(ObjectSuffix) $(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IntermediateDirectory)/logOper.c$(ObjectSuffix) $(IntermediateDirectory)/JsonOper.c$(ObjectSuffix) $(IntermediateDirectory)/PreCompileOper.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/config.c$(ObjectSuffix): config.c $(IntermediateDirectory)/config.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/config.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/config.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/config.c$(DependSuffix): config.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/config.c$(ObjectSuffix) -MF$(IntermediateDirectory)/config.c$(DependSuffix) -MM config.c

$(IntermediateDirectory)/config.c$(PreprocessSuffix): config.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/config.c$(PreprocessSuffix) config.c

$(IntermediateDirectory)/dateOper.c$(ObjectSuffix): dateOper.c $(IntermediateDirectory)/dateOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/dateOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dateOper.c$(DependSuffix): dateOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dateOper.c$(DependSuffix) -MM dateOper.c

$(IntermediateDirectory)/dateOper.c$(PreprocessSuffix): dateOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dateOper.c$(PreprocessSuffix) dateOper.c

$(IntermediateDirectory)/fileOper.c$(ObjectSuffix): fileOper.c $(IntermediateDirectory)/fileOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/fileOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileOper.c$(DependSuffix): fileOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fileOper.c$(DependSuffix) -MM fileOper.c

$(IntermediateDirectory)/fileOper.c$(PreprocessSuffix): fileOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fileOper.c$(PreprocessSuffix) fileOper.c

$(IntermediateDirectory)/strOper.c$(ObjectSuffix): strOper.c $(IntermediateDirectory)/strOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/strOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/strOper.c$(DependSuffix): strOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/strOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/strOper.c$(DependSuffix) -MM strOper.c

$(IntermediateDirectory)/strOper.c$(PreprocessSuffix): strOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/strOper.c$(PreprocessSuffix) strOper.c

$(IntermediateDirectory)/logOper.c$(ObjectSuffix): logOper.c $(IntermediateDirectory)/logOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/logOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/logOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/logOper.c$(DependSuffix): logOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/logOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/logOper.c$(DependSuffix) -MM logOper.c

$(IntermediateDirectory)/logOper.c$(PreprocessSuffix): logOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/logOper.c$(PreprocessSuffix) logOper.c

$(IntermediateDirectory)/JsonOper.c$(ObjectSuffix): JsonOper.c $(IntermediateDirectory)/JsonOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/JsonOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/JsonOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/JsonOper.c$(DependSuffix): JsonOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/JsonOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/JsonOper.c$(DependSuffix) -MM JsonOper.c

$(IntermediateDirectory)/JsonOper.c$(PreprocessSuffix): JsonOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/JsonOper.c$(PreprocessSuffix) JsonOper.c

$(IntermediateDirectory)/PreCompileOper.c$(ObjectSuffix): PreCompileOper.c $(IntermediateDirectory)/PreCompileOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/PreCompilerHandler/PreCompileOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PreCompileOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PreCompileOper.c$(DependSuffix): PreCompileOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PreCompileOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/PreCompileOper.c$(DependSuffix) -MM PreCompileOper.c

$(IntermediateDirectory)/PreCompileOper.c$(PreprocessSuffix): PreCompileOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PreCompileOper.c$(PreprocessSuffix) PreCompileOper.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


