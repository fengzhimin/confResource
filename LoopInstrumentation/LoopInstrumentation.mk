##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=LoopInstrumentation
ConfigurationName      :=Debug
WorkspacePath          :=/home/fzm/github/confResource
ProjectPath            :=/home/fzm/github/confResource/LoopInstrumentation
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=fzm
Date                   :=27/10/18
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
ObjectsFileList        :="LoopInstrumentation.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lxml2 -lmysqlclient -lpthread
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
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/config.c$(ObjectSuffix) $(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IntermediateDirectory)/dirOper.c$(ObjectSuffix) $(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IntermediateDirectory)/logOper.c$(ObjectSuffix) $(IntermediateDirectory)/xmlOper.c$(ObjectSuffix) $(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) 



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
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/config.c$(ObjectSuffix): config.c $(IntermediateDirectory)/config.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/config.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/config.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/config.c$(DependSuffix): config.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/config.c$(ObjectSuffix) -MF$(IntermediateDirectory)/config.c$(DependSuffix) -MM config.c

$(IntermediateDirectory)/config.c$(PreprocessSuffix): config.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/config.c$(PreprocessSuffix) config.c

$(IntermediateDirectory)/dateOper.c$(ObjectSuffix): dateOper.c $(IntermediateDirectory)/dateOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/dateOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dateOper.c$(DependSuffix): dateOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dateOper.c$(DependSuffix) -MM dateOper.c

$(IntermediateDirectory)/dateOper.c$(PreprocessSuffix): dateOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dateOper.c$(PreprocessSuffix) dateOper.c

$(IntermediateDirectory)/dirOper.c$(ObjectSuffix): dirOper.c $(IntermediateDirectory)/dirOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/dirOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dirOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dirOper.c$(DependSuffix): dirOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dirOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dirOper.c$(DependSuffix) -MM dirOper.c

$(IntermediateDirectory)/dirOper.c$(PreprocessSuffix): dirOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dirOper.c$(PreprocessSuffix) dirOper.c

$(IntermediateDirectory)/fileOper.c$(ObjectSuffix): fileOper.c $(IntermediateDirectory)/fileOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/fileOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileOper.c$(DependSuffix): fileOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fileOper.c$(DependSuffix) -MM fileOper.c

$(IntermediateDirectory)/fileOper.c$(PreprocessSuffix): fileOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fileOper.c$(PreprocessSuffix) fileOper.c

$(IntermediateDirectory)/strOper.c$(ObjectSuffix): strOper.c $(IntermediateDirectory)/strOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/strOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/strOper.c$(DependSuffix): strOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/strOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/strOper.c$(DependSuffix) -MM strOper.c

$(IntermediateDirectory)/strOper.c$(PreprocessSuffix): strOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/strOper.c$(PreprocessSuffix) strOper.c

$(IntermediateDirectory)/logOper.c$(ObjectSuffix): logOper.c $(IntermediateDirectory)/logOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/logOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/logOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/logOper.c$(DependSuffix): logOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/logOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/logOper.c$(DependSuffix) -MM logOper.c

$(IntermediateDirectory)/logOper.c$(PreprocessSuffix): logOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/logOper.c$(PreprocessSuffix) logOper.c

$(IntermediateDirectory)/xmlOper.c$(ObjectSuffix): xmlOper.c $(IntermediateDirectory)/xmlOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/xmlOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/xmlOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/xmlOper.c$(DependSuffix): xmlOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/xmlOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/xmlOper.c$(DependSuffix) -MM xmlOper.c

$(IntermediateDirectory)/xmlOper.c$(PreprocessSuffix): xmlOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/xmlOper.c$(PreprocessSuffix) xmlOper.c

$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix): sourceOper.c $(IntermediateDirectory)/sourceOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/github/confResource/LoopInstrumentation/sourceOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sourceOper.c$(DependSuffix): sourceOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sourceOper.c$(DependSuffix) -MM sourceOper.c

$(IntermediateDirectory)/sourceOper.c$(PreprocessSuffix): sourceOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sourceOper.c$(PreprocessSuffix) sourceOper.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


