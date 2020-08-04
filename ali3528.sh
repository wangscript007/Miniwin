if [ $# -gt 0 ]
then
mkdir -p out-ali3528r
pushd out-ali3528r
TYPE=Release
else
mkdir -p out-ali3528
pushd out-ali3528
TYPE=Debug
fi

cmake -DCMAKE_TOOLCHAIN_FILE=../ali3528_mtitoolchain.cmake \
   -DCMAKE_INSTALL_PREFIX=./ \
   -DNGL_CHIPSET="ali3528" \
   -DNGL_CA=acs \
   -DCMAKE_BUILD_TYPE=${TYPE} \
   ..
popd
