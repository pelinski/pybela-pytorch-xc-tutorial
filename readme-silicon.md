# pybela + pytorch cross-compilation tutorial

**These instructions are for Macs with Apple Silicon chips (M1/M2). If you are using a different machine, follow the instructions in [readme.md](readme.md).**

In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:

1. Record a dataset of sensor data using [pybela](https://github.com/belaplatform/pybela)
2. Train an RNN to predict the sensor data using [pytorch](https://pytorch.org/)
3. Cross-compile with the [xc-bela-container](https://github.com/pelinski/xc-bela-container) and deploy the model to run in real-time in Bela

## Setting up your Bela

You will need to flash the Bela experimental image `v0.5.0alpha2` which can be downloaded [here](https://github.com/BelaPlatform/bela-image-builder/releases/tag/v0.5.0alpha2). You can follow [these instructions](https://learn.bela.io/using-bela/bela-techniques/managing-your-sd-card/#flash-an-sd-card-using-balena-etcher) to flash the image onto your Bela's microSD card.

Once the image is flashed, insert the microSD into your Bela and connect it to your computer.

Inside the jupyter notebook (in the next section) we will run a script that will copy the necessary libraries to your Bela and updates its core code. Alternatively, you can run, after cloning this repo: `sh copy-libs-to-bela.sh` and `sh setup-bela-dev.sh` after cloning this repo.

## Quickstart

If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, start it (open the Docker app).

Pull the docker image:

```bash
docker pull pelinski/pybela-pytorch-tutorial:v0.1.0
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela-tutorial`)
If you are using a windows machine, replace `BBB_HOSTNAME=192.168.7.2` for `BBB_HOSTNAME=192.168.6.2`.

```bash
docker run -it --name bela-tutorial -e BBB_HOSTNAME=192.168.7.2 -p 8889:8889 pelinski/pybela-pytorch-tutorial:v0.1.0
```

Inside the container, you can start the jupyter notebook with

```bash
jupyter notebook --ip=* --port=8889  --allow-root --no-browser
```

Look for a link of the form `http://127.0.0.1:8889/tree?token=<a-long-token>` in the terminal output and open it in the browser. This will show a list of files. Open the notebook `tutorial.ipynb` and follow the tutorial instructions there. If the link does not work, try changing the port number `8889` to another value, e.g., `5555`.

The tutorial continues in the jupyter notebook!

## 2. Run the jupyter notebook

There seems to be a bug with pytorch when running on Docker on Mac Apple Silicon (M1/M2) machines. If you are using one of these machines, you should run the jupyter notebook locally (i.e., outside the container, on your machine).

First, clone this repo and `cd` into it:

```bash
git clone --recurse-submodules -j8  https://github.com/pelinski/pybela-pytorch-xc-tutorial.git
cd pybela-pytorch-xc-tutorial
```

Then, install the required python packages (you might want to activate a python environment first):

```bash
pip install requirements.txt
```

you can start the jupyter notebook by running:

```bash
jupyter notebook tutorial-silicon.ipynb
```

Follow the steps in the notebook to record a dataset in Bela and train a model to predict the potentiometer's values.

## 3. Cross-compile the inference code with Docker

Make sure the Docker app is open. Then, pull the docker image:

```bash
docker pull pelinski/xc-bela-container:v0.1.4
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela`)

```bash
docker run -it --name bela -e BBB_HOSTNAME=192.168.7.2  -p 8888:8888 pelinski/xc-bela-container:v0.1.4
```

Inside the container, you can cross-compile the Bela project with:

```bash
cd bela-code/pot-inference/ && sh build.sh
```

Once deployed, you can run it from the Bela terminal (which you can access from your regular terminal typing `ssh root@bela.local`):

```bash
cd Bela/projects/pot-inference
./pot-inference --modelPath ../model.jit
```
