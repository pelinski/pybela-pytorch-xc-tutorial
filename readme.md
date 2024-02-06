# pybela + pytorch cross-compilation tutorial

**If you are using a Mac with an Apple chip (M1/M2), follow the instructions in [readme-silicon.md](readme-silicon.md).**

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

## 2. Run the Docker container and jupyter notebook

Pull the docker image:

```bash
docker pull pelinski/xc-bela-container:v0.1.2
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela`)
If you are using a windows machine, replace `BBB_HOSTNAME=192.168.7.2` for `BBB_HOSTNAME=192.168.6.2`.

```bash
docker run -it --name bela -e BBB_HOSTNAME=192.168.7.2 -p 8888:8888 pelinski/xc-bela-container:v0.1.2
```

Inside the container, you can start the jupyter notebook with

```bash
pipenv run jupyter notebook --ip=0.0.0.0 --port=8888  --allow-root
```

This will set up the docker image, run a container, and start a jupyter notebook in the browser. If the notebook doesn't open automatically, look for a link of the form `http://127.0.0.1:8888/tree?token=<a-long-token>` in the terminal output and open it in the browser. This will show a list of files. Open the notebook `tutorial.ipynb` and follow the tutorial instructions there.

## Troubleshooting

If you get the following error when trying to run `torch` inside the container

```
RuntimeError: could not create a primitive descriptor for a matmul primitive
```

this seems to be an error related to running pytorch on Docker on a Mac M1/M2 machine, I have yet not found a solution for it. I suggest you instead follow [these instructions](readme-silicon.md) run the jupyter notebook locally (i.e., outside the container, on your machine).
