import networkx as nx
import argparse
from graphviz import render
from networkx.drawing.nx_agraph import write_dot

def showGraphStr(fName):
    G = nx.drawing.nx_agraph.read_dot(fName)
    #G = graph
    print("Nodes and their attributes:")
    for node_id in G.nodes:
        print(G._node[node_id])

def toImage(fName, outName):
    print("Render %s to %s"%(fName, outName))
    render('dot', 'png', fName)

def dfsTest(graph):
    #G = nx.drawing.nx_agraph.read_dot(fName)
    G = graph
    print("------------")
    print(list(G.nodes))
    print(list(G.edges))
    nodes = nx.dfs_edges(G)
    for n in nodes:
        #print(n)
        print("%s -> %s"%(G.nodes[n[0]],G.nodes[n[1]]))
    print("------------")

def toDotAndMakeImg(graph, fName):
    graph.graph['graph']={'rankdir':'LR'}
    write_dot(graph, fName)
    toImage(fName, fName+".png")

def compactGraph(graph):
    '''
    Transforms a dot graph in a compact form
    idea:
    - perform a depth-first traversal of the graph
    - if 2 directly linked nodes are from the same service, add their duration
    - when going back, transform the graph into a sequence
    '''
    G = graph
    GCompact = nx.Graph()

    nodes = nx.dfs_edges(G)
    already_list=[]
    previous = {"serv":"","id":""}
    dur = 0
    for n in nodes:
        #print for debug
        #dfsTest(GCompact)
        fromServ = G.nodes[n[0]]["label"]
        toServ = G.nodes[n[1]]["label"]
        # create nodes and edges if needed
        if fromServ != previous["serv"]:
            nodeName=fromServ
            while(nodeName in already_list):
                nodeName+="_"
            print("Add node: %s"%(nodeName))
            GCompact.add_node(nodeName)
            already_list.append(nodeName)
            # if it's a goback, add edge
            if(previous["id"]!=""):
                GCompact.add_edge(previous["id"], nodeName)


            previous = {"serv":fromServ,"id":nodeName}
            GCompact.nodes[previous["id"]]["label"]=fromServ
            # don't add dur if it is a go back to a forking node
            if( not "visited" in G.nodes[n[0]]):
                GCompact.nodes[previous["id"]]["dur"]=int(G.nodes[n[0]]["dur"])
            else:
                # maybe 0, manage it in the code generator after
                GCompact.nodes[previous["id"]]["dur"]=0


        if toServ == previous["serv"]:
            print("Add dur %s %s"%(previous["id"], G.nodes[n[1]]["dur"]))
            GCompact.nodes[previous["id"]]["dur"]+=int(G.nodes[n[1]]["dur"])

        if toServ != fromServ:
            nodeName=toServ
            while(nodeName in already_list):
                nodeName+="_"
            print("Add node: %s"%(nodeName))
            GCompact.add_node(nodeName)
            already_list.append(nodeName)

            GCompact.nodes[nodeName]["label"]=toServ
            GCompact.nodes[nodeName]["dur"]=int(G.nodes[n[1]]["dur"])

            print("Add edge: %s -> %s"%(previous["id"], nodeName))
            # add dur to label just for display purpose
            GCompact.nodes[previous["id"]]["label"]+=" dur:"+str(GCompact.nodes[previous["id"]]["dur"])

            GCompact.add_edge(previous["id"], nodeName)
            previous = {"serv":toServ, "id":nodeName}

        # once we went through a node, mark it so that we don't compute several times the same flops if going back and forth
        G.nodes[n[0]]["visited"]=True
    GCompact.nodes[previous["id"]]["label"]+=" dur:"+str(GCompact.nodes[previous["id"]]["dur"])
    return GCompact

if __name__=="__main__":
    try:
        parser = argparse.ArgumentParser()
        # Adding optional argument
        parser.add_argument("-f", "--filename", help = "dot file to parse", required=True)

        # Read arguments from command line
        args = parser.parse_args()
        G = nx.drawing.nx_agraph.read_dot(args.filename)
        toImage(args.filename, "test.png")
        dfsTest(G)
        print("aaaaaaa")
        dfsTest(compactGraph(G))
        toDotAndMakeImg(compactGraph(G),"zizi.dot")

    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)


