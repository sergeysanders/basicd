##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=basicd
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=/Users/sergey/projloc/rimer/basicd
ProjectPath            :=/Users/sergey/projloc/rimer/basicd
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Sergey Sanders
Date                   :=14/11/2022
CodeLitePath           :="/Users/sergey/Library/Application Support/CodeLite"
LinkerName             :=clang
SharedObjectLinkerName :=clang -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="basicd.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := ar rcus
CXX      := clang++
CC       := clang
CXXFLAGS :=  -gdwarf-2 -fstandalone-debug -O0 -Wall $(Preprocessors)
CFLAGS   :=  -gdwarf-2 -fstandalone-debug -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := llvm-as


##
## User defined environment variables
##
CodeLiteDir:=/Applications/codelite.app/Contents/SharedSupport/
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/rpn.c$(ObjectSuffix) $(IntermediateDirectory)/bcore.c$(ObjectSuffix) $(IntermediateDirectory)/banalizer.c$(ObjectSuffix) $(IntermediateDirectory)/bfunc.c$(ObjectSuffix) $(IntermediateDirectory)/bstring.c$(ObjectSuffix) $(IntermediateDirectory)/bprime.c$(ObjectSuffix) $(IntermediateDirectory)/berror.c$(ObjectSuffix) 



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
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)


$(IntermediateDirectory)/.d:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/rpn.c$(ObjectSuffix): rpn.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/rpn.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rpn.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rpn.c$(PreprocessSuffix): rpn.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rpn.c$(PreprocessSuffix) rpn.c

$(IntermediateDirectory)/bcore.c$(ObjectSuffix): bcore.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/bcore.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bcore.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bcore.c$(PreprocessSuffix): bcore.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bcore.c$(PreprocessSuffix) bcore.c

$(IntermediateDirectory)/banalizer.c$(ObjectSuffix): banalizer.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/banalizer.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/banalizer.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/banalizer.c$(PreprocessSuffix): banalizer.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/banalizer.c$(PreprocessSuffix) banalizer.c

$(IntermediateDirectory)/bfunc.c$(ObjectSuffix): bfunc.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/bfunc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bfunc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bfunc.c$(PreprocessSuffix): bfunc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bfunc.c$(PreprocessSuffix) bfunc.c

$(IntermediateDirectory)/bstring.c$(ObjectSuffix): bstring.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/bstring.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bstring.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bstring.c$(PreprocessSuffix): bstring.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bstring.c$(PreprocessSuffix) bstring.c

$(IntermediateDirectory)/bprime.c$(ObjectSuffix): bprime.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/bprime.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bprime.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bprime.c$(PreprocessSuffix): bprime.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bprime.c$(PreprocessSuffix) bprime.c

$(IntermediateDirectory)/berror.c$(ObjectSuffix): berror.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/rimer/basicd/berror.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/berror.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/berror.c$(PreprocessSuffix): berror.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/berror.c$(PreprocessSuffix) berror.c

##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


