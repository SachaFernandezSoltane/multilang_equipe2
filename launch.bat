@echo off
setlocal

REM Ouvre une fenêtre pour ssp-c
start "ssp-c" cmd /k docker run -v "%cd%/instances:/data" ssp-c /data/

REM Ouvre une fenêtre pour ssp-c#
start "ssp-cs" cmd /k docker run -v "%cd%/instances:/data" ssp-cs  /data/

REM Ouvre une fenêtre pour ssp-c++
start "ssp-cpp" cmd /k docker run -v "%cd%/instances:/data" ssp-cpp  /data/

REM Ouvre une fenêtre pour ssp-go
start "ssp-go" cmd /k docker run -v "%cd%/instances:/data" ssp-go  /data/

REM Ouvre une fenêtre pour ssp-java
start "ssp-java" cmd /k docker run -v "%cd%/instances:/data" ssp-java  /data/

REM Ouvre une fenêtre pour ssp-javascript
start "ssp-js" cmd /k docker run -v "%cd%/instances:/data" ssp-js  /data/

REM Ouvre une fenêtre pour ssp-kotlin
start "ssp-kt" cmd /k docker run -v "%cd%/instances:/data" ssp-kt  /data/

REM Ouvre une nouvelle fenêtre CMD, va dans le dossier scala, puis exécute scala-cli
cd /d "%~dp0scala"
start "scala cli" cmd /k scala-cli .\SSPApp.scala -- ../instances/

endlocal
