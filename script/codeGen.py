import networkx as nx
import argparse
import os
import sys
import graphReader



def genOutputFunctionSwitchCode(graph, requestType, hop = 1):
  '''
  Generates the switch case for each node of the graph
  to be pasted in the output function (redirecting taskRequests
  to the next service depending on the hop value and actual service)
  '''
  mapCode={}
  root = [n for n,d in graph.in_degree() if d==0] [0]
  edges = nx.dfs_edges(graph,source = root)

  # take care of lonely node after fork
  if(graph.number_of_nodes() == 1):
    servName = list(graph.nodes(data=True))[0][1]["serv"]
    print("ENNDDDD %s "%(list(graph.nodes)))
    # TODO DELETIONS
    code = """
    \ncase RequestType::%s:
    if(td->nbHops == %d){
      XBT_DEBUG("Output Function %s (FINAL NODE, NO CHILD!)");
      // TODO DELETE
    }
    """ % (requestType, hop, servName)
    if(not servName in mapCode):
      mapCode[servName] = code
    else:
      mapCode[servName] += code


  for edge in edges:
    print("edge: ",edge)
    # if more than one successor, it means we have parallel spans
    # in this case, add the output code for this requesttype, and call each of the successors
    if(len(list(graph.successors(edge[0]))) > 1):
      print("FORRRRRK")
      srcServName = graph.nodes[edge[0]]["serv"]
      # generate the code for this node with one put per parallel child
      code = """
      if(td->nbHops == %d){
        XBT_DEBUG("Output Function for %s (PARALLEL CHILDS)");
      """ % (hop, edge[0])
      # foreach successor, add its put code
      childs = list(graph.successors(edge[0]))
      for i in range(len(childs)):
        dst = graph.nodes[childs[i]]
        # code to be used
        code += """
          XBT_DEBUG("Put to %s");
          s4u_Mailbox* m_%s = s4u_Mailbox::by_name("%s");
          TaskDescription* td_%d = new TaskDescription(*td);
          td_%d->requestType = RequestType::%s_%d;
          m_%s->put(td_%d, td_%d->dSize);
        """ % (dst["serv"], dst["serv"], dst["serv"], i, i, requestType,i,dst["serv"],i,i)
        if(not dst["serv"] in mapCode):
          mapCode[srcServName] = "\ncase RequestType::%s:"%(requestType)
        mapCode[srcServName] += code
      mapCode[srcServName]+="\n}\nbreak;\n"

      # return the result of the maps of childrens and the current map
      for i in range(len(childs)):
        # ugly but easiest way I found for now
        subgraph = create_subgraph(graph,childs[i])
        graphReader.showGraphStr(subgraph)
        mapCode = mergeCodeMaps([mapCode, genOutputFunctionSwitchCode(subgraph,requestType+"_"+str(i),hop+1)])
      return mapCode
    # while we have sequential spans, no need to do something complex
    else:
      #graphReader.showGraphStr(graph)
      print("0:",graph.nodes[edge[0]])
      print("1:",graph.nodes[edge[1]]["serv"])
      outputServName = graph.nodes[edge[0]]["serv"]
      dstName = graph.nodes[edge[1]]["serv"]
      print("generate code for: %s request: %s"%(edge[0], requestType))
      print("%s (serv %s) sends to %s"%(edge[0],  outputServName, edge[1]))

      # code to be used
      code = """
      if(td->nbHops == %d){
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
      if(td->nbHops == %d){
        XBT_DEBUG("Output Function for %s, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      """% (hop, requestType)
        if(not graph.nodes[edge[1]]["serv"] in mapCode):
          mapCode[graph.nodes[edge[1]]["serv"]] = "\ncase RequestType::%s:"%(requestType)

        mapCode[graph.nodes[edge[1]]["serv"]] += code

  # add break;
  for k in mapCode:
    print("add break to %s"%(k))
    mapCode[k]+="""\nbreak;\n"""

  return mapCode

