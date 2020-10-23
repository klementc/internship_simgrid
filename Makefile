SG_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/
NS_PATH=/opt/ns3/
#/home/clem/code/github.com/klementc/simgrid
PARAMS=  -L $(SG_PATH)/build/lib -lsimgrid -I $(SG_PATH)/build/include -I $(SG_PATH)/include -I $(SG_PATH)/ -I $(SG_PATH)/build/

all: service

service: ElasticTask.cpp main.cpp
	g++ $(PARAMS) $^ -o $@

