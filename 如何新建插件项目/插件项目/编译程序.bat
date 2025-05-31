@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 初始化配置.
set "HISTORY_FILE=编译设定.cfg"
set "DEFAULT_DEBUG=2"
set "DEFAULT_COMPILER=%~dp0..\amxxpc.exe"
for %%I in ("!DEFAULT_COMPILER!") do set "DEFAULT_COMPILER=%%~fI"
set "DEFAULT_OUTPUT=%~dp0..\..\plugins"
for %%I in ("!DEFAULT_OUTPUT!") do set "DEFAULT_OUTPUT=%%~fI"
set "SELECTED_FILES=*.sma"
set "SELECTED_NAMES=*.sma"

:: 读取历史记录.
if exist "%HISTORY_FILE%" (
    for /f "tokens=1,* delims=: " %%a in ('type "%HISTORY_FILE%" 2^>nul') do (
        if "%%a"=="debug" set "DEBUG=%%b"
        if "%%a"=="compiler" set "COMPILER=%%b"
        if "%%a"=="output" set "OUTPUT_DIR=%%b"
        if "%%a"=="include" (
            if defined INCLUDE_DIRS (
                set "INCLUDE_DIRS=!INCLUDE_DIRS! "%%~b""
            ) else (
                set "INCLUDE_DIRS="%%~b""
            )
        )
    )
    :: 确保关键变量有值(即使配置文件缺少对应行) 
    if not defined COMPILER set "COMPILER=%DEFAULT_COMPILER%"
    if not defined OUTPUT_DIR set "OUTPUT_DIR=%DEFAULT_OUTPUT%"
    if not defined DEBUG set "DEBUG=%DEFAULT_DEBUG%"
) else (
    call :SET_DEFAULTS
)

:: 检查包含目录(不能以\或/结尾.解析器会识别到下一级).
if not defined INCLUDE_DIRS (
    set "INCLUDE_DIRS="
    for %%D in (
        "%~dp0include"
    ) do (
        set "NEW_DIR=%%~D"
        for %%I in ("!NEW_DIR!") do (
            if exist "%%~fI\" (
                if defined INCLUDE_DIRS (
                    set "INCLUDE_DIRS=!INCLUDE_DIRS! "%%~fI""
                ) else (
                    set "INCLUDE_DIRS="%%~fI""
                )
            )
        )
    )
)

:: 主菜单循环. 
:MAIN_LOOP
cls
echo [AMXX 编译工具] 
echo. 
echo 1. 开始编译 
echo. 
echo 2. 退出 
echo. 
echo 3. 恢复默认设置  
echo. 
echo 4. 更改调试级别
if %DEBUG% equ 0 (
echo    0: 优化代码+禁止调试+删除断言+不打印内存占用信息 
) else if %DEBUG% equ 1 (
echo    1: 优化代码+禁止调试+保留断言+打印内存占用信息 
) else if %DEBUG% equ 2 (
echo    2: 优化代码+允许调试+保留断言+打印内存占用信息 [默认] 
) else if %DEBUG% equ 3 (
echo    3: 不优化代码+允许调试+保留断言+打印内存占用信息 
)
echo. 
echo 5. 更改编译器位置 
echo    %COMPILER%
echo. 
echo 6. 额外引用目录
for %%F in (%INCLUDE_DIRS%) do (
    echo    %%~F
)
echo. 
echo 7. 更改输出插件位置 
echo    %OUTPUT_DIR%
echo. 
echo 8. 选择源码文件 
for %%F in (%SELECTED_NAMES%) do (
    echo    %%~F
)
echo.

:: 输入处理 
where choice >nul 2>&1
if %errorlevel% equ 0 (
    choice /C 12345678 /N /M "请选择操作: "
    set "CHOICE=!errorlevel!"
) else (
    call :LEGACY_INPUT 
)

:: 处理选择 
if "%CHOICE%"=="1" goto :COMPILE
if "%CHOICE%"=="2" exit /b
if "%CHOICE%"=="3" goto :SET_DEFAULTS
if "%CHOICE%"=="4" goto :SET_DEBUG
if "%CHOICE%"=="5" goto :SET_COMPILER
if "%CHOICE%"=="6" goto :EDIT_INCLUDE
if "%CHOICE%"=="7" goto :SET_OUTPUT
if "%CHOICE%"=="8" goto :SELECT_FILES
goto MAIN_LOOP

