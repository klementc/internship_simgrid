import networkx as nx
import argparse
import os
import sys

def genOutputFunctionSwitchCode(graph, requestType):
  '''
  Generates the switch case for each node of the graph
  to be pasted in the output function (redirecting taskRequests
  to the next service depending on the hop value and actual service)
  '''
  mapCode={}
  hop = 1
  edges = nx.dfs_edges(graph)
  for edge in edges:
    outputServName = graph.nodes[edge[0]]["serv"]
    dstName = graph.nodes[edge[1]]["serv"]
    print("generate code for: %s request: %s"%(edge[0], requestType))
    print("%s (serv %s) sends to %s"%(edge[0],  outputServName, edge[1]))

    # code to be used
    code = """
    if(nbHops == %d){
      XBT_DEBUG("Output Function for %s, put to %s");
      s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("%s");
    	m_user_timeline->put(td, td->dSize);
    }
    """ % (hop, requestType, dstName, dstName)

    # add it to the map
    if(not outputServName in mapCode):
      mapCode[outputServName] = "\ncase RequestType::%s:"%(requestType)

    mapCode[outputServName] += code
    # used to differentiate multiple calls to the same service
    hop+=1

    # if the dst node is the last node of the sequence, add its code now
    if(len(list(graph.successors(edge[1]))) == 0):
      print("%s is the last node"%(edge[1]))
      # code to be used
      code ="""
    if(nbHops == %d){
      XBT_DEBUG("Output Function for %s, Final service, DELETE");
      delete td;
      // TODO DELETE JAEGER SPANS
    }
    """% (hop, requestType)
      mapCode[graph.nodes[edge[1]]["serv"]] += code

  # add break;
  for k in mapCode:
    print("add break to %s"%(k))
    mapCode[k]+="\nbreak;\n"

  return mapCode

def genETMSwitchFunction(graph, requestType):
  '''
  Generates the code to be pasted in the switch function given to ETMs
  so that they know the amount of computation to perform depending on the
  request type and hop value
  '''
  mapCode={}
  hop = 1
  edges = nx.dfs_edges(graph)
  for edge in edges:
    outputServName = graph.nodes[edge[0]]["serv"]
    dur = graph.nodes[edge[0]]["dur"]
    print("generate flop amount code for: %s request: %s"%(edge[0], requestType))
    print("%s (service: %s cost: %s)"%(edge[0],  outputServName, dur))

    # code to be used
    code = """
    if(nbHops == %d){XBT_DEBUG("Entered cost Function for %s"); return %s;}
    """ % (hop, requestType, dur*1000)

    # add it to the map
    if(not outputServName in mapCode):
      mapCode[outputServName] = "\ncase RequestType::%s:"%(requestType)

    mapCode[outputServName] += code

    hop += 1

    # if the dst node is the last node of the sequence, add its code now
    if(len(list(graph.successors(edge[1]))) == 0):
      print("%s is the last node"%(edge[1]))
      # code to be used
      code ="""
    if(nbHops == %d){XBT_DEBUG("Entered cost Function for %s"); return %s;}
    """% (hop, requestType, graph.nodes[edge[1]]["dur"]*1000)
      mapCode[graph.nodes[edge[1]]["serv"]] += code

  # add break;
  for k in mapCode:
    print("add break to %s"%(k))
    mapCode[k]+="\nbreak;\n"

  return mapCode

def genETMInitCode(graphs):
  '''
  ETM constructor for a given list of graphs
  (only 1 ETM if a service is in multiple graphs (i.e. different requests))
  '''
  servList = []
  for graph in graphs:
    # for each graph, add service if not already added to servList
    edges = nx.dfs_edges(graph)
    for edge in edges:
      s1Name = graph.nodes[edge[0]]["serv"]
      s2Name = graph.nodes[edge[1]]["serv"]
      if(not s1Name in servList):
        servList.append(s1Name)
      if(not s2Name in servList):
        servList.append(s2Name)

  # add the code for each service
  print("%d different services"%(len(servList)))
  code = ""
  hostIndex=1
  for service in servList:
    print("Generate constructor for service %s"%(service))
    code += """
    // create ETM for service %s
    std::vector<std::string> v_serv_%s = std::vector<std::string>();
    v_serv_%s.push_back("%s");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_%s = std::make_shared<simgrid::s4u::ElasticTaskManager>("%s",v_serv_%s);
    serv_%s->setBootDuration(0);
    serv_%s->setDataSizeRatio(1);
    serv_%s->setOutputFunction(return_%s);
    serv_%s->setProcessRatioFunc(pr_%s);
    serv_%s->addHost(simgrid::s4u::Host::by_name("cb1-%s"));
    simgrid::s4u::Actor::create("etm_%s", simgrid::s4u::Host::by_name("cb1-%s"), [serv_%s] { serv_%s->run(); });

    """%(service,service,service,service,service,service,service,service,service,service,service,service,service,service,hostIndex,service,hostIndex,service,service)
    hostIndex+=1
  return code

def generateFullSimulationCode(mapReqNameToGraph):
  '''
  Generates almost ready to run code given a map of requestName->requestGraph
  - each graph must be a compacted graph of a request
  - don't give more than one graph for the same request

  This function gives as an output:
  - the return function
  - the pr functions (used by ETMs and instances to know how much computation is required)
  - construction of the ETMs

  Do by hand:
  - add hosts on the machines you want (1 example on cb1-#number by the automatic tool)
  - add dataSources
  '''
  code = ""

  # add imports

  # add output return functions

  # add pr functions

  # add constructors

  return code
