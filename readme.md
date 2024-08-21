# pybela + pytorch cross-compilation tutorial

**If you are using a Mac with an Apple chip (M1/M2), follow the instructions in [readme-silicon.md](readme-silicon.md).**

In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:

1. Record a dataset of potentiometer values using [pybela](https://github.com/belaplatform/pybela)
2. Train an RNN to predict the potentiometer's values using [pytorch](https://pytorch.org/)
3. Cross-compile and deploy the model to run in real-time in Bela

## Quickstart
To avoid installation chaos, we have prepared a docker container. If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, start it (open the Docker app).

Pull the docker image:

```bash
docker pull pelinski/pybela-pytorch-tutorial:v0.1.0
```

This will pull the dockerised cross-compiler. You can start the container by running:
(this will create the container for the first time. If you have created the container already, you can enter the container back by running `docker start -ia bela`)
If you are using a windows machine, replace `BBB_HOSTNAME=192.168.7.2` for `BBB_HOSTNAME=192.168.6.2`.

```bash
docker run -it --name bela-tutorial -e BBB_HOSTNAME=192.168.7.2 -p 8889:8889 pelinski/pybela-pytorch-tutorial:v0.1.0
```

Inside the container, you can start the jupyter notebook with

```bash
jupyter notebook --ip=* --port=8889  --allow-root --no-browser
```

Look for a link of the form `http://127.0.0.1:8888/tree?token=<a-long-token>` in the terminal output and open it in the browser. This will show a list of files. Open the notebook `tutorial.ipynb` and follow the tutorial instructions there. If the link does not work, try changing the port number `8889` to another value, e.g., `5555`.

## Troubleshooting

If you get any strange errors (possibly with `undefined reference`) when trying to compile a Bela project after switching the Bela branch (step 1), try running these commands in Bela:

```bash
ssh root@bela.local
cd ~/Bela
make -f Makefile.libraries cleanall
make coreclean
```

and then try to compile the project again.

If you get the following error when trying to run `torch` inside the container

```
RuntimeError: could not create a primitive descriptor for a matmul primitive
```

this seems to be an error related to running pytorch on Docker on a Mac M1/M2 machine, I have yet not found a solution for it. I suggest you instead follow [these instructions](readme-silicon.md) run the jupyter notebook locally (i.e., outside the container, on your machine).
