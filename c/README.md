docker build -t ssp .   

docker run -v "$PWD/instances:/data" ssp "/data/"