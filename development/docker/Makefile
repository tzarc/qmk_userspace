QMK_TOOLCHAIN_VERSION := v14.2.0-3
CLANG_VERSION := 18.1.8

.PHONY: all
all: container

.PHONY: binaries
binaries: qmk_toolchain clang_toolchain

.PHONY: qmk_toolchain
qmk_toolchain:
	@if [ $$(uname -m | tr 'A-Z' 'a-z') = "x86_64" ]; then \
		ARCH=X64; \
	else \
		ARCH=ARM64; \
	fi; \
	toolchain_url=$$(curl -fsSL https://api.github.com/repos/qmk/qmk_toolchains/releases/tags/$(QMK_TOOLCHAIN_VERSION) \
		| jq -r '.assets[] | select(.name | contains("toolchain")) | .browser_download_url' \
		| grep linux$${ARCH}); \
	if [ ! -e $$(basename $${toolchain_url}) ]; then \
		echo Downloading $${toolchain_url}; \
		curl -fsSLO $${toolchain_url}; \
	fi

.PHONY: clang_toolchain
clang_toolchain:
	@if [ $$(uname -m | tr 'A-Z' 'a-z') = "x86_64" ]; then \
		clang_url="https://github.com/llvm/llvm-project/releases/download/llvmorg-$(CLANG_VERSION)/clang+llvm-$(CLANG_VERSION)-x86_64-linux-gnu-ubuntu-18.04.tar.xz"; \
	else \
		clang_url="https://github.com/llvm/llvm-project/releases/download/llvmorg-1$(CLANG_VERSION)/clang+llvm-$(CLANG_VERSION)-aarch64-linux-gnu.tar.xz"; \
	fi; \
	if [ ! -e $$(basename $${clang_url}) ]; then \
		echo Downloading $${clang_url}; \
		curl -fsSLO $${clang_url}; \
	fi

.PHONY: container
container: binaries Dockerfile.debian-qmk
	@docker rmi qmk/qmk_cli:develop || true
	docker build -D -t qmk/qmk_cli:develop -f Dockerfile.debian-qmk .

.PHONY: clean
clean:
	rm -f clang+llvm-$(CLANG_VERSION)-*.tar.xz
	rm -f qmk_toolchain-*.tar.xz
	docker rmi qmk/qmk_cli:develop || true

.PHONY: shell
shell: container
	docker run --rm -it -v $$(qmk config user.qmk_home | cut -d= -f2):/qmk_firmware qmk/qmk_cli:develop
