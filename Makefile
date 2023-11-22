# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# avoid 'Entering|Leaving directory' messages
MAKEFLAGS += --no-print-directory

QMK_USERSPACE := $(patsubst %/,%,$(dir $(shell realpath "$(lastword $(MAKEFILE_LIST))")))
ifeq ($(QMK_USERSPACE),)
    QMK_USERSPACE := $(shell pwd)
endif

# Add extra binaries to path
export PATH := $(QMK_USERSPACE)/bin:$(PATH)

# Deal with macOS
ifeq ($(shell uname -s),Darwin)
SED = gsed
ECHO = gecho
else
SED = sed
ECHO = echo
endif

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Repositories

$(QMK_USERSPACE)/qmk_firmware:
	git clone --depth=1 https://github.com/tzarc/qmk_firmware.git $(QMK_USERSPACE)/qmk_firmware

$(QMK_USERSPACE)/qmk_userspace:
	git clone --depth=1 https://github.com/qmk/qmk_userspace.git $(QMK_USERSPACE)/qmk_userspace

$(QMK_USERSPACE)/qmk-dot-github:
	git clone --depth=1 https://github.com/qmk/.github.git $(QMK_USERSPACE)/qmk-dot-github

qmk_firmware: $(QMK_USERSPACE)/qmk_firmware
qmk_userspace: $(QMK_USERSPACE)/qmk_userspace
qmk-dot-github: $(QMK_USERSPACE)/qmk-dot-github

.PHONY: repositories
repositories: qmk_firmware qmk_userspace qmk-dot-github

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Cleaning

.PHONY: clean distclean

clean:
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware clean

distclean:
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware distclean

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Container management

CONTAINER_PREAMBLE := export HOME="/tmp"; export PATH="/tmp/.local/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt

format-core:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a"

pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk pytest"

format-and-pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a && qmk pytest"

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QMK_FIRMWARE_ROOT = $(shell qmk config -ro user.qmk_home | cut -d= -f2 | sed -e 's@^None$$@@g')
ifeq ($(QMK_FIRMWARE_ROOT),)
    $(error Cannot determine qmk_firmware location. `qmk config -ro user.qmk_home` is not set)
endif

%:
	+$(MAKE) -C $(QMK_FIRMWARE_ROOT) $(MAKECMDGOALS) QMK_USERSPACE=$(QMK_USERSPACE)
