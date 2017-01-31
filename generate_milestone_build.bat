@echo off
echo [Converting fbx models...]
cd
cd Tools
FbxConverter.exe ..\Game\Assets\Mesh\*.* ..\Game\Assets\Animation\*.*
cd ..
rmdir ..\AutomatonAssault_Build /s /q
mkdir ..\AutomatonAssault_Build
copy x64\Release\Game.exe ..\AutomatonAssault_Build\
copy Dependencies\Wwise\bin\x64\AkSoundEngineDLL.dll ..\AutomatonAssault_Build\
echo
echo [Copying assets to build folder...]
xcopy Game\HLSL\*.* ..\AutomatonAssault_Build\HLSL\ /s
(echo .fbx & echo .fbm) > tempext
xcopy Game\Assets\*.* ..\AutomatonAssault_Build\Assets\ /s /exclude:tempext
del tempext
pause
