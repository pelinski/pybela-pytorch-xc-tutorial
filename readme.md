# pyBela AIMC 2023 tutorial
In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:
1. Record a dataset of potentiometer values
2. Train an RNN to predict the potentiometer's values
3. Cross-compile and deploy the model to run in real-time in Bela


To avoid installation chaos, we have prepared a docker container. If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, open a terminal and run:

```bash
git clone -b AIMC2023 --single-branch https://github.com/pelinski/xc-bela-container
```

```bash
cd xc-bela-container && sh run.sh
```

This will set up the docker image, run a container, and start a jupyter notebook in the browser. Open the notebook `tutorial.ipynb` and follow the tutorial instructions there.
