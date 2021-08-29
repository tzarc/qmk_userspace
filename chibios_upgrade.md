# Upgrading the ChibiOS repositories

```sh
# Get ChibiOS
git clone git@github.com:qmk/ChibiOS.git qmk_chibios
cd qmk_chibios
git remote add upstream https://github.com/ChibiOS/ChibiOS
git fetch --all --tags --prune

# Work out the tag that we want to propagate into QMK, then:
git push origin ver20.3.2

# For the develop branch window, work out the tag we're using, then:
git tag -a develop_2021_q3 -m develop_2021_q3 svn-mirror/ver20.3.3
git push origin develop_2021_q3
```

```sh
# Get ChibiOS-Contrib
git clone git@github.com:qmk/ChibiOS-Contrib.git qmk_chibios-contrib
cd qmk_chibios-contrib
git remote add upstream https://github.com/ChibiOS/ChibiOS-Contrib
git fetch --all --tags --prune

# Work out the branch based on the selected ChibiOS version above, then:
git checkout chibios-20.3.x

# For the develop branch window, using the same tag as ChibiOS:
git tag -a develop_2021_q3 -m develop_2021_q3 mirror/chibios-20.3.x
git push origin develop_2021_q3
```

# Upgrading ChibiOS within QMK

```sh
# From your QMK directory:
cd $QMK_FIRMWARE_DIR
git checkout develop
make git-submodule
git checkout -b chibios-upgrade develop_2021_q3

# Upgrade ChibiOS, using the tag we created before
cd $QMK_FIRMWARE_DIR
cd lib/chibios
git fetch --all --tags --prune
git checkout develop_2021_q3

# Upgrade ChibiOS-Contrib, using the tag we created before
cd $QMK_FIRMWARE_DIR
cd lib/chibios-contrib
git fetch --all --tags --prune
git checkout develop_2021_q3

# Upgrade ChibiOS configs:
cd $QMK_FIRMWARE_DIR
./util/chibios-upgrader.sh

# Commit...
cd $QMK_FIRMWARE_DIR
git commit -am 'ChibiOS upgrade to ver20.3.3.'

# Build...
make -j -O all-chibios:default

# Push...
git push origin chibios-upgrade-ver20.3.3
```