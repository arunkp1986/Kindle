import sys
import csv
import os
import sys
from math import floor

cwd = os.getcwd()
"""
if(len(sys.argv)>1):
    dirname = sys.argv[1]
else:
    print("pass directory name")
    exit(0)
"""
#print("dirname:::",dirname)

#print(files)

metrics = {"simSeconds":"simSeconds","numCycles":"system.cpu.numCycles","demandMisses":"system.cpu.dcache.demandMisses::cpu.data","demandAvgMissLatency":"system.l3.demandAvgMissLatency::cpu.data"}

#benchmarks = ["random","sparse","stream","probnorm","probpoisson","quick","stackuseal","stackusear","stackusebr","stackusecr","stackusedr","stackusefr"]

directories = ["case1"]
#benchmarks = ["two","four", "eight"]
benchmarks = {"two":"256MB","four":"128MB","eight":"64MB"}

def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if ("case5" in dirpath and "512MB" in dirpath):
            for filename in filenames:
                if ("stats.txt" in filename):
                    filepath = os.path.join(dirpath,filename)
                    files.append(filepath)
    #print(files)


def get_data(data,files):
    for fil in files:
        scheme = fil.split('/')[-4]
        case = fil.split('/')[-3]
        dirname = (fil.split('/')[-2]).split("_")[0]
        interval = (fil.split('/')[-2]).split("_")[-1]
        #print(fil, scheme, case, dirname, interval)
        with open(fil,"r") as f:
            for line in f:
                if(metrics["simSeconds"] in line):
                    value = float(line.split()[1])
                    if(dirname in benchmarks):
                        size = benchmarks[dirname]
                        data[scheme][case][size][interval]["simSeconds"].append(value)
    #print(data)

if __name__=="__main__":
    files = []
    get_files(files)
    data = {"nvm":{"case5":{"64MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}, "128MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}, "256MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}, "rebuild":{"case5":{"64MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}, "128MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}, "256MB":{"10ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"100ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"1000ms":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}}
    get_data(data, files)
    para = "simSeconds"
    with open('table_4.tsv'.format(para), 'w', newline='') as csvfile:
        fieldnames = ['Alloc/Free Size','Interval', 'Persistent', 'Rebuild']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames,delimiter='\t', lineterminator='\n')
        writer.writeheader()
        d = {}
        for k1,v1 in data.items():
            for k2,v2 in v1.items():
                for k3,v3 in v2.items():
                    if not k3 in d:
                        d[k3] = {}
                    for k4,v4 in v3.items():
                        if not k4 in d[k3]:
                            d[k3][k4] = {}
                        if(k1 == 'nvm'):
                            d[k3][k4]['persistent'] = floor(v4[para][0]*1000)
                        elif(k1 == 'rebuild'):
                            d[k3][k4]['rebuild'] = floor(v4[para][0]*1000)
        #print(d)
        for k1,v1 in d.items():
            for k2,v2 in v1.items():
                writer.writerow({'Alloc/Free Size':k1,'Interval':k2,'Persistent':v2["persistent"],'Rebuild':v2["rebuild"]})
