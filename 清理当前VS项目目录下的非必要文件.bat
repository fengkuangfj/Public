@echo off

if not ""=="%1" (
cd /d "%1"
)

del /f /s /q /a *.aps 2>nul
del /f /s /q /a *.backup 2>nul
del /f /s /q /a *.cache 2>nul
del /f /s /q /a *.db 2>nul
del /f /s /q /a *.db-journal 2>nul
del /f /s /q /a *.dep 2>nul
del /f /s /q /a *.err 2>nul
del /f /s /q /a *.error 2>nul
rem del /f /s /q /a *.exp 2>nul
del /f /s /q /a *.htm 2>nul
del /f /s /q /a *.idb 2>nul
del /f /s /q /a *.ilk 2>nul
del /f /s /q /a *.iobj 2>nul
del /f /s /q /a *.ipch 2>nul
del /f /s /q /a *.ipdb 2>nul
del /f /s /q /a *.lastbuildstatus 2>nul
del /f /s /q /a *.lnk 2>nul
del /f /s /q /a *.log 2>nul
del /f /s /q /a *.mac 2>nul
rem del /f /s /q /a *.manifest 2>nul
del /f /s /q /a *.ncb 2>nul
del /f /s /q /a *.obj 2>nul
del /f /s /q /a *.opensdf 2>nul
del /f /s /q /a *.pch 2>nul
del /f /s /q /a *.pdb 2>nul
del /f /s /q /a *.pml 2>nul
del /f /s /q /a *.res 2>nul
rem del /f /s /q /a *.scc 2>nul
del /f /s /q /a *.sdf 2>nul
del /f /s /q /a *.suo 2>nul
del /f /s /q /a *.svn-bast 2>nul
del /f /s /q /a *.tlb 2>nul
del /f /s /q /a *.tlog 2>nul
del /f /s /q /a *.unsuccessfulbuild 2>nul
del /f /s /q /a *.user 2>nul
del /f /s /q /a *.vddklaunch 2>nul
rem del /f /s /q /a *.vspscc 2>nul
rem del /f /s /q /a *.vsscc 2>nul
rem del /f /s /q /a *.vssscc 2>nul
del /f /s /q /a *.wrn 2>nul
rem del /f /s /q /a *.xml 2>nul

for /r . %%d in (.) do rd /s /q "%%d\objchk_win7_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_win7_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_win7_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wlh_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wlh_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wlh_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wnet_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wnet_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wnet_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wxp_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wxp_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objchk_wxp_x86" 2>nul

for /r . %%d in (.) do rd /s /q "%%d\objfre_win7_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_win7_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_win7_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wlh_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wlh_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wlh_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wnet_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wnet_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wnet_x86" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wxp_amd64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wxp_ia64" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\objfre_wxp_x86" 2>nul

for /r . %%d in (.) do rd /s /q "%%d\.vs" 2>nul
rem for /r . %%d in (.) do rd /s /q "%%d\bin" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\Debug" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\DebugCK" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\DebugPS" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\DUI70.pdb" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\ipch" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\kernelbase.pdb" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\msvcr120.i386.pdb" 2>nul
rem for /r . %%d in (.) do rd /s /q "%%d\Release" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\ReleaseCK" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\ReleasePS" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\obj" 2>nul
rem for /r . %%d in (.) do rd /s /q "%%d\Package" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\Unicode Debug" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\Unicode Release" 2>nul
for /r . %%d in (.) do rd /s /q "%%d\x64" 2>nul