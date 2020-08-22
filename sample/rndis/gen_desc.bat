echo off
set DESC_PATH=%cd%
set DESC_NAME=%1
if "%DESC_NAME%"=="" (set "DESC_NAME=rndis_desc.lua")
cd ..\..\TeenyDT
lua gen_descriptor.lua %DESC_PATH%\%DESC_NAME% %~2 %~3
cd %DESC_PATH%