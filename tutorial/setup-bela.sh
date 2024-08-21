if [ ! -d "/opt/pytorch-install" ]; then
    echo "Pytorch not found in /opt/pytorch-install"

    # Download pytorch to /opt/pytorch-install
    url=https://github.com/rodrigodzf/bela-torch/releases/download/master/pytorch-install.tar.gz
    echo "Downloading Pytorch from $url"
    wget -O - $url | tar -xz -C /opt
fi

echo "Copying libtorch to Bela..."

rsync \
--timeout=10 \
-avzP  /opt/pytorch-install/lib/libtorch_cpu.so /opt/pytorch-install/lib/libtorch.so root@$BBB_HOSTNAME:Bela/lib/

echo "Finished"