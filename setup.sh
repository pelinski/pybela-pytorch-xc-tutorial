pipenv install

echo "Copying projects to Bela..."

rsync \
--timeout=10 \
-avrzP  bela-code/inference \
root@$BBB_HOSTNAME:Bela/projects/

rsync \
--timeout=10 \
-avrzP  bela-code/dataset-capture  \
root@$BBB_HOSTNAME:Bela/projects/


pipenv run jupyter notebook
