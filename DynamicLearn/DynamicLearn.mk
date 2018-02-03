##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=DynamicLearn
ConfigurationName      :=Debug
WorkspacePath          :=/home/fzm/confResource
ProjectPath            :=/home/fzm/confResource/DynamicLearn
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=fzm
Date                   :=02/02/18
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
ObjectsFileList        :="DynamicLearn.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
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
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/sample.cpp$(ObjectSuffix) $(IntermediateDirectory)/SrcConfOpt.cpp$(ObjectSuffix) $(IntermediateDirectory)/config.cpp$(ObjectSuffix) $(IntermediateDirectory)/OWSample.cpp$(ObjectSuffix) $(IntermediateDirectory)/PWSample.cpp$(ObjectSuffix) $(IntermediateDirectory)/nOWSample.cpp$(ObjectSuffix) $(IntermediateDirectory)/PBSample.cpp$(ObjectSuffix) $(IntermediateDirectory)/RDSample.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/sample.cpp$(ObjectSuffix): sample.cpp $(IntermediateDirectory)/sample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/sample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sample.cpp$(DependSuffix): sample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sample.cpp$(DependSuffix) -MM sample.cpp

$(IntermediateDirectory)/sample.cpp$(PreprocessSuffix): sample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sample.cpp$(PreprocessSuffix) sample.cpp

$(IntermediateDirectory)/SrcConfOpt.cpp$(ObjectSuffix): SrcConfOpt.cpp $(IntermediateDirectory)/SrcConfOpt.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/SrcConfOpt.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SrcConfOpt.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SrcConfOpt.cpp$(DependSuffix): SrcConfOpt.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SrcConfOpt.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SrcConfOpt.cpp$(DependSuffix) -MM SrcConfOpt.cpp

$(IntermediateDirectory)/SrcConfOpt.cpp$(PreprocessSuffix): SrcConfOpt.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SrcConfOpt.cpp$(PreprocessSuffix) SrcConfOpt.cpp

$(IntermediateDirectory)/config.cpp$(ObjectSuffix): config.cpp $(IntermediateDirectory)/config.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/config.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/config.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/config.cpp$(DependSuffix): config.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/config.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/config.cpp$(DependSuffix) -MM config.cpp

$(IntermediateDirectory)/config.cpp$(PreprocessSuffix): config.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/config.cpp$(PreprocessSuffix) config.cpp

$(IntermediateDirectory)/OWSample.cpp$(ObjectSuffix): OWSample.cpp $(IntermediateDirectory)/OWSample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/OWSample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OWSample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OWSample.cpp$(DependSuffix): OWSample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OWSample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OWSample.cpp$(DependSuffix) -MM OWSample.cpp

$(IntermediateDirectory)/OWSample.cpp$(PreprocessSuffix): OWSample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OWSample.cpp$(PreprocessSuffix) OWSample.cpp

$(IntermediateDirectory)/PWSample.cpp$(ObjectSuffix): PWSample.cpp $(IntermediateDirectory)/PWSample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/PWSample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PWSample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PWSample.cpp$(DependSuffix): PWSample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PWSample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PWSample.cpp$(DependSuffix) -MM PWSample.cpp

$(IntermediateDirectory)/PWSample.cpp$(PreprocessSuffix): PWSample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PWSample.cpp$(PreprocessSuffix) PWSample.cpp

$(IntermediateDirectory)/nOWSample.cpp$(ObjectSuffix): nOWSample.cpp $(IntermediateDirectory)/nOWSample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/nOWSample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/nOWSample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/nOWSample.cpp$(DependSuffix): nOWSample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/nOWSample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/nOWSample.cpp$(DependSuffix) -MM nOWSample.cpp

$(IntermediateDirectory)/nOWSample.cpp$(PreprocessSuffix): nOWSample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/nOWSample.cpp$(PreprocessSuffix) nOWSample.cpp

$(IntermediateDirectory)/PBSample.cpp$(ObjectSuffix): PBSample.cpp $(IntermediateDirectory)/PBSample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/PBSample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PBSample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PBSample.cpp$(DependSuffix): PBSample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PBSample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PBSample.cpp$(DependSuffix) -MM PBSample.cpp

$(IntermediateDirectory)/PBSample.cpp$(PreprocessSuffix): PBSample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PBSample.cpp$(PreprocessSuffix) PBSample.cpp

$(IntermediateDirectory)/RDSample.cpp$(ObjectSuffix): RDSample.cpp $(IntermediateDirectory)/RDSample.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/fzm/confResource/DynamicLearn/RDSample.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RDSample.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RDSample.cpp$(DependSuffix): RDSample.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/RDSample.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/RDSample.cpp$(DependSuffix) -MM RDSample.cpp

$(IntermediateDirectory)/RDSample.cpp$(PreprocessSuffix): RDSample.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/RDSample.cpp$(PreprocessSuffix) RDSample.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


