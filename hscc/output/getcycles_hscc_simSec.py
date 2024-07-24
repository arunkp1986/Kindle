import sys
import csv
import os
import sys

cwd = os.getcwd()

metrics = {"simSeconds":"simSeconds","numCycles":"system.cpu.numCycles","demandMisses":"system.cpu.dcache.demandMisses::cpu.data","demandAvgMissLatency":"system.l3.demandAvgMissLatency::cpu.data"}


directories = ["hscc","ssp_threads"]
benchmarks = ["gapbs","sssp", "workloadb"]

def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if('hscc/' in dirpath):
            for filename in filenames:
                if ("stats.txt" in filename):
                    filepath = os.path.join(dirpath,filename)
                    files.append(filepath)
    #print(files)

def get_data(data,files):
    for fil in files:
        scheme = "hscc"
        interval = fil.split('/')[-3]
        dirname = fil.split('/')[-2]
        #print(fil, scheme, interval, dirname)
        with open(fil,"r") as f:
            for line in f:
                if(metrics["simSeconds"] in line):
                    value = float(line.split()[1])
                    data[scheme][interval][dirname]["simSeconds"].append(value)
                if(metrics["numCycles"] in line):
                    value = line.split()[1]
                    data[scheme][interval][dirname]["numCycles"].append(value)
                if(metrics["demandMisses"] in line):
                    value = line.split()[1]
                    data[scheme][interval][dirname]["demandMisses"].append(value)
                if(metrics["demandAvgMissLatency"] in line):
                    value = line.split()[1]
                    data[scheme][interval][dirname]["demandAvgMissLatency"].append(value)
    #print(data)

if __name__=="__main__":
    para = "ipc"
    final_data = {}
    files = []
    get_files(files)
    data = {"hscc":{"five":{"gapbs":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "sssp":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "workloadb":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}},"twentyfive":{"gapbs":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"sssp":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"workloadb":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}},"fifty":{"gapbs":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"sssp":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]},"workloadb":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}},"nomigration":{"gapbs":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "sssp":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}, "workloadb":{"simSeconds":[],"numCycles":[],"demandMisses":[],"demandAvgMissLatency":[]}}}}
    get_data(data, files)
    #print(data["ssp_threads"])
    para = "simSeconds"
    with open('hscc_threshold.tsv'.format(para), 'w', newline='') as csvfile:
        fieldnames = ['#bench', 'th-5', 'th-25', 'th-50']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames,delimiter='\t', lineterminator='\n')
        writer.writeheader()
        #print(final_data)
        d = {}
        for k1,v1 in data['hscc'].items():
            if(k1 != 'nomigration'):
                if not k1 in d:
                    d[k1] = {}
                for k2,v2 in v1.items():
                    d[k1][k2] = round(v2[para][1]/data['hscc']['nomigration'][k2][para][1],9)
        #print(d)
        x = {}
        for k1,v1 in d.items():
            for k2,v2 in v1.items():
                if not k2 in x:
                    x[k2] = {}
                x[k2][k1] = v2
        #print(x)
        for k1,v1 in x.items():
            name = 'x'
            if(k1 == 'gapbs'):
                name = 'Gapbs\\\_pr'
            elif(k1 == 'sssp'):
                name = 'G500\\\_sssp'
            elif(k1 == 'workloadb'):
                name = 'Ycsb\\\_mem'
            writer.writerow({'#bench':name,'th-5':v1['five'],'th-25':v1["twentyfive"],'th-50':v1["fifty"]})

