# Kindle: A Comprehensive Framework for Exploring  OS-Architecture Interplay in Hybrid Memory Systems
Kindle is an open-source framework based on gem5 and gemOS to explore and prototype research ideas in hybrid memory systems, crossing the hardware-software boundaries and performing comprehensive empirical evaluation.

### Building and Running Kindle
- Install Docker on your system, we used Ubuntu 20.04.3.
- Download Docker export file [prosper.tar](https://drive.google.com/file/d/15zgZGVF875KMg2COBpXdJpEJAlfV88Jr/view?usp=sharing), size ~ 1GB
- Import downloaded **prosper.tar** using `docker import prosper.tar prosper:latest`
- List the image using `docker image ls`
- Start the docker container `docker run -it --privileged prosper:latest /bin/bash`
- All steps mentioned below are to be performed inside container.
- To install "column" command run `apt-get install bsdmainutils` inside container.
- Change to home directory inside the docker container `cd /home`
- Clone this git repo using `git clone --recurse-submodules https://github.com/arunkp1986/Kindle.git` inside the running container.
