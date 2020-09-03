#!/bin/bash -ex

source /etc/profile.d/devkit-env.sh

apt-get install -qq -y g++ build-essential

cd perm2/P-Emerald_2
make NOSOUND=1
mv P-Emerald_2.nds P-Emerald_2_NOSOUND.nds
make DESQUID=1 NOSOUND=1 rebuild
mv P-Emerald_2.nds P-Emerald_2_NOSOUND_DESQUID.nds
make DESQUID=1 rebuild
mv P-Emerald_2.nds P-Emerald_2_DESQUID.nds
make rebuild