def genETMSwitchFunction(graph, requestType, hop = 1):
  '''
  Generates the code to be pasted in the switch function given to ETMs
  so that they know the amount of computation to perform depending on the
  request type and hop value
  '''
  mapCode={}

  # take care of lonely nodes after fork
  if(graph.number_of_nodes() == 1):
    servName = list(graph.nodes(data=True))[0][1]["serv"]
    dur=int(list(graph.nodes(data=True))[0][1]["dur"])
    print("ENNDDDD %s "%(list(graph.nodes)))
    code = """
    \ncase RequestType::%s:
    if(td->nbHops == %d){XBT_DEBUG("Entered cost Function for %s"); return %d;}
    """ % (requestType, hop, servName,int(dur)*1000)
    if(not servName in mapCode):
      mapCode[servName] = code
    else:
      mapCode[servName] += code

  root = [n for n,d in graph.in_degree() if d==0] [0]
  edges = nx.dfs_edges(graph,source = root)

  for edge in edges:
    print(edge)
    # if more than one successor, it means we have parallel spans
    # in this case, add the output code for this requesttype, and call each of the successors
    print("%d : %s"%(len(list(graph.successors(edge[0]))),list(graph.successors(edge[0]))))
    if(len(list(graph.successors(edge[0]))) > 1):
      print("FORRKKKKKK")
      outputServName = graph.nodes[edge[0]]["serv"]
      dur = int(graph.nodes[edge[0]]["dur"])
      #print("generate flop amount code for: %s request: %s"%(edge[0], requestType))
      #print("%s (service: %s cost: %s)"%(edge[0],  outputServName, dur))

      code ="""
      if(td->nbHops == %d){XBT_DEBUG("Entered cost Function for %s"); return %d;\n}\nbreak;\n
      """% (hop, outputServName, int(dur)*1000)
      if(not graph.nodes[edge[0]]["serv"] in mapCode):
        mapCode[graph.nodes[edge[0]]["serv"]] =  "\ncase RequestType::%s:"%(requestType)
      mapCode[graph.nodes[edge[0]]["serv"]] += code

      # return the result of the maps of childrens and the current map
      childs = list(graph.successors(edge[0]))
      for i in range(len(childs)):
        # ugly but easiest way I found for now
        subgraph = create_subgraph(graph,childs[i])
        print("nextgraph:")
        graphReader.showGraphStr(subgraph)
        mapCode = mergeCodeMaps([mapCode, genETMSwitchFunction(subgraph,requestType+"_"+str(i),hop+1)])
      return mapCode

    # while we have sequential spans, no need to do something complex
    else:
      outputServName = graph.nodes[edge[0]]["serv"]
      dur = int(graph.nodes[edge[0]]["dur"])
      print("generate flop amount code for: %s request: %s"%(edge[0], requestType))
      print("%s (service: %s cost: %s)"%(edge[0],  outputServName, dur))

      # code to be used
      code = """
      if(td->nbHops == %d){XBT_DEBUG("Entered cost Function for %s"); return %s;}
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
      if(td->nbHops == %d){XBT_DEBUG("Entered cost Function for %s (FINAL NODE, NO CHILD!)"); return %s;}
      """% (hop, requestType, int(graph.nodes[edge[1]]["dur"])*1000)
        if(not graph.nodes[edge[1]]["serv"] in mapCode):
          mapCode[graph.nodes[edge[1]]["serv"]] =  "\ncase RequestType::%s:"%(requestType)

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
    root = [n for n,d in graph.in_degree() if d==0] [0]
    edges = nx.dfs_edges(graph,source = root)
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
  code += """
#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>
#include <simgrid/s4u/Comm.hpp>
#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"
#include "DataSource.hpp"
#include <memory>

XBT_LOG_NEW_DEFAULT_CATEGORY(run_log, "logs of the experiment");
"""

  # add output return functions
  code += "/* RETURN FUNCTIONS, AUTO GENERATED CODE, MODIFY IF YOU KNOW WHAT YOU WANT */"

  outputFunctionCodes = {}
  for k in mapReqNameToGraph:
    print("fetch output code for reques %s"%(k))
    cases = genOutputFunctionSwitchCode(mapReqNameToGraph[k], k)
    for serv in cases:
      # create header of func if not already done
      if(not serv in outputFunctionCodes):
        outputFunctionCodes[serv] = """
    void return_%s(TaskDescription* td) {
      XBT_DEBUG("Return function of service %s");
      switch (td->requestType)
      {"""%(serv,serv)

      # now add the case for this request
      outputFunctionCodes[serv] += cases[serv]
  # close the function
  for f in outputFunctionCodes:
    outputFunctionCodes[f]+="""\t}\n}"""
    code += outputFunctionCodes[f]

  # add pr functions
  code += "\n/* PR FUNCTIONS, AUTO GENERATED CODE, MODIFY IF YOU KNOW WHAT YOU WANT */"
  prFunctionCodes = {}
  for k in mapReqNameToGraph:
    print("fetch pr code for request %s"%(k))
    cases = genETMSwitchFunction(mapReqNameToGraph[k], k)
    for serv in cases:
      # create header of func if not already done
      if(not serv in prFunctionCodes):
        prFunctionCodes[serv] = """
    double pr_%s(TaskDescription* td) {
      XBT_DEBUG("pr function of service %s");
      switch (td->requestType)
      {"""%(serv, serv)

      # now add the case for this request
      prFunctionCodes[serv] += cases[serv]
  # close the function
  for f in prFunctionCodes:
    prFunctionCodes[f]+="""\t}//it should never end up here\nreturn -1;\n}"""
    code += prFunctionCodes[f]


  # add constructors
  code+="""
  void run() {
    XBT_INFO("Starting run()");
  """
  l = [mapReqNameToGraph[k] for k in mapReqNameToGraph ]
  code+=genETMInitCode(l)

  code+="""

  /* ADD DATASOURCES MANUALLY HERE, SET THE END TIMER AS YOU WISH, AND LAUNCH YOUR SIMULATOR*/

  // kill policies and ETMs
  simgrid::s4u::this_actor::sleep_for(150); /*set it according to your needs*/
  XBT_INFO("Done. Killing policies and etms");
  """
  servList = []
  for graph in l:
    # for each graph, add service if not already added to servList
    root = [n for n,d in graph.in_degree() if d==0] [0]
    edges = nx.dfs_edges(graph,source = root)
    for edge in edges:
      s1Name = graph.nodes[edge[0]]["serv"]
      s2Name = graph.nodes[edge[1]]["serv"]
      if(not s1Name in servList):
        servList.append(s1Name)
      if(not s2Name in servList):
        servList.append(s2Name)
  for serv in servList:
    code+="""
    serv_%s->kill();"""%(serv)

  code+="""
}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);

	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-200"), [&]{run();});
	e->run();
	return 0;
}
"""
  return code


