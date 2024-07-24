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

directories = ["case1"]
benchmarks = ["two","four", "eight"]

def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if ("case3" in dirpath and "512MB" in dirpath):
            for filename in filenames:
                if ("stats.txt" in filename):
                    filepath = os.path.join(dirpath,filename)
                    files.append(filepath)
    #print(files)


def get_data(data,files):
    for fil in files:
        scheme = fil.split('/')[-4]
        case = fil.split('/')[-3]
        dirname = (fil.split('/')[-2]).replace("_512MB","")
        #print(fil, scheme, case, dirname)
        with open(fil,"r") as f:
            for line in f:
                if(metrics["simSeconds"] in line):
                    value = line.split()[1]
                    if(dirname in benchmarks):
                        data[scheme][case][dirname]["simSeconds"].append(value)
                if(metrics["numCycles"] in line):
                    value = line.split()[1]
                    if(dirname in benchmarks):
                        data[scheme][case][dirname]["numCycles"].append(value)
                if(metrics["demandMisses"] in line):
                    value = line.split()[1]
                    if(dirname in benchmarks):
                        data[scheme][case][dirname]["demandMisses"].append(value)
                if(metrics["demandAvgMissLatency"] in line):
                    value = line.split()[1]
                    if(dirname in benchmarks):
                        data[scheme][case][dirname]["demandAvgMissLatency"].append(value)
    #print(data)

def populate_data(final_data,bench):
    for k1,v1 in data.items():
        for k2,v2 in v1.items():
            assert len(v2["simSeconds"]) == 3, "missing data in simSeconds,{0},{1},{2}".format(k1,k2,bench)
            instructions = v2["simSeconds"]
            assert len(v2["numCycles"]) == 3, "missing data in numCycles,{0},{1},{2}".format(k1,k2,bench)
            cycles = v2["numCycles"]
            assert len(v2["demandMisses"]) == 3, "missing data in numCycles,{0},{1},{2}".format(k1,k2,bench)
            misses = v2["demandMisses"]
            #final_data[bench][k1][k2]["ipc"] = [int(x)/int(y) for x,y in zip(instructions, cycles)]
            final_data[bench][k1][k2]["ipc"] = cycles
            final_data[bench][k1][k2]["mpki"] = [(int(x)*1000)/int(y) for x,y in zip(misses, instructions)]
            final_data[bench][k1][k2]["latency"] = v2["demandAvgMissLatency"]

if __name__=="__main__":
    files = []
    get_files(files)
    data = {"nvm":{"case3":{"two":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "four":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "eight":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}},"rebuild":{"case3":{"two":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "four":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "eight":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}
    get_data(data, files)
    para = "simSeconds"
    with open('{0}_case3.csv'.format(para), 'w', newline='') as csvfile:
        fieldnames = ['scheme', 'two', 'four', 'eight']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for k1,v1 in data.items():
            for k2,v2 in v1.items():
                writer.writerow({'scheme':k1,'two':v2["two"][para][0],'four':v2["four"][para][0],
                    'eight':v2["eight"][para][0]})
