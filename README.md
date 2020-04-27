# DICOM Web-PACS Viewer 

This is my old pre-dicomweb PACS proxy and HTML5 DICOM-viewer based on Qt and DCMTK.
It provides a rest api that proxies requests to DIMSE networking (C-FIND and C-MOVE) and uses a html/js client to render images.

The webclient source is hosted here https://github.com/knopkem/pacsnode-webclient

# HOW to build
* install modern compiler (gcc, vs, clang)
* install CMAKE
* install recent version of Qt
* Build dcmtk (3.5.6) with shared libs (on windows disabled option: DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS)
* clone repo and configure with CMAKE
* set QT_ROOT_PREFIX to root of binary directory e.g. C:/Qt_online/5.14.2/msvc2017_64
* set DCMTK_DIR to build directory
* generate and build
* build install
* start from build/install_dir folder
* optional: install NSIS: build package

Prebuild binaries available here: https://sourceforge.net/projects/pacsnode/
