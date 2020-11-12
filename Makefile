SG_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/
NS_PATH=/opt/ns3/
#/home/clem/code/github.com/klementc/simgrid
PARAMS=  -L $(SG_PATH)/build/lib -lsimgrid -I $(SG_PATH)/build/include -I $(SG_PATH)/include -I $(SG_PATH)/ -I $(SG_PATH)/build/

all: service fifa

service: ElasticTask.cpp ElasticPolicyCPUThreshold.cpp main.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

fifa: ElasticTask.cpp ElasticPolicyCPUThreshold.cpp FifaExperiment.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@