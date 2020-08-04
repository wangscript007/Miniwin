if [ $# -gt 0 ]
then
mkdir -p out-gx3213r
pushd out-gx3213r
TYPE=Release
else
mkdir -p out-gx3213
pushd out-gx3213
TYPE=Debug
fi

cmake -DCMAKE_TOOLCHAIN_FILE=../national-toolchain.cmake \
   -DCMAKE_INSTALL_PREFIX=./ \
   -DNGL_CHIPSET="gx3213" \
   -DNGL_CA="irdetocca" \
   -DCMAKE_BUILD_TYPE=${TYPE}\
   ..
popd
