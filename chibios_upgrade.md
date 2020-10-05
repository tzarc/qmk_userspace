# Upgrading the ChibiOS repositories

```shell
# Get ChibiOS
git clone git@github.com:qmk/ChibiOS.git qmk_chibios
cd qmk_chibios
git remote add upstream https://github.com/ChibiOS/ChibiOS
git fetch --all --tags --prune

# Work out the tag that we want to propagate into QMK, then:
git push origin ver20.3.2

# For the develop branch window, work out the tag we're using, then:
git tag -a develop_2020_q4 -m develop_2020_q4 ver20.3.2
git push origin develop_2020_q4
```

```shell
# Get ChibiOS-Contrib
git clone git@github.com:qmk/ChibiOS-Contrib.git qmk_chibios-contrib
cd qmk_chibios-contrib
git remote add upstream https://github.com/ChibiOS/ChibiOS-Contrib
git fetch --all --tags --prune

# Work out the branch based on the selected ChibiOS version above, then:
git checkout chibios-20.3.x

# For the develop branch window, using the same tag as ChibiOS:
git tag -a develop_2020_q4 -m develop_2020_q4
git push origin develop_2020_q4
```

# Upgrading ChibiOS within QMK

```shell

```