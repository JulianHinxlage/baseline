call cmake.bat

cd ..
mkdir build
cd build

msbuild Baseline.sln /p:Configuration=Debug

cd ..
cd scripts
