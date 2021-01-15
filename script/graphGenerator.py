import yaml
from graphviz import Digraph


class yamlDesc():
  def __init__(self, path):
    with open(path) as f:
      self.graphDesc = yaml.load(f, Loader=yaml.FullLoader)
      self.serviceList = [e for e in self.graphDesc["services"]]
      self.polList = [e for e in self.graphDesc["policies"]]

  def genDot(self):
    f = Digraph('graph services', filename='graph.gv')
    f.attr(rankdir='LR')
    for e in self.graphDesc["services"]:
      for g in self.graphDesc["services"][e]["mailboxesOut"]:
        f.edge(e, g)
    f.render(filename='graph.dot')

  def retFunName(self, name):
    return "return"+name

  def etmName(self, name):
    return "etm"+name

  def polName(self, name):
    return "pol"+name

  def genCodeServiceRet(self, name, serviceDesc):
    """
      Generates the C++ code of the return function of an ElasticTaskManager
    """
    s = "void "+self.retFunName(name)+"(TaskDescription* td) {\n\tXBT_DEBUG(\"Return function of service "+name+"\");\n"
    for out in serviceDesc["mailboxesOut"]:
      # get mailbox and send taskdescription to output
      s+="\ts4u_Mailbox* m"+out+" = s4u_Mailbox::by_name(\""+out+"\");\n"
      s+="\tm"+out+"->put(td, td->dSize);\n}\n"
    return s

  def genCodeServiceInit(self, name, serviceDesc):
    """
      Generates the C++ code corresponding to an ElasticTaskManager
    """
    mboxIn = serviceDesc["mailboxesIn"]
    # create the object and its associated actor
    s="std::vector<std::string> v"+name+" = std::vector<std::string>();\n"
    for mb in mboxIn:
      s+="v"+name+".push_back(\""+mb+"\");\n"

    s += "std::shared_ptr<simgrid::s4u::ElasticTaskManager> "+self.etmName(name)+" = std::make_shared<simgrid::s4u::ElasticTaskManager>(\""+self.etmName(name)+"\",v"+name+");\n"

    # set service properties
    s+=self.etmName(name)+"->setProcessRatio("+str(serviceDesc["processRatio"])+");\n"
    s+=self.etmName(name)+"->setOutputFunction("+self.retFunName(name)+");\n"
    s+=self.etmName(name)+"->setBootDuration("+str(serviceDesc["instanceBootDuration"])+");\n"
    s+=self.etmName(name)+"->setDataSizeRatio("+str(serviceDesc["dataRatio"])+");\n"

    s += "simgrid::s4u::Actor::create(\""+self.etmName(name)+"_a\", simgrid::s4u::Host::by_name(\""+serviceDesc["managerNode"]+"\"), ["+self.etmName(name)+"] { "+self.etmName(name)+"->run(); });\n"

    # add initial hosts
    for host in serviceDesc["initNodes"]:
      s+=self.etmName(name)+"->addHost(simgrid::s4u::Host::by_name(\""+host+"\"));\n"



    return s+"\n"


  def genCodePolicy(self, name, polDesc):
    """
      Generates the C++ code to intialize an elastic policy
    """
    # create the policy object
    s="\tsimgrid::s4u::ElasticPolicyCPUThreshold* "+self.polName(name)+" = new simgrid::s4u::"+polDesc["type"]+"("+",".join(map(str, polDesc["params"]))+");\n"

    # add hosts to the policy's pool
    for host in polDesc["nodePool"]:
      s+=self.polName(name)+"->addHost(simgrid::s4u::Host::by_name(\""+host+"\"));\n"

    # add ETMs to the policy
    for etm in polDesc["services"]:
      s+=self.polName(name)+"->addElasticTaskManager("+self.etmName(etm)+".get());\n"

    # create policy actor
    s+="simgrid::s4u::Actor::create(\""+self.polName(name)+"_a\", simgrid::s4u::Host::by_name(\""+polDesc["executingNode"]+"\"), ["+self.polName(name)+"] {"+self.polName(name)+"->run(); });"

    return s+"\n"

  def genKillCode(self):
    """
      Destruction code to use in the end of the experiment
    """
    s="\n// kill policies and ETMs\nXBT_INFO(\"Done. Killing policies and etms\");\n"
    s+="simgrid::s4u::this_actor::sleep_for(600);"
    for pol in self.graphDesc["policies"]:
      s+=self.polName(pol)+"->kill();\n"
    for etm in self.graphDesc["services"]:
      s+=self.etmName(etm)+"->kill();\n"
    return s

  def sendingTimestamps(self):
    """
      Code to send timestamps to the first mailbox of the application
    """
    s="s4u_Mailbox* mb = s4u_Mailbox::by_name(\""+self.graphDesc["timestamps"]["mailboxIn"]+"\");\n"
    s+="boost::uuids::random_generator generator;\nstd::ifstream file;\ndouble a;\n"
    s+="file.open(\""+self.graphDesc["timestamps"]["filePath"]+"\");\n"
    # remove first line, useful for csvs
    s+="std::string s; file>>s;\n"
    s+="while (file >> a) {\nif(a > simgrid::s4u::Engine::get_clock())\n\tsimgrid::s4u::this_actor::sleep_until(a);\n"
    s+="TaskDescription* t = new TaskDescription(generator(), -1, 0);\n"
    # TODO change size here according to user choice
    s+="t->dSize=1;\nmb->put(t, t->dSize);\n}"

    return s

  def generateRun(self):
    s="void run() {\nXBT_INFO(\"Starting run()\");\n"
    for etm in self.graphDesc["services"]:
      s+=self.genCodeServiceInit(etm, self.graphDesc["services"][etm])
    for pol in self.graphDesc["policies"]:
      s+=self.genCodePolicy(pol, self.graphDesc["policies"][pol])

    s+= self.sendingTimestamps()

    s+=self.genKillCode()

    return s+"\n}\n"

  def generateMain(self):
    s="int main(int argc, char* argv[]) {\n\tsimgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);\n\te->load_platform(argv[1]);\n"
    s+="\tsimgrid::s4u::Actor::create(\"main\", simgrid::s4u::Host::by_name(YOUHAVETOSETTHIS), [&]{run();});\n\te->run();\n\treturn 0;\n}"
    return s

  def genFullCode(self):
    # includes
    s="#include <simgrid/s4u/Actor.hpp>\n#include <simgrid/s4u/Host.hpp>\n#include <simgrid/s4u/Mailbox.hpp>\n#include <simgrid/s4u/Engine.hpp>\n"
    s+="#include \"ElasticPolicy.hpp\"\n#include \"ElasticTask.hpp\"\n"

    s+="XBT_LOG_NEW_DEFAULT_CATEGORY(run_log, \"logs of the experiment\");\n\n"

    for ser in self.graphDesc["services"]:
      s+=self.genCodeServiceRet(ser, self.graphDesc["services"][ser])
    s+=self.generateRun()
    s+=self.generateMain()

    return s
y = yamlDesc("service_platform.yaml")
#print(y.graphDesc)
#print(y.polList)
#print(y.serviceList)
#print(y.genCodeServiceRet("service1", y.graphDesc["services"]["service1"]))

#print(y.genCodeServiceInit("service1", y.graphDesc["services"]["service1"]))

#print(y.genCodePolicy("cpuPol1", y.graphDesc["policies"]["cpuPol1"]))

#print(y.generateRun())

print(y.genFullCode())
y.genDot()