# gs_core
![GitHub](https://img.shields.io/github/license/mrfrenik/gs_core)
[![Discord](https://img.shields.io/discord/485178488203116567?label=discord&logo=discord)](https://discord.gg/QXwpETB)
![GitHub top language](https://img.shields.io/github/languages/top/mrfrenik/gs_core)

Library of working examples to demonstrate the [gunslinger](https://github.com/MrFrenik/gunslinger) core engine framework.

## Cloning instructions: 
- Use the following command to clone the repo and init the gs_core submodule
```
git clone --recursive https://github.com/MrFrenik/gs_core gs_core/gs_core
```

## Updating GS instructions (updates gs_core submodule and rebases to main branch): 
```
git submodule update --remote --rebase --recursive
```
## Build Instructions:

### Windows
## MSVC:
- From start menu, open `{x86|x64} Native Tools for {VS Version Here}`
- cd to `root dir` where you downloaded gs_core
- gs_core comes with a project generator. For now, all projects will be installed in the same root directory as your gs_core directory.
- To run the project generator, run:
```bash
proc\win\cl.bat {your_project_name}
```
- If a directory already exists with that name, you will be prompted to select a different one.
- Your project will be placed in the top level directory where gs_core lives. 