def mergeCodeMaps(mapsToMerge):
  '''
  Used to sum the code in different maps (used when having parallel spans)
  '''
  finalMap = {}
  for m in mapsToMerge:
    for k in m:
      if(not k in finalMap):
        finalMap[k] = m[k]
      else:
        finalMap[k] += "\n"+m[k]

  return finalMap

# taken from https://stackoverflow.com/questions/32935510/networkx-digraph-create-subgraph-digraph-by-node
#def create_subgraph(G, node):
#    edges = nx.dfs_successors(G, node)
#    nodes = [node]
#    for k,v in edges.items():
#        nodes.extend([k])
#        nodes.extend(v)
#    return G.subgraph(nodes)
#def create_subgraph(Ga,sub_G,start_node):
#    for n in Ga.successors(start_node):
#        sub_G.add_edge(start_node,n)
#        create_subgraph(Ga,sub_G,n)

def create_subgraph(G, node):
  nodes = nx.single_source_shortest_path(G,node).keys()
  res = G.subgraph(nodes).copy()

  for i in G.out_edges(node, data=True):
    #print(i[0]," -> ",i[1])
    res.update([i])
    res.add_edge(i[0], i[1])
  for n in nodes:
    for j in G.out_edges(n, data=True):
      #print("%s -> %s"%(j[0],j[1]))
      res.add_edge(j[0], j[1])

  #print("IN GRAPH")
  #graphReader.dfsTest(G)
  #print("OUT GRAPH")
  #graphReader.dfsTest(res)
  return res