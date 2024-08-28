#!/bin/bash -e
mkdir -p /sysroot/opt/pytorch-install

url=https://github.com/pelinski/bela-torch/releases/download/v1.13.1/pytorch-v1.13.1.tar.gz
echo "Downloading Pytorch from $url"
wget -O - $url | tar -xz -C /sysroot/opt/pytorch-install