:: 传统输入模式 
:LEGACY_INPUT 
:RETRY 
set /p "CHOICE=请选择操作: "
echo !CHOICE!|findstr /r "^[0-8]$" >nul
if %errorlevel% neq 0 (
    echo 请输入0至8范围内的值 
    goto :RETRY
)
goto :eof

:: 编译主流程 
:COMPILE
if not exist "%COMPILER%" (
    echo 编译器位置错误 
    pause
    goto MAIN_LOOP
)
if not exist "%OUTPUT_DIR%\" (
    echo 输出插件位置错误 
    pause
    goto MAIN_LOOP
)
if %DEBUG% lss 0 (
    echo 调试级别错误 
    pause
    goto MAIN_LOOP
)
if %DEBUG% gtr 3 (
    echo 调试级别错误 
    pause
    goto MAIN_LOOP
)

:: 设置代码页并启用延迟扩展 
setlocal enabledelayedexpansion
for /f "tokens=2 delims=:." %%c in ('chcp') do set /a "OLD_CP=%%c"
chcp 65001 >nul

:: 开始编译 
set "ERROR_FLAG=0"
for %%f in (%SELECTED_FILES%) do (
    set "full_path=%%~ff"  :: 确保使用完整路径 
    set "file_name=%%~nf"
    set "OUT_FILE=!OUTPUT_DIR!\!file_name!.amxx"
    
    echo.
    echo 正在编译: %%~nxf
    
    :: 构建包含目录参数 
    set "INCLUDE_PARAM="
    if defined INCLUDE_DIRS (
        for %%d in (!INCLUDE_DIRS!) do (
            set "INCLUDE_PARAM=!INCLUDE_PARAM! "-i%%~d""
        )
    )
    
    echo.
    REM 根据解析器要求,包含目录格式必须是"-iC\include"不能舍去双引号,双引号内部不能嵌套 
    REM 根据解析器要求,输出位置格式必须是"-oC\plugin"不能舍去双引号,双引号内部不能嵌套 
    REM 根据解析器要求,调试级别格式必须是"-d2"不能舍去双引号,双引号内部不能嵌套 
    echo "!COMPILER!" "!full_path!"!INCLUDE_PARAM! "-o!OUT_FILE!" "-d!DEBUG!"
    echo.
    
    "!COMPILER!" "!full_path!"!INCLUDE_PARAM! "-o!OUT_FILE!" "-d!DEBUG!"
    
    echo ----------------------------------------
    echo.
    
    if !errorlevel! neq 0 (
        set "ERROR_FLAG=1"
    )
)

:: 保存错误标志并恢复原始代码页 
set "TEMPLG_ERROR=!ERROR_FLAG!"
chcp !OLD_CP! >nul
endlocal & set "ERROR_FLAG=%TEMPLG_ERROR%"

if %ERROR_FLAG% equ 1 (
    echo 部分文件编译失败!
) else (
    echo 全部编译完成!
)
pause
goto MAIN_LOOP

:: 恢复默认设置 
:SET_DEFAULTS
set "COMPILER=%DEFAULT_COMPILER%"
set "OUTPUT_DIR=%DEFAULT_OUTPUT%"
set "DEBUG=%DEFAULT_DEBUG%"
set "INCLUDE_DIRS="
for %%D in (
    "%~dp0include"
) do (
    set "NEW_DIR=%%~D"
    for %%I in ("!NEW_DIR!") do (
        if exist "%%~fI\" (
            if defined INCLUDE_DIRS (
                set "INCLUDE_DIRS=!INCLUDE_DIRS! "%%~fI""
            ) else (
                set "INCLUDE_DIRS="%%~fI""
            )
        )
    )
)
call :SAVE_HISTORY
goto MAIN_LOOP

