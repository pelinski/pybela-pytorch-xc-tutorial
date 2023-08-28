pipenv install

echo "Copying projects to Bela..."

export 

rsync \
--timeout=10 \
-avrzP  bela-code/inference \
root@$BBB_HOSTNAME:Bela/projects/

rsync \
--timeout=10 \
-avrzP  bela-code/dataset-capture  \
root@$BBB_HOSTNAME:Bela/projects/

echo "Starting Jupyter Notebook..."
