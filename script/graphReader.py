import networkx as nx
import argparse
from graphviz import render

def showGraphStr(fName):
    G = nx.drawing.nx_agraph.read_dot(fName)
    print("Nodes and their attributes:")
    for node_id in G.nodes:
        print(G._node[node_id])

def toImage(fName, outName):
    print("Render %s to %s"%(fName, outName))
    render('dot', 'png', fName)

def dfsTest(fName):
    G = nx.drawing.nx_agraph.read_dot(fName)
    nodes = nx.dfs_edges(G)
    for n in nodes:
        #print(n)
        print("%s -> %s"%(G.nodes[n[0]],G.nodes[n[1]]))

def compactGraph(fName):
    '''
    Transforms a dot graph in a compact form
    idea:
    - perform a depth-first traversal of the graph
    - if 2 directly linked nodes are from the same service, add their duration
    - when going back, transform the graph into a sequence
    '''

if __name__=="__main__":
    try:
        parser = argparse.ArgumentParser()
        # Adding optional argument
        parser.add_argument("-f", "--filename", help = "dot file to parse", required=True)

        # Read arguments from command line
        args = parser.parse_args()
        showGraphStr(args.filename)
        toImage(args.filename, "test.png")
        dfsTest(args.filename)

    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)


