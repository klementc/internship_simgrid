import networkx as nx
import argparse
from graphviz import render
from networkx.drawing.nx_agraph import write_dot
import os
import sys
import codeGen

# copied from https://stackoverflow.com/questions/3041986/apt-command-line-interface-like-yes-no-input, I find it fun
def query_yes_no(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is True for "yes" or False for "no".
    """
    valid = {"yes": True, "y": True, "ye": True,
             "no": False, "n": False}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "
                             "(or 'y' or 'n').\n")


def showGraphStrFromF(fName):
    G = nx.drawing.nx_agraph.read_dot(fName)
    showGraphStr(G)

def showGraphStr(G):
    #G = graph
    print("Nodes and their attributes:")
    for node_id in G.nodes:
        print("%s: %s"%(node_id,G._node[node_id]))

def toImage(fName, outName):
    print("Render %s to %s"%(fName, outName))
    render('dot', 'png', fName)

def dfsTest(graph):
    #G = nx.drawing.nx_agraph.read_dot(fName)
    G = graph
    print("------------ Nodes in a DFS traversal:")
    print(list(G.nodes))
    print(list(G.edges))
    root = [n for n,d in G.in_degree() if d==0] [0]
    nodes = nx.dfs_edges(G, source = root)
    for n in nodes:
        #print(n)
        print("%s -> %s"%(G.nodes[n[0]],G.nodes[n[1]]))
    print("------------")

def toDotAndMakeImg(graph, fName, imgName=None):
    graph.graph['graph']={'rankdir':'LR'}
    write_dot(graph, fName)
    # compacted image:
    if(imgName == None):
        imgName = fName+".png"

    toImage(fName, imgName)

def getSumOfTimes(graph):
    sum = 0
    for n in graph.nodes:
        sum += int(graph._node[n]["dur"])

    return sum

def compactGraph(graph, already_list=[], previous={"serv":"","id":""}):
    '''
    Transforms a dot graph in a compact form
    idea:
    - perform a depth-first traversal of the graph
    - if 2 directly linked nodes are from the same service, add their duration
    - when going back, transform the graph into a sequence
    '''
    #print("enter compact, already list: %s"%(already_list))
    G = graph
    GCompact = nx.DiGraph()
    #dfsTest(graph)
    if(G.number_of_nodes() == 1):
        print("1 NODE ")
        return (GCompact, already_list)
    root = [n for n,d in G.in_degree() if d==0] [0]
    nodes = nx.dfs_edges(G, source = root)
    #previous = {"serv":"","id":""}
    for n in nodes:
        #print for debug
        fromServ = G.nodes[n[0]]["label"]
        toServ = G.nodes[n[1]]["label"]
        # create nodes and edges if needed
        if fromServ != previous["serv"]:
            nodeName=fromServ
            while(nodeName in already_list):
                nodeName+="_"
            #print("Add node: %s"%(nodeName))
            GCompact.add_node(nodeName)
            GCompact.nodes[nodeName]["serv"]=fromServ

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

        # if several childs, ask if compact or not and do the appropriate action if yes
        if(len(list(G.successors(n[0])))>1 and not "par" in G.nodes[n[0]]):
            par = query_yes_no("/!\ Node %s has more than one child. Please say if childrens are to be executed in parallel (y) or not (n) /!\\\nChildrens: %s\n"%(n[0], list(G.successors(n[0]))))
            if(not par):
                G.nodes[n[0]]["par"]="no"
            else:
                # add each child and add call compact on each subgraph
                for succ in list(G.successors(n[0])):
                    nodeName=G.nodes[succ]["label"]
                    while(nodeName in already_list):
                        nodeName+="_"
                    #already_list.append(nodeName)
                    #print(already_list)
                    print(nodeName)
                    #print("add succ for %s"%(nodeName))
                    GCompact.add_node(nodeName)
                    GCompact.nodes[nodeName]["label"]=G.nodes[succ]["label"]
                    GCompact.nodes[nodeName]["serv"]=G.nodes[succ]["label"]
                    GCompact.nodes[nodeName]["dur"]=int(G.nodes[succ]["dur"])
                    #wGCompact.add_edge(previous["id"], nodeName)

                    ch = codeGen.create_subgraph(G,succ)
                    #dfsTest(ch)
                    #print("call compact for %s"%(codeGen.create_subgraph(G,succ).nodes()))
                    GCompactchild, already_list = compactGraph(ch,already_list=already_list, previous={"serv":"","id":""})
                    GCompact.add_nodes_from(GCompactchild.nodes(data=True),)
                    GCompact.add_edges_from(GCompactchild.edges())
                    GCompact.add_edge(previous["id"], nodeName)
                    already_list.append(nodeName)

                return (GCompact,already_list)

        if toServ == previous["serv"]:
            #print("Add dur %s %s"%(previous["id"], G.nodes[n[1]]["dur"]))
            GCompact.nodes[previous["id"]]["dur"]+=int(G.nodes[n[1]]["dur"])

        if toServ != fromServ:
            nodeName=toServ
            while(nodeName in already_list):
                nodeName+="_"
            #print("Add node: %s"%(nodeName))
            GCompact.add_node(nodeName)
            already_list.append(nodeName)

            GCompact.nodes[nodeName]["label"]=toServ
            GCompact.nodes[nodeName]["serv"]=toServ
            GCompact.nodes[nodeName]["dur"]=int(G.nodes[n[1]]["dur"])

            #print("Add edge: %s -> %s"%(previous["id"], nodeName))
            # add dur to label just for display purpose
            GCompact.nodes[previous["id"]]["label"]+=" dur:"+str(GCompact.nodes[previous["id"]]["dur"])

            GCompact.add_edge(previous["id"], nodeName)
            previous = {"serv":toServ, "id":nodeName}

        # once we went through a node, mark it so that we don't compute several times the same flops if going back and forth
        G.nodes[n[0]]["visited"]=True
    #GCompact.nodes[previous["id"]]["label"]+=" dur:"+str(GCompact.nodes[previous["id"]]["dur"])

    return (GCompact, already_list)

if __name__=="__main__":
    try:
        parser = argparse.ArgumentParser()
        # Adding optional argument
        parser.add_argument("-f", "--filename", help = "dot file to parse", required=False)
        parser.add_argument("-o", "--outFname", help="output image file name", required=False)
        parser.add_argument("-g", "--generate", help="generates code for the given file if set", required=False)
        parser.add_argument("-s", "--sum", help="compare sum of times between uncompacted and compact graphs if set", required=False)
        parser.add_argument("-i", "--interactive", help="Provide your traces and create your simulation code interractively", action='store_true')

        # Read arguments from command line
        args = parser.parse_args()

        graphDict = {}
        # interractive part
        if(args.interactive):
            print("""Welcome to the simulation code generator.\nBefore executing this you need to obtain jaeger traces of the requests you\nwant in your simulator as a dot file. You can obtain them from this modified jaeger-ui: https://github.com/klementc/jaeger-ui""")
            while(query_yes_no("Do you want to add a new trace to your simulator?")):
                print("Enter the request name associated to this trace : ",end="")
                reqName = input()
                print("Enter the path to your file : ", end="")
                path = input()
                G = nx.drawing.nx_agraph.read_dot(path)

                print("Processing dot graph for request %s (file: %s)"%(reqName, path))
                processedGraph = compactGraph(G)[0]
                processedGraphPathImg = path+"_processed.png"
                processedGraphPathDot = path+"_processed.dot"
                print("Save output graph as image to '%s', dot file to '%s'"%(processedGraphPathImg, processedGraphPathDot))
                toDotAndMakeImg(processedGraph, processedGraphPathDot, processedGraphPathImg)
                graphDict[reqName] = processedGraph


            print("All traces processed. Now generating code for :")
            for k in graphDict:
                print("\t- Trace %s"%(k))
            print("Please give the name of the code file to produce : ", end="")
            codeFile = input()
            code = codeGen.generateFullSimulationCode(graphDict)

            with open(codeFile, "w") as text_file:
                text_file.write(code)

            print("=--------------------------------------------------=\nCode generated successfully to '%s'"%(codeFile))
            print("You now need to add your dataSources to the simulation code before running it!")
        else:
            G = nx.drawing.nx_agraph.read_dot(args.filename)
            GCompact = compactGraph(G)[0]

            if(args.generate):
                d = codeGen.generateFullSimulationCode({"COMPOSE": G})

                with open("generated.cpp", "w") as text_file:
                    text_file.write(d)

            if(args.sum):
                print("Sum of times in the original file: %d"%(getSumOfTimes(G)))
                print("Sum of times in the the processed graph: %d"%(getSumOfTimes(GCompact)))

    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)


