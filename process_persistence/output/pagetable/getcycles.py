import sys
import csv
import os
import sys

cwd = os.getcwd()

metrics = {"simSeconds":"simSeconds","numCycles":"system.cpu.numCycles","demandMisses":"system.cpu.dcache.demandMisses::cpu.data","demandAvgMissLatency":"system.l3.demandAvgMissLatency::cpu.data"}


directories = ["case1"]
benchmarks = ["gapbs","sssp", "workloadb"]

def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if "case1" in dirpath:
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
    data = {"nvm":{"case1":{"16MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "32MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "64MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"128MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}},"rebuild":{"case1":{"16MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "32MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "64MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"128MB":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}
    get_data(data, files)
    para = "simSeconds"
    with open("pagetable_case1.tsv", 'w', newline='') as csvfile:
        fieldnames = ['#scheme', 'persistent', 'rebuild']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames,delimiter='\t', lineterminator='\n')
        writer.writeheader()
        d = {}
        for k1,v1 in data.items():
            for k2,v2 in v1.items():
                for k3,v3 in v2.items():
                    if not k3 in d:
                        d[k3] = {}
                        if(k1 == "nvm"):
                            d[k3]['persistent'] = round(v3[para][0]*1000,0)
                        elif(k1 == "rebuild"):
                            d[k3]['rebuild'] = round(v3[para][0]*1000,0)
                    else:
                        if(k1 == "nvm"):
                            d[k3]['persistent'] = round(v3[para][0]*1000,0)
                        elif(k1 == "rebuild"):
                            d[k3]['rebuild'] = round(v3[para][0]*1000,0)
        for k1,v1 in d.items():
            writer.writerow({'#scheme':k1,'persistent':v1['persistent'],'rebuild':v1['rebuild']})
