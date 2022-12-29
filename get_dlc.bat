@pushd "%~dp0"
@git submodule update --init
@krink\get_dlc.bat
@popd