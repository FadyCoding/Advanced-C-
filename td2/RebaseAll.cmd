@echo off

git checkout td4
if NOT %ERRORLEVEL% == 0 goto :err
git rebase master
if NOT %ERRORLEVEL% == 0 goto :err

git checkout td3
if NOT %ERRORLEVEL% == 0 goto :err
git rebase td4
if NOT %ERRORLEVEL% == 0 goto :err

git checkout td2
if NOT %ERRORLEVEL% == 0 goto :err
git rebase td3
if NOT %ERRORLEVEL% == 0 goto :err

git checkout td1
if NOT %ERRORLEVEL% == 0 goto :err
git rebase td2
if NOT %ERRORLEVEL% == 0 goto :err

git checkout master
if NOT %ERRORLEVEL% == 0 goto :err

goto end

:err
echo === Process stopped! ===

:end
