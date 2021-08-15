#!/bin/bash

# From: https://docs.rust-embedded.org/book/intro/install.html
rustup target add thumbv6m-none-eabi thumbv7m-none-eabi thumbv7em-none-eabi thumbv7em-none-eabihf thumbv8m.base-none-eabi thumbv8m.main-none-eabi thumbv8m.main-none-eabihf
cargo install cargo-binutils
rustup component add llvm-tools-preview
cargo install cargo-generate