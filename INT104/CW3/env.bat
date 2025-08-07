@echo off
REM This version includes proper error handling and conda initialization

REM Configuration - customize these values

call "C:\ProgramData\Anaconda3\Scripts\activate.bat"

set ENV_NAME=int104
set PYTHON_VERSION=3.9

REM Initialize conda (critical step)
call conda.bat activate

REM Check if conda is available
where conda >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Conda not found in PATH. Ensure Anaconda/Miniconda is installed.
    echo Make sure you ran conda init or added conda to your PATH.
    pause
    exit /b 1
)

echo Creating Conda environment '%ENV_NAME%' with Python %PYTHON_VERSION%...
call conda create --name %ENV_NAME% python=%PYTHON_VERSION% -y
if %ERRORLEVEL% neq 0 (
    echo Failed to create environment
    pause
    exit /b 1
)

echo Installing numpy in '%ENV_NAME%'...
call conda install --name %ENV_NAME% numpy scipy pandas matplotlib scikit-learn openpyxl seaborn -y

if %ERRORLEVEL% neq 0 (
    echo Failed to install numpy
    pause
    exit /b 1
)

echo.
echo Successfully created environment '%ENV_NAME%' with numpy installed.
echo To activate the environment, run:
echo    conda activate %ENV_NAME%
echo.
pause