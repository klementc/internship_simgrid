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


def cleanNames(G,nodeList):
    s=""
    for n in nodeList:
        s+="\n\t- %s"%(G.nodes[n]["label"])
    return s

def verifyNoMissingServs(G1, G2):
    l1 = []
    l2 = []
    for n in G1.nodes():
        l1.append(G1.nodes[n]["label"])
    for n in G2.nodes():
        l2.append(G2.nodes[n]["label"])

    print("MISSING:"+str(set(l1) ^ set(l2)))

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


def groupNodesWithSameServ(graph, root, already_list=[]):
    graphOut = nx.DiGraph()
    subg = codeGen.create_subgraph(graph, root)

    # add root and then add dst of each edge
    rootServ = graph.nodes[root]["label"]
    rootId = rootServ
    while(rootId in already_list):
        rootId += "_"
    already_list.append(rootId)
    graphOut.add_node(rootId)
    nx.set_node_attributes(graphOut,{rootId:{"serv":rootServ,"label":rootServ,"id":rootId,"dur":int(graph.nodes[root]["dur"])}})

    currentServ = {"serv":rootServ,"id":rootId}


    edges = nx.dfs_edges(subg, source = root)
    for src, dst in edges:
        srcChildNodes = list(subg.successors(src))
        if(len(srcChildNodes)>1):
            # add grouped graph for each child
            for succ in srcChildNodes:
                gChild, alistCh = groupNodesWithSameServ(graph, succ, already_list=already_list)
                already_list += alistCh
                graphOut.add_nodes_from(gChild.nodes(data=True))
                graphOut.add_edges_from(gChild.edges(data=True))
                # add link between src and succ
                rootChild =  [n for n,d in gChild.in_degree() if d==0][0]
                graphOut.add_edge(currentServ["id"],gChild.nodes[rootChild]["id"])
            return (graphOut, already_list)
        else:
            # already in this service, add duration
            if(graph.nodes[dst]["label"] == currentServ["serv"]):
                graphOut.nodes[currentServ["id"]]["dur"] += int(graph.nodes[dst]["dur"])
            # new service, add service and edge
            else:
                dstServ = graph.nodes[dst]["label"]
                dstId = dstServ
                while(dstId in already_list):
                    dstId += "_"
                already_list.append(dstId)
                graphOut.add_node(dstId)
                nx.set_node_attributes(graphOut,{dstId:{"serv":dstServ,"label":dstServ,"id":dstId,"dur":int(graph.nodes[dst]["dur"])}})
                graphOut.add_edge(currentServ["id"], dstId)
                currentServ = {"serv":dstServ,"id":dstId}

    return (graphOut, already_list)

def sequentializeOrNot(graph, root, seqLeafs=None):
    graphOut = nx.DiGraph()
    #print("enter seqornot:",graph.nodes(data=True))
    #print("root: ",root)
    subg = codeGen.create_subgraph(graph, root)
    leafList = []

    if(subg.number_of_nodes() == 1):
        print("Only one node, add it and return")
        leaf = [nodeKey for nodeKey in subg.nodes()][0]
        leafList.append(leaf)
        return subg, leafList
    else:
        # it means the subgraph has edges, process for each edge
        edges = nx.dfs_edges(subg, source = root)
        for src,dst in edges:
            graphOut.add_node(src)
            nx.set_node_attributes(graphOut,{src:graph.nodes[src]})
            #print(src+"->"+dst)
            srcChildNodes = list(subg.successors(src))
            if(len(srcChildNodes)==1):
                print("Sequence, no problem add: %s %s",graph.nodes[dst],list(graph.successors(dst)))
                graphOut.add_node(dst)
                nx.set_node_attributes(graphOut,{dst:graph.nodes[dst]})
                graphOut.add_edge(src,dst)
                # if no child to dst, add it to the leaflist
                if(len(list(graph.successors(dst)))==0):
                    print("ADDD LEAFF")
                    leafList.append(dst)
            else:
                print("FORK %d nodes"%(len(srcChildNodes)))
                # create subgraph for all childs and link them together
                prevChildLeaves = []
                parallel = query_yes_no("Execute in parallel?\nNode: %s\nChilds: %s\n"%(subg.nodes[src]["label"], cleanNames(subg, srcChildNodes)))
                for succ in srcChildNodes:
                    # create subgraph
                    #print("create subgraph: ",graph.nodes[succ])
                    gChild, leavesChild = sequentializeOrNot(graph, succ)
                    #print("Fork adds: %s"%(gChild.nodes(data=True)))
                    graphOut.add_nodes_from(gChild.nodes(data=True))
                    graphOut.add_edges_from(gChild.edges(data=True))

                    # if in parallel, attach child graph to the src node
                    if(parallel):
                        #print("Add edge: %s -> %s"%(src, succ))
                        graphOut.add_edge(src, succ)
                    else:
                        # TODO
                        # if first child:
                        #   add to src
                        # elif only one leaf in previous child:
                        #   add new child to previous leaf and new leaf = child.leaf
                        # else: (multiple leaves in previous child)
                        #   choose node to add to  (or take longest)
                        if(len(prevChildLeaves) == 0):
                            graphOut.add_edge(src,succ)
                            prevChildLeaves = leavesChild
                            print("CHHHH: %s"%(prevChildLeaves))
                        elif(len(prevChildLeaves) == 1):
                            graphOut.add_edge(prevChildLeaves[0], succ)
                            prevChildLeaves = leavesChild
                            print("CIIIIIII: %s"%(prevChildLeaves))
                        else:
                            longest = nx.dag_longest_path(gChild,weight="dur")
                            print("LONGEST: %s"%(longest))
                            query_yes_no("TODO, WILL ADD TO %s "%prevChildLeaves[0])
                            graphOut.add_edge(longest[-1], succ)
                            prevChildLeaves = longest[-1]
                # GET LONGEST PATH  IN CURRENT GRAPH IF REQUIRED IN PARENT CALL
                longest = nx.dag_longest_path(graphOut,weight="dur")[-1]
                return graphOut, [longest] # TODO RETURN CORRECT CHILDS

    return graphOut, leafList

