SG_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/
NS_PATH=/opt/ns3/
JAEG=~/Code/github.com/jaegertracing/jaeger-client-cpp
#/home/clem/code/github.com/klementc/simgrid
PARAMS= -ljaegertracing -lopentracing -I/usr/local/include/jaegertracing/ -lyaml-cppd -L /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib -isystem /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/include -L $(SG_PATH)/build/lib -lsimgrid -I $(SG_PATH)/build/include -I $(SG_PATH)/include -I $(SG_PATH)/ -I $(SG_PATH)/build/


# export LD_LIBRARY_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/build/lib:/usr/local/lib/:/home/clem/Code/github.com/jaegertracing/jaeger-client-cpp/::/home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib

all: service fifa graph test

service: ElasticTask.cpp ElasticPolicyHybrid1.cpp ElasticPolicyCPUThreshold.cpp main.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

fifa: ElasticTask.cpp ElasticPolicyHybrid1.cpp ElasticPolicyCPUThreshold.cpp FifaExperiment.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

graph: ElasticTask.cpp ElasticPolicyHybrid1.cpp ElasticPolicyCPUThreshold.cpp GraphExperiment.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

test: ElasticTask.cpp ElasticPolicyHybrid1.cpp ElasticPolicyCPUThreshold.cpp test.cpp ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@
