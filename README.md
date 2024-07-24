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

### Evaluating Kindle Framework
- Kindle frame work for sample **quick_sort.c** program is provided in **framework** directory
- Follow steps mentioned under **For framework evaluation** to run Kindle for sample **quick_sort.c**
- Rest of the README in **framework** directory provides more information on working of Kindle

### Evaluating of Persistent Pagetable Maintenance Schemes using Kindle
- **process_persistence** contains **rebuild** and **persistent** schemes of pagetable maintenance
- change into **process_persistence** directory
- `cd process_persistence`
- execute **run.sh**, it copies the **gemos.img** disk image and builds gem5
- `./run.sh`
- execute **run_fig4_a.sh** to copy scripts and run simulation to generate output in **output** folder
- **run_fig4_a.sh** finally generates **figure_4_a.eps** plot in **output/pagetable/results**
- Compare generated **figure_4_a.eps** with expected result in **output/pagetable/expected_results**
- Similarly execute **run_fig4_b.sh** to generate **figure_4_b.eps** plot in **output/pagetable/results**
- Compare generated **run_fig4_b.sh** with expected result in **output/pagetable/expected_results**
- Execute **run_table_3.sh** generate results for **Table 3** (table_3.tsv) in  **output/pagetable/results**
- Compare generated **table_3.tsv** with expected result in **output/pagetable/expected_results** using diff command
- `diff expected_results/table_3.tsv results/table_3.tsv`  
