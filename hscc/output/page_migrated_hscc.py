import sys
import csv
import os
import sys

cwd = os.getcwd()
metrics = {"PageSelection":"dram_page_selection","PageCopy":"dram_page_copy","pagecount":"stack"}

def get_files( files):
    for (dirpath, dirnames, filenames) in os.walk(cwd):
        if('hscc/' in dirpath):
            for filename in filenames:
                if ("gemos.out" in filename):
                    filepath = os.path.join(dirpath,filename)
                    files.append(filepath)
    #print(files)

def get_data(data,files):
    for fil in files:
        scheme = 'hscc'
        interval = fil.split('/')[-3]
        dirname = fil.split('/')[-2]
        #print(fil, scheme, interval, dirname)
        with open(fil,"r") as f:
            for line in f:
                if(metrics["PageSelection"] in line):
                    l = line.replace(","," ")
                    value = l.replace(":"," ").split()[-3]
                    if(scheme == "hscc" and value != '0'):
                        data[scheme][interval][dirname]["PageSelection"].append(int(value))
                if(metrics["PageCopy"] in line):
                    l = line.replace(","," ")
                    value = l.replace(":"," ").split()[-1]
                    if(scheme == "hscc" and value != '0'):
                        data[scheme][interval][dirname]["PageCopy"].append(int(value))
                if(metrics["pagecount"] in line):
                    l = line.replace(","," ")
                    heap = l.replace(":"," ").split()[-1]
                    stack = l.replace(":"," ").split()[-3]
                    value = int(heap)+int(stack)
                    if(value != 0):
                        data[scheme][interval][dirname]["pagecount"].append(value)

    #print(data)


if __name__=="__main__":
    para = "ipc"
    final_data = {}
    files = []
    get_files(files)
    data = {"hscc":{"five":{"gapbs":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]}, "sssp":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]}, "workloadb":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]}},"twentyfive":{"gapbs":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]},"sssp":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]},"workloadb":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]}},"fifty":{"gapbs":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]},"sssp":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]},"workloadb":{"PageSelection":[],"PageCopy":[],"pagecount":[],"demandAvgMissLatency":[]}}}}
    get_data(data, files)
    #print(data["hscc"])
    para = "pagecount"
    with open('table4.tsv'.format(para), 'w', newline='') as csvfile:
        fieldnames = ['Benchmark', 'Th-5', 'Th-25', 'Th-50']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter='\t', lineterminator='\n')
        writer.writeheader()
        #print(final_data)
        d = {}
        for k1,v1 in data['hscc'].items():
            if not k1 in d:
                d[k1] = {}
            for k2,v2 in v1.items():
                d[k1][k2] = sum(v2[para])
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
                name = 'Gapbs_pr'
            elif(k1 == 'sssp'):
                name = 'G500_sssp'
            elif(k1 == 'workloadb'):
                name = 'Ycsb_mem'
            writer.writerow({'bench':name,'Th-5':v1['five'],'Th-25':v1["twentyfive"],'Th-50':v1["fifty"]})
