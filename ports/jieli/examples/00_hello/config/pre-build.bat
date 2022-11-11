::SDK路径设置环境变量、指定SDK路径、拷贝SDK内部所需文件到工程目录.


@echo off
echo curpath:%~dp0

SET SourceFile=..\build
SET SDK_PATH=..\fw-AC63_BT_SDK-master

if exist %SourceFile% (
	echo %SourceFile% is  exist!
) else (
    echo %SourceFile% is not exist!
	md %SourceFile%\cpu\bd19\config
	md %SourceFile%\cpu\bd19\tools\download
	copy %SDK_PATH%\apps\spp_and_le\board\bd19\board*.h %SourceFile%\cpu\bd19\config
	copy %SDK_PATH%\apps\spp_and_le\include\app_config.h %SourceFile%\cpu\bd19\config
	copy %SDK_PATH%\cpu\bd19\tools %SourceFile%\cpu\bd19\tools
	xcopy /S %SDK_PATH%\cpu\bd19\tools\download %SourceFile%\cpu\bd19\tools\download
)
