call cmake.bat

cd ..
mkdir build
cd build

msbuild baseline.sln /p:Configuration=Debug

cd ..
cd scripts
