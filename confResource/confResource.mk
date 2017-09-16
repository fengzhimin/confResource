##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=confResource
ConfigurationName      :=Debug
WorkspacePath          :=/home/fzm/confResource
ProjectPath            :=/home/fzm/confResource/confResource
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=fzm
Date                   :=16/09/17
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
ObjectsFileList        :="confResource.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lxml2 -lpthread -lmysqlclient
IncludePath            := $(IncludeSwitch)/usr/include/libxml2/ $(IncludeSwitch)/usr/include/mysql/  $(IncludeSwitch). $(IncludeSwitch). 
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
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/CXmlOper.c$(ObjectSuffix) $(IntermediateDirectory)/CPPXmlOper.c$(ObjectSuffix) $(IntermediateDirectory)/mysqlOper.c$(ObjectSuffix) $(IntermediateDirectory)/buildFuncLibrary.c$(ObjectSuffix) $(IntermediateDirectory)/buildTempTable.c$(ObjectSuffix) $(IntermediateDirectory)/confOper.c$(ObjectSuffix) $(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IntermediateDirectory)/dirOper.c$(ObjectSuffix) \
	$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IntermediateDirectory)/config.c$(ObjectSuffix) $(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) $(IntermediateDirectory)/logOper.c$(ObjectSuffix) 



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
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/CXmlOper.c$(ObjectSuffix): CXmlOper.c $(IntermediateDirectory)/CXmlOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/CXmlOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CXmlOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CXmlOper.c$(DependSuffix): CXmlOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CXmlOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/CXmlOper.c$(DependSuffix) -MM CXmlOper.c

$(IntermediateDirectory)/CXmlOper.c$(PreprocessSuffix): CXmlOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CXmlOper.c$(PreprocessSuffix) CXmlOper.c

$(IntermediateDirectory)/CPPXmlOper.c$(ObjectSuffix): CPPXmlOper.c $(IntermediateDirectory)/CPPXmlOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/CPPXmlOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CPPXmlOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CPPXmlOper.c$(DependSuffix): CPPXmlOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CPPXmlOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/CPPXmlOper.c$(DependSuffix) -MM CPPXmlOper.c

$(IntermediateDirectory)/CPPXmlOper.c$(PreprocessSuffix): CPPXmlOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CPPXmlOper.c$(PreprocessSuffix) CPPXmlOper.c

$(IntermediateDirectory)/mysqlOper.c$(ObjectSuffix): mysqlOper.c $(IntermediateDirectory)/mysqlOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/mysqlOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mysqlOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mysqlOper.c$(DependSuffix): mysqlOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mysqlOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/mysqlOper.c$(DependSuffix) -MM mysqlOper.c

$(IntermediateDirectory)/mysqlOper.c$(PreprocessSuffix): mysqlOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mysqlOper.c$(PreprocessSuffix) mysqlOper.c

$(IntermediateDirectory)/buildFuncLibrary.c$(ObjectSuffix): buildFuncLibrary.c $(IntermediateDirectory)/buildFuncLibrary.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/buildFuncLibrary.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/buildFuncLibrary.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/buildFuncLibrary.c$(DependSuffix): buildFuncLibrary.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/buildFuncLibrary.c$(ObjectSuffix) -MF$(IntermediateDirectory)/buildFuncLibrary.c$(DependSuffix) -MM buildFuncLibrary.c

$(IntermediateDirectory)/buildFuncLibrary.c$(PreprocessSuffix): buildFuncLibrary.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/buildFuncLibrary.c$(PreprocessSuffix) buildFuncLibrary.c

$(IntermediateDirectory)/buildTempTable.c$(ObjectSuffix): buildTempTable.c $(IntermediateDirectory)/buildTempTable.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/buildTempTable.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/buildTempTable.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/buildTempTable.c$(DependSuffix): buildTempTable.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/buildTempTable.c$(ObjectSuffix) -MF$(IntermediateDirectory)/buildTempTable.c$(DependSuffix) -MM buildTempTable.c

$(IntermediateDirectory)/buildTempTable.c$(PreprocessSuffix): buildTempTable.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/buildTempTable.c$(PreprocessSuffix) buildTempTable.c

$(IntermediateDirectory)/confOper.c$(ObjectSuffix): confOper.c $(IntermediateDirectory)/confOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/confOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/confOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/confOper.c$(DependSuffix): confOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/confOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/confOper.c$(DependSuffix) -MM confOper.c

$(IntermediateDirectory)/confOper.c$(PreprocessSuffix): confOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/confOper.c$(PreprocessSuffix) confOper.c

$(IntermediateDirectory)/fileOper.c$(ObjectSuffix): fileOper.c $(IntermediateDirectory)/fileOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/fileOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileOper.c$(DependSuffix): fileOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fileOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fileOper.c$(DependSuffix) -MM fileOper.c

$(IntermediateDirectory)/fileOper.c$(PreprocessSuffix): fileOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fileOper.c$(PreprocessSuffix) fileOper.c

$(IntermediateDirectory)/strOper.c$(ObjectSuffix): strOper.c $(IntermediateDirectory)/strOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/strOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/strOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/strOper.c$(DependSuffix): strOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/strOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/strOper.c$(DependSuffix) -MM strOper.c

$(IntermediateDirectory)/strOper.c$(PreprocessSuffix): strOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/strOper.c$(PreprocessSuffix) strOper.c

$(IntermediateDirectory)/dirOper.c$(ObjectSuffix): dirOper.c $(IntermediateDirectory)/dirOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/dirOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dirOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dirOper.c$(DependSuffix): dirOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dirOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dirOper.c$(DependSuffix) -MM dirOper.c

$(IntermediateDirectory)/dirOper.c$(PreprocessSuffix): dirOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dirOper.c$(PreprocessSuffix) dirOper.c

$(IntermediateDirectory)/dateOper.c$(ObjectSuffix): dateOper.c $(IntermediateDirectory)/dateOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/dateOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dateOper.c$(DependSuffix): dateOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dateOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dateOper.c$(DependSuffix) -MM dateOper.c

$(IntermediateDirectory)/dateOper.c$(PreprocessSuffix): dateOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dateOper.c$(PreprocessSuffix) dateOper.c

$(IntermediateDirectory)/config.c$(ObjectSuffix): config.c $(IntermediateDirectory)/config.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/config.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/config.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/config.c$(DependSuffix): config.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/config.c$(ObjectSuffix) -MF$(IntermediateDirectory)/config.c$(DependSuffix) -MM config.c

$(IntermediateDirectory)/config.c$(PreprocessSuffix): config.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/config.c$(PreprocessSuffix) config.c

$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix): sourceOper.c $(IntermediateDirectory)/sourceOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/sourceOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sourceOper.c$(DependSuffix): sourceOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sourceOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sourceOper.c$(DependSuffix) -MM sourceOper.c

$(IntermediateDirectory)/sourceOper.c$(PreprocessSuffix): sourceOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sourceOper.c$(PreprocessSuffix) sourceOper.c

$(IntermediateDirectory)/logOper.c$(ObjectSuffix): logOper.c $(IntermediateDirectory)/logOper.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/fzm/confResource/confResource/logOper.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/logOper.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/logOper.c$(DependSuffix): logOper.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/logOper.c$(ObjectSuffix) -MF$(IntermediateDirectory)/logOper.c$(DependSuffix) -MM logOper.c

$(IntermediateDirectory)/logOper.c$(PreprocessSuffix): logOper.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/logOper.c$(PreprocessSuffix) logOper.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