:: 更改调试级别 
:SET_DEBUG
echo.
echo 调试级别选择: 
echo 0: 优化代码+禁止调试+删除断言+不打印内存占用信息 
echo 1: 优化代码+禁止调试+保留断言+打印内存占用信息 
echo 2: 优化代码+允许调试+保留断言+打印内存占用信息 [默认] 
echo 3: 不优化代码+允许调试+保留断言+打印内存占用信息 
:DEBUG_LOOP
where choice >nul 2>&1
if %errorlevel% equ 0 (
    choice /C 0123 /N /M "请选择调试级别(0~3): "
    set "NEW_DBG=!errorlevel!"
    set /a "NEW_DBG -=1"
    if !NEW_DBG! geq 0 if !NEW_DBG! leq 3 (
        set "DEBUG=!NEW_DBG!"
        call :SAVE_HISTORY
        goto MAIN_LOOP
    )
    echo 无效的调试级别:!NEW_DBG!
    goto DEBUG_LOOP
) else (
    :RETRY_DEBUG
    set /p "NEW_DBG=请填写调试级别(0~3): "
    echo !NEW_DBG!|findstr /r "^[0-3]$" >nul
    if errorlevel 1 (
        echo 无效的调试级别:!NEW_DBG!
        goto RETRY_DEBUG
    )
    set "DEBUG=!NEW_DBG!"
    call :SAVE_HISTORY
    goto MAIN_LOOP
)

:: 更改编译器位置 
:SET_COMPILER
echo.
echo 编译器位置示例:"C:\Half-Life\cstrike\addons\amxmodx\scripting\amxxpc.exe"
echo 留空则使用默认设定:"%DEFAULT_COMPILER%"
set /p "NEW_COMP=请填写编译器位置: "
if "!NEW_COMP!"=="" (
    set "COMPILER=%DEFAULT_COMPILER%"
    call :SAVE_HISTORY
    goto MAIN_LOOP
)
for %%I in ("!NEW_COMP!") do (
    if exist "%%~fI" (
        set "COMPILER=%%~fI"
        call :SAVE_HISTORY
    ) else (
        echo 无效的编译器位置:
        echo "!NEW_COMP!"
        set "NEW_COMP="
        pause
    )
)
goto MAIN_LOOP

:: 额外引用目录
:EDIT_INCLUDE
cls
echo [额外引用目录]
echo 当前引用目录: %INCLUDE_DIRS%
echo.
echo 1. 增加引用目录
echo 2. 删除引用目录
echo 3. 返回主菜单
echo.

where choice >nul 2>&1
if %errorlevel% equ 0 (
    choice /C 123 /N /M "请选择操作: "
    set "SUB_CHOICE=!errorlevel!"
) else (
    :RETRY_SUB
    set /p "SUB_CHOICE=请选择操作(1-3): "
    echo !SUB_CHOICE!|findstr /r "^[1-3]$" >nul
    if errorlevel 1 (
        echo 无效的选择
        goto RETRY_SUB
    )
)

if "!SUB_CHOICE!"=="1" goto :ADD_INCLUDE
if "!SUB_CHOICE!"=="2" goto :DEL_INCLUDE
if "!SUB_CHOICE!"=="3" goto MAIN_LOOP

:ADD_INCLUDE
echo.
echo 引用目录示例:"C:\Half-Life\cstrike\addons\amxmodx\scripting\include"
echo 支持相对路径(如:..\include),将自动转换为绝对路径
echo 留空取消操作
set /p "NEW_INC=请输入要添加的引用目录: "
if "!NEW_INC!"=="" goto EDIT_INCLUDE

:: 转换为绝对路径
for %%I in ("!NEW_INC!") do (
    if exist "%%~fI\" (
        set "NEW_INC=%%~fI"
        :: 检查是否已存在
        set "EXIST_FLAG=0"
        if defined INCLUDE_DIRS (
            for %%d in (!INCLUDE_DIRS!) do (
                if /i "%%~d"=="!NEW_INC!" set "EXIST_FLAG=1"
            )
        )
        if !EXIST_FLAG! equ 0 (
            if defined INCLUDE_DIRS (
                set "INCLUDE_DIRS=!INCLUDE_DIRS! "!NEW_INC!""
            ) else (
                set "INCLUDE_DIRS="!NEW_INC!""
            )
            call :SAVE_HISTORY
        ) else (
            echo 该目录已存在: "!NEW_INC!"
            pause
        )
    ) else (
        echo 目录不存在: "!NEW_INC!"
        pause
    )
)
goto EDIT_INCLUDE

