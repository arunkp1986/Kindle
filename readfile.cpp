#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>


#define FILENAME "data.csv"

using namespace std;
struct data{
	unsigned interval;
	unsigned offset;
	char ops;
        int size;
	char type[6];
	int index;
};

int main(int argc, char* argv[])
{
    string fname;
    struct data ds;
    int fd = open("data.img",O_RDWR);
    if(fd < 0){
        printf("file open error\n");
	exit(0);
    }
    const char* elems = getenv("ELEMS");
    if(!elems){
        printf("ELEMS variable not set\n");
	exit(0);
    }
    unsigned long size = sizeof(struct data)*stoi(elems);
    lseek(fd, size, SEEK_SET);
    struct data end = {.interval = 0, .offset = 0,.ops = 'Y',.size = 0,.type = {'X','\0'},.index=0 };
    write(fd, &end, sizeof(struct data));
    struct data* ts = (struct data*)mmap(NULL,size,PROT_READ|PROT_WRITE, MAP_SHARED,fd,0);
    vector<string> row;
    string line, word;
    stringstream linestream;
    unsigned count = 0;
    fstream file (FILENAME, ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            linestream.clear();
            linestream.str(line);
            while(getline(linestream, word, ',')){
                row.push_back(word);
	    }
            ts[count].interval = stoul(row[0],nullptr,10);
            ts[count].offset = stoul(row[1],nullptr,10);
            ts[count].ops = row[2][0];
            ts[count].size = stoi(row[3]);
	    strncpy(ts[count].type,row[4].c_str(),strlen(row[4].c_str()));
	    ts[count].index = stoi(row[5]); 
	    count +=1;
	}
    }
    else{
      cout<<"Could not open the file\n";
      return 0;
    }
    if(msync(ts,size,MS_SYNC) < 0){
        printf("msync failed\n");
	exit(0);
    }
    cout<<"count of records loaded to disk image "<<count<<endl;
    munmap(ts, size);
    close(fd);
    return 0;
}
