# how to reorganize

- git mv src/ lib/
- undo for main, CMakeLists.txt
- git mv include lib/
- git mv interfaces lib/
- git mv \<something common\> ../lib_common
- modify platformio.ini as seen in AC
- maybe manually fix intellisense include paths (VSCode will tell you) 