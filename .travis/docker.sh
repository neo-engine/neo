#!/bin/bash -ex

source /etc/profile.d/devkit-env.sh

ls -a
echo $TRAVIS_BUILD_DIR

cd P-Emerald_2
make NOSOUND=1
mv P-Emerald_2.nds P-Emerald_2_NOSOUND.nds
make DESQUID=1 NOSOUND=1 rebuild
mv P-Emerald_2.nds P-Emerald_2_NOSOUND_DESQUID.nds
make DESQUID=1 rebuild
mv P-Emerald_2.nds P-Emerald_2_DESQUID.nds
make rebuild
