import sys
import csv
import os
import sys

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


def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if "case2" in dirpath:
            for filename in filenames:
                if ("stats.txt" in filename):
                    filepath = os.path.join(dirpath,filename)
                    files.append(filepath)
    #print(files)


def get_data(data,files):
    for fil in files:
        scheme = fil.split('/')[-4]
        case = fil.split('/')[-3]
        dirname = fil.split('/')[-2]
        #print(fil, scheme, case, dirname)
        with open(fil,"r") as f:
            for line in f:
                if(metrics["simSeconds"] in line):
                    value = float(line.split()[1])
                    data[scheme][case][dirname]["simSeconds"].append(value)
                if(metrics["numCycles"] in line):
                    value = line.split()[1]
                    data[scheme][case][dirname]["numCycles"].append(value)
                if(metrics["demandMisses"] in line):
                    value = line.split()[1]
                    data[scheme][case][dirname]["demandMisses"].append(value)
                if(metrics["demandAvgMissLatency"] in line):
                    value = line.split()[1]
                    data[scheme][case][dirname]["demandAvgMissLatency"].append(value)
    #print(data)

if __name__=="__main__":
    files = []
    get_files(files)
    data = {"nvm":{"case2":{"1GB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "2MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "4KB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}},"rebuild":{"case2":{"1GB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "2MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "4KB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}
    get_data(data, files)
    para = "simSeconds"
    with open('pagetable_case2.tsv'.format(para), 'w', newline='') as csvfile:
        fieldnames = ['#gap', 'persistent', 'rebuild']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter='\t', lineterminator='\n')
        writer.writeheader()
        d = {}
        for k1,v1 in data.items():
            for k2,v2 in v1.items():
                for k3,v3 in v2.items():
                    if not k3 in d:
                        d[k3] = {}
                    if(k1=="nvm"):
                        d[k3]['persistent'] = round(v3[para][0]*1000,0)
                    elif(k1 == "rebuild"):
                        d[k3]['rebuild'] = round(v3[para][0]*1000,0)
        #print(d)
        for k1,v1 in d.items():
            writer.writerow({'#gap':k1,'persistent':v1['persistent'],'rebuild':v1['rebuild']})
