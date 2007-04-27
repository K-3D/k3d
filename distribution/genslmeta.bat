:: Check fie name was passed
IF [%1]==[] GOTO E_FLAG

:: pass shader file through preprocessor then pass results to sl2xml 
cpp.exe -E -P -x c %1 | k3d-sl2xml.exe > %1.slmeta
EXIT

:E_FLAG
ECHO. shader name missing
ECHO. 

