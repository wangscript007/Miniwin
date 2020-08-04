mkdir -p out-montage
pushd out-montage
cmake -DCMAKE_TOOLCHAIN_FILE=../montage-toolchain.cmake \
   -DCMAKE_INSTALL_PREFIX=./ \
   -DNGL_CHIPSET=montage \
   -DUSE_RFB_GRAPH=ON ..
popd