:DEL_INCLUDE
if not defined INCLUDE_DIRS (
    echo 当前没有可删除的引用目录!
    pause
    goto EDIT_INCLUDE
)

cls
echo [删除引用目录]
echo 当前引用目录列表:
setlocal enabledelayedexpansion
set "COUNT=0"
for %%d in (%INCLUDE_DIRS%) do (
    set /a COUNT+=1
    set "INC_!COUNT!=%%~d"
    echo !COUNT!. %%~d
)
if !COUNT! equ 0 (
    echo 没有引用目录可删除!
    endlocal
    pause
    goto EDIT_INCLUDE
)

echo.
echo 根据编号选择要删除的目录
echo 可填写多个编号(用空格分隔),以此删除多个目录
echo 留空取消操作
set /p "DEL_NUM=请输入要删除的目录编号: "
if "!DEL_NUM!"=="" (
    endlocal
    goto EDIT_INCLUDE
)

:: 处理删除操作
set "NEW_DIRS="
for /l %%i in (1,1,!COUNT!) do (
    set "KEEP=1"
    for %%n in (!DEL_NUM!) do (
        if %%i equ %%n set "KEEP=0"
    )
    if !KEEP! equ 1 (
        if defined NEW_DIRS (
            set "NEW_DIRS=!NEW_DIRS! "!INC_%%i!""
        ) else (
            set "NEW_DIRS="!INC_%%i!""
        )
    )
)

endlocal & set "INCLUDE_DIRS=%NEW_DIRS%"
call :SAVE_HISTORY
goto EDIT_INCLUDE

:: 更改输出插件位置 
:SET_OUTPUT
echo.
echo 输出插件位置示例:"C:\Half-Life\cstrike\addons\amxmodx\plugins"
echo 留空则使用默认设定:"%OUTPUT_DIR%"
set "NEW_OUT="
set /p "NEW_OUT=请填写输出插件位置: "
if "!NEW_OUT!"=="" (
    set "OUTPUT_DIR=%DEFAULT_OUTPUT%"
    call :SAVE_HISTORY
    goto MAIN_LOOP
)
for %%I in ("!NEW_OUT!") do (
    if exist "%%~fI\" (
        set "OUTPUT_DIR=%%~fI"
        call :SAVE_HISTORY
    ) else (
        echo 无效的输出插件位置:
        echo "!NEW_OUT!"
        set "NEW_OUT="
        pause
    )
)
goto MAIN_LOOP

:: 选择源码文件 
:SELECT_FILES
echo.
echo 可选择需要编译的源码文件:
setlocal enabledelayedexpansion
set "COUNT=0"
for %%f in (*.sma) do (
    set /a COUNT+=1
    set "FILE_!COUNT!=%%~dpfnxf"
    set "FILE_NAME_!COUNT!=%%~nxf"
    echo !COUNT!. %%~nxf
)
if !COUNT! equ 0 (
    echo 未找到.sma文件!
    endlocal
    pause
    goto MAIN_LOOP
)
:SELECT_LOOP
echo.
echo 根据文件编号选择.
echo 可填写多个编号(用空格分隔),以此选中多个文件.
echo 留空表示选择全部文件.
set /p "NUM=请填写文件编号: "
if "!NUM!"=="" (
    endlocal & set "SELECTED_FILES=*.sma" & set "SELECTED_NAMES=*.sma"
    goto MAIN_LOOP
)
set "SELECTED="
set "NAMES="
for %%a in (!NUM!) do (
    if defined FILE_%%a (
        set "file=!FILE_%%a!"
        set "SELECTED=!SELECTED! "!file!""
        set "NAMES=!NAMES! !FILE_NAME_%%a!"
    ) else (
        echo 无效编号: %%a
        goto SELECT_LOOP
    )
)
:SELECT_END
endlocal & set "SELECTED_FILES=%SELECTED%" & set "SELECTED_NAMES=%NAMES%"
goto MAIN_LOOP

:: 保存配置 
:SAVE_HISTORY
(
    echo compiler: %COMPILER%
    echo output: %OUTPUT_DIR%
    echo debug: %DEBUG%
    if defined INCLUDE_DIRS (
        for %%d in (%INCLUDE_DIRS%) do echo include: %%~d
    )
) > "%HISTORY_FILE%"
goto :eof