# pyBela AIMC 2023 tutorial

In this tutorial, we will use a jupyter notebook to communicate with Bela from the host machine and:

1. Record a dataset of potentiometer values
2. Train an RNN to predict the potentiometer's values
3. Cross-compile and deploy the model to run in real-time in Bela

To avoid installation chaos, we have prepared a docker container. If you haven't got docker installed on your machine yet, you can follow the instructions [here](https://docs.docker.com/engine/install/). Once you have docker installed, start it (run the Docker GUI). Then, open a terminal and run:

```bash
docker pull pelinski/xc-bela-container:aimc2023
```

This will pull the dockerised cross-compiler. You can start the container by running:

```bash
docker run -it --name bela --env-file devcontainer.env  -p 8888:8888 pelinski/xc-bela-container:aimc2023
```

Inside the container, you can start the jupyter notebook with

```bash
pipenv run jupyter notebook --ip=0.0.0.0 --port=8888 --no-browser --allow-root

```

This will set up the docker image, run a container, and start a jupyter notebook in the browser. If the notebook doesn't open automatically, look for a link of the form `http://127.0.0.1:8888/tree?token=25fdd11ba4da56070fca58ca70fb91f47bcd46b277ebe660` in the terminal output and open it in the browser. This will show a list of files, open the notebook `tutorial.ipynb` and follow the tutorial instructions there.

Note: You will need the Bela experimental version `v0.5.0alpha2` which can be downloaded here https://github.com/BelaPlatform/bela-image-builder/releases/tag/v0.5.0alpha2. Follow the instructions [here](https://learn.bela.io/using-bela/bela-techniques/managing-your-sd-card/#flash-an-sd-card-using-balena-etcher) to flash that image onto your Bela. You should also checkout the Bela `dev` branch at `52178389dca7b07ceb5b09137f661e59e3c9e5b8`.