def compactGraph(graph, already_list=[], previous={"serv":"","id":""}):
    '''
    Transforms a dot graph in a compact form
    idea:
    - perform a depth-first traversal of the graph
    - if 2 directly linked nodes are from the same service, add their duration
    - when going back, transform the graph into a sequence
    '''
    G = graph
    GCompact = nx.DiGraph()
    if(G.number_of_nodes() == 1):
        print("1 NODE ")
        verifyNoMissingServs(G, GCompact)
        return (GCompact, already_list)
    root = [n for n,d in G.in_degree() if d==0] [0]
    nodes = nx.dfs_edges(G, source = root)
    #previous = {"serv":"","id":""}
    for n in nodes:
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
            par = query_yes_no("/!\\ Node %s has more than one child. Please say if childrens are to be executed in parallel (y) or not (n) /!\\\nChildrens: %s\n"%(G.nodes[n[0]]["label"], cleanNames(G,list(G.successors(n[0])))))
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

                verifyNoMissingServs(G, GCompact)
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
            #GCompact.nodes[previous["id"]]["label"]+=" dur:"+str(GCompact.nodes[previous["id"]]["dur"])

            GCompact.add_edge(previous["id"], nodeName)
            previous = {"serv":toServ, "id":nodeName}

        # once we went through a node, mark it so that we don't compute several times the same flops if going back and forth
        G.nodes[n[0]]["visited"]=True

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
                for n in processedGraph.nodes():
                    processedGraph.nodes[n]["label"]+=" dur: "+str(processedGraph.nodes[n]["dur"])
                processedGraphPathImg = path+"_processed.png"
                processedGraphPathDot = path+"_processed.dot"
                print("Save output graph as image to '%s', dot file to '%s'"%(processedGraphPathImg, processedGraphPathDot))
                toDotAndMakeImg(processedGraph, processedGraphPathDot, processedGraphPathImg)
                graphDict[reqName] = processedGraph

                print("Sum of times in the original file: %d"%(getSumOfTimes(G)))
                print("Sum of times in the the processed graph: %d"%(getSumOfTimes(processedGraph)))

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
            #GCompact = compactGraph(G)[0]
            g, _ = sequentializeOrNot(G, root = [n for n,d in G.in_degree() if d==0][0])
            toDotAndMakeImg(g, "aaa.dot", "aaa.png")
            g2, _ = groupNodesWithSameServ(g, [n for n,d in g.in_degree() if d==0][0])
            toDotAndMakeImg(g2, "aaa2.dot", "aaa2.png")

            print("Sum of times in the original file: %d"%(getSumOfTimes(G)))
            print("Sum of times in the the processed graph: %d"%(getSumOfTimes(g)))
            print("Sum of times in the the grouped graph: %d"%(getSumOfTimes(g2)))

            d = codeGen.generateFullSimulationCode({"COMPOSE": g2})

            with open("generated.cpp", "w") as text_file:
                text_file.write(d)

    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)


