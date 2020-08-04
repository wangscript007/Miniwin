if [ $# -gt 0 ]
then
mkdir -p out-x86r
pushd out-x86r
TYPE=Release
else
mkdir -p out-x86
pushd out-x86
TYPE=Debug
fi

cmake -DNGL_CHIPSET=x86  -DUSE_RFB_GRAPH=ON  \
    -DCMAKE_INSTALL_PREFIX=./ \
    -DCMAKE_BUILD_TYPE=${TYPE} \
    ..
popd
