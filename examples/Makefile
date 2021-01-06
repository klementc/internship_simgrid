SG_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/
NS_PATH=/opt/ns3/
JAEG=~/Code/github.com/jaegertracing/jaeger-client-cpp
#/home/clem/code/github.com/klementc/simgrid
PARAMS= -ljaegertracing -lopentracing -I/usr/local/include/jaegertracing/ -lyaml-cppd -L /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib -isystem /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/include -L $(SG_PATH)/build/lib -lsimgrid -I $(SG_PATH)/build/include -I $(SG_PATH)/include -I $(SG_PATH)/ -I $(SG_PATH)/build/


# export LD_LIBRARY_PATH=~/Code/framagit.org/simgridOfficiel/simgrid/build/lib:/usr/local/lib/:/home/clem/Code/github.com/jaegertracing/jaeger-client-cpp/::/home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib

all: service fifa graph test

service: ../elasticity/ElasticTask.cpp ../elasticity/ElasticPolicyHybrid1.cpp ../elasticity/ElasticPolicyCPUThreshold.cpp main.cpp ../elasticity/ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

fifa: ../elasticity/ElasticTask.cpp ../elasticity/ElasticPolicyHybrid1.cpp ../elasticity/ElasticPolicyCPUThreshold.cpp FifaExperiment.cpp ../elasticity/ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

graph: ../elasticity/ElasticTask.cpp ../elasticity/ElasticPolicyHybrid1.cpp ../elasticity/ElasticPolicyCPUThreshold.cpp GraphExperiment.cpp ../elasticity/ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@

test: ../elasticity/ElasticTask.cpp ../elasticity/ElasticPolicyHybrid1.cpp ../elasticity/ElasticPolicyCPUThreshold.cpp test.cpp ../elasticity/ElasticPolicyReactive1.cpp
	g++ $(PARAMS) $^ -o $@
