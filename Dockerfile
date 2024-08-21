FROM pelinski/xc-bela-container:v1.1.0

COPY docker-scripts/download_libtorch.sh ./
RUN ./download_libtorch.sh  && rm download_libtorch.sh

COPY tutorial/bela-code/watcher /sysroot/root/Bela/projects/watcher/

COPY tutorial/bela-code/dataset-capture/ /sysroot/root/Bela/projects/dataset-capture/

COPY tutorial/bela-code/pot-inference /sysroot/root/Bela/projects/pot-inference/

# RUN apt-get update && \
#       apt-get install -y python3 pip
            
# COPY tutorial/requirements.txt ./
# RUN pip install -r requirements.txt
      
CMD /bin/bash