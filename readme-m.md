# pybela + pytorch cross-compilation tutorial

**These instructions are for Macs with Apple chips (M1/M2). If you are using a different machine, follow the instructions in [readme.md](readme.md).**

In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:

1. Record a dataset of potentiometer values using [pybela](https://github.com/belaplatform/pybela)
2. Train an RNN to predict the potentiometer's values using [pytorch](https://pytorch.org/)
3. Cross-compile and deploy the model to run in real-time in Bela

To avoid installation chaos, we have prepared a docker container. If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, start it (open the Docker app).

## 1. Set up your Bela

You will need the Bela experimental image `v0.5.0alpha2` which can be downloaded here https://github.com/BelaPlatform/bela-image-builder/releases/tag/v0.5.0alpha2. Follow [these instructions](https://learn.bela.io/using-bela/bela-techniques/managing-your-sd-card/#flash-an-sd-card-using-balena-etcher) to flash that image onto your Bela.

Follow the instructions below to checkout the Bela repo to commit `7d99f81` on the `pybela-xc` branch at https://github.com/pelinski/Bela/. You can check which commit you are in with `git rev-parse --short HEAD`.

### Option A: Bela connected to internet

If your Bela is connected to the internet, you can do this by ssh-ing into it and running

```bash
git remote add pelinski https://github.com/pelinski/Bela.git
git fetch pelinski
git checkout pybela-xc
```

### Option B: Bela not connected to internet

If your Bela is not connected to the internet, you can still update the Bela repo running:

```bash
git clone https://github.com/pelinski/Bela.git
cd Bela
git remote add board root@bela.local:Bela/
git checkout pybela-xc
git push -f board pybela-xc:pybela-xc
```

Then, ssh into Bela and run

```bash
ssh root@bela.local
cd Bela
git checkout pybela-xc
```

## 2.  Run the jupyter notebook

There seems to be a bug with pytorch when running on Docker on Mac M1/M2 machines. If you are using a Mac with an Apple chip, you should run the jupyter notebook locally (i.e., outside the container, on your machine). First, install `pipenv`:

First, clone this repo and `cd` into it:

```bash
git clone https://github.com/pelinski/pybela-pytorch-xc-tutorial
cd pybela-pytorch-xc-tutorial
```

install the python environment and torch:

```bash
pipenv install
pipenv run pip3 install torch
```

you can start the jupyter notebook by running:

```bash
pipenv run jupyter notebook tutorial-m1.ipynb
```

Follow the steps in the notebook to record a dataset in Bela and train a model to predict the potentiometer's values.

## 3. Cross-compile the inference code with Docker
Make sure the Docker app is open.  Then, pull the docker image:

```bash
docker pull pelinski/xc-bela-container:v0.1.1
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela`)

```bash
docker run -it --name bela --env-file devcontainer.env  -p 8888:8888 pelinski/xc-bela-container:v0.1.1
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

