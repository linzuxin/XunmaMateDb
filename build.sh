cd meta_mgr_cpp
rm -rf build
mkdir build
cd build
cmake ..
make
./meta_tester ./libmeta_mgr.so ./data