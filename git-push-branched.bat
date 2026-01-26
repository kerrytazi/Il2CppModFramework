@echo off
setlocal enabledelayedexpansion

:: Exit on error
set ERRORLEVEL=0
call :main
exit /b %ERRORLEVEL%

:main
:: Get current branch name
for /f "tokens=*" %%i in ('git branch --show-current 2^>nul') do set CURRENT_BRANCH=%%i

:: Check if we're already on master
if "%CURRENT_BRANCH%"=="master" (
    echo Already on master branch. Skip stashing.
    git pull --rebase origin master
    if errorlevel 1 (
        echo Error: Failed to pull from origin/master
        set ERRORLEVEL=1
        exit /b 1
    )
    goto :after_master_checkout
)

:: Not on master - stash, checkout, rebase, pop
git stash push --include-untracked
if errorlevel 1 (
    echo Error: Failed to stash push
    set ERRORLEVEL=1
    exit /b 1
)

git checkout master
if errorlevel 1 (
    echo Error: Failed to checkout master
    set ERRORLEVEL=1
    exit /b 1
)

git pull --rebase origin master
if errorlevel 1 (
    echo Error: Failed to pull from origin/master
    set ERRORLEVEL=1
    exit /b 1
)

git stash pop
if errorlevel 1 (
    echo Error: Failed to stash pop
    set ERRORLEVEL=1
    exit /b 1
)

:after_master_checkout

set /p BRANCH_NAME="Enter branch name: "
if "%BRANCH_NAME%"=="" (
    echo Error: Branch name cannot be empty
    set ERRORLEVEL=1
    exit /b 1
)

:: Check if branch already exists locally
git show-ref --verify --quiet refs/heads/%BRANCH_NAME%
if not errorlevel 1 (
    echo Error: Branch %BRANCH_NAME% already exists locally
    set ERRORLEVEL=1
    exit /b 1
)

git checkout -b %BRANCH_NAME%
if errorlevel 1 (
    echo Error: Failed to create branch %BRANCH_NAME%
    set ERRORLEVEL=1
    exit /b 1
)

git add .
if errorlevel 1 (
    echo Error: Failed to add files
    set ERRORLEVEL=1
    exit /b 1
)

set /p COMMIT_MSG="Enter commit message: "
if "%COMMIT_MSG%"=="" (
    echo Error: Commit message cannot be empty
    set ERRORLEVEL=1
    exit /b 1
)

git commit -m "%COMMIT_MSG%"
if errorlevel 1 (
    echo Error: Failed to commit changes
    set ERRORLEVEL=1
    exit /b 1
)

git push -u origin %BRANCH_NAME%
if errorlevel 1 (
    echo Error: Failed to push branch %BRANCH_NAME%
    set ERRORLEVEL=1
    exit /b 1
)

echo Done! Branch %BRANCH_NAME% has been created and pushed to remote.
pause
exit /b 0
