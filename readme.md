# pybela + pytorch bela cross-compilation tutorial

In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:

1. Record a dataset of sensor data using [pybela](https://github.com/belaplatform/pybela)
2. Train a TCN to predict the sensor data using [pytorch](https://pytorch.org/)
3. Cross-compile with the [xc-bela-container](https://github.com/pelinski/xc-bela-container) and deploy the model to run in real-time in Bela

## Setting up your Bela

You will need to flash the Bela experimental image `v0.5.0alpha2` which can be downloaded [here](https://github.com/BelaPlatform/bela-image-builder/releases/tag/v0.5.0alpha2). You can follow [these instructions](https://learn.bela.io/using-bela/bela-techniques/managing-your-sd-card/#flash-an-sd-card-using-balena-etcher) to flash the image onto your Bela's microSD card.

Once the image is flashed, insert the microSD into your Bela and connect it to your computer. Inside the container (in the next section) we will run a script that will copy the necessary libraries to your Bela and updates its core code.

## Quickstart

If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, start it (open the Docker app). There is no need to create an account to follow this tutorial.

Pull the docker image:

```bash
docker pull pelinski/pybela-pytorch-xc-tutorial:v0.1.1
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela-tutorial`)
If you are using a windows machine, replace `BBB_HOSTNAME=192.168.7.2` for `BBB_HOSTNAME=192.168.6.2`.

```bash
docker run -it --name bela-tutorial -e BBB_HOSTNAME=192.168.7.2 -p 8889:8889 pelinski/pybela-pytorch-xc-tutorial:v0.1.1
```

**If you are using your own Bela** (i.e., not the ones prepared for the workshop), you will need to copy a couple of libraries to Bela and update its core code. If your Bela has a rev C cape you will also need to update the Bela cape firmware. You can do this by running the following commands inside the container:

```bash
sh scripts/copy-libs-to-bela.sh && sh scripts/setup-bela-dev.sh
sh scripts/setup-bela-revC.sh # only if you have a rev C cape
```

Inside the container, you can start the jupyter notebook with

```bash
jupyter notebook --ip=* --port=8889  --allow-root --no-browser
```

Look for a link of the form `http://127.0.0.1:8889/tree?token=<a-long-token>` in the terminal output and open it in the browser. This will show a list of files. Open the notebook `tutorial.ipynb` and follow the tutorial instructions there. If the link does not work, try changing the port number `8889` to another value, e.g., `5555`.

The tutorial continues in the jupyter notebook!

## Troubleshooting

If you get any strange errors (possibly with `undefined reference`) when trying to compile a Bela project after switching the Bela branch (step 1), try running these commands in Bela:

```bash
ssh root@bela.local
cd ~/Bela
make -f Makefile.libraries cleanall
make coreclean
```

and then try to compile the project again.
