call build.bat

cd ..
mkdir deploy
cd build
cd Release

xcopy *.exe ..\..\deploy
xcopy *.dll ..\..\deploy

cd ..
cd ..
cd scripts
