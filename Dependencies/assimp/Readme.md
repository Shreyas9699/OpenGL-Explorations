Open Asset Import Library (assimp)
==================================

Open Asset Import Library is a library to load various 3d file formats into a shared, in-memory format. It supports more than __40 file formats__ for import and a growing selection of file formats for export.

### How to get the builds ###
I believe assimp are not release the builds anymore, so you will have to build by yourselfs, I have added steps you can follow.

1. Download latest release from github (here)[https://github.com/assimp/assimp/releases]
2. Once downloaded and unzipped, go to the main dir
3. Open command promt, make sure cmake is installed and run below command:
	`cmake .\CMakeLists.txt`
4. Once completed, you should be able to see a `Assimp.sln`
5. Open the `Assimp.sln` and select the mode you want to build `Release` or `Debug`
6. Once done, you should be able to see `lib`, `bin`
7. Navigate and copyt relevant files into dependencies and link them to the project

If you are donwloading this repo directly, the release files are already there, so just make sure to add dependencies:

## Note: Only works for x64 as there is no build option Win32 ##