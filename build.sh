cd "$(dirname "$0")"
mkdir -p build
rm -rf build/*
cd build
cmake ../ -DENABLE_HIP=ON -DHOOMD_DIR=/home/ian/anaconda3/envs/hoomd3-gpu/lib/python3.9/site-packages/lib/cmake/hoomd
make install
# CC=clang CXX=clang++