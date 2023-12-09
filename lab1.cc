#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct consumers_infos{
  string consumer;
  string interest_per_second;
  string app_duration_time;
  string prefix;  
};
typedef struct consumers_infos consumers_infos;

struct producers_infos{
  string producer;
  string prefix;
};
typedef struct producers_infos producers_infos;

struct links_infos{
  int from_node;
  int to_node;
};
typedef struct links_infos links_infos;

namespace ns3 {

int main(int argc, char* argv[])
{
  // config para
  int nums_nodes, nums_lines, nums_consumers, nums_producers;
  vector<links_infos> line_rec;
  vector<consumers_infos> consumers_rec;
  vector<producers_infos> producers_rec;

  // read the input
  cout<<"Input your topology config : ";
  string path;
  getline(cin, path);
  path = "./Lab1_Checker/scenario/" + path;
  ifstream in(path);
  if(!in.is_open()){
    cout<<"Can not read input file ..."<<endl;
    return 1; // error num
  }

  // config the requirements
  string s;
  getline(in, s);
  nums_nodes = stoi(s);
  getline(in, s);
  nums_lines = stoi(s);
  getline(in, s);
  nums_consumers = stoi(s);
  getline(in, s);
  nums_producers = stoi(s);

  // config the lines
  for(int i=0;i<nums_lines;i++){
    getline(in, s);
    stringstream ss(s);
    vector<string> v;
    string line_node;
    while(ss >> line_node){
      v.push_back(line_node);
    }
    links_infos l_info;
    l_info.from_node = stoi(v[0]);
    l_info.to_node = stoi(v[1]); 
    line_rec.push_back(l_info);
  }

  // config the consumers
  for(int i=0;i<(int)nums_consumers;i++){
    getline(in, s);
    stringstream ss(s);
    vector<string> v;
    string ndn_info;
    while(ss >> ndn_info){
      v.push_back(ndn_info);
    }
    consumers_infos c_info;
    c_info.consumer = v[0];
    c_info.interest_per_second = v[1];
    c_info.app_duration_time = v[2];
    c_info.prefix = v[3];
    consumers_rec.push_back(c_info);
  }

  // config the consumers
  for(int i=0;i<(int)nums_producers;i++){
    getline(in, s);
    stringstream ss(s);
    vector<string> v;
    string ndn_info;
    while(ss >> ndn_info){
      v.push_back(ndn_info);
    }
    producers_infos p_info;
    p_info.producer = v[0];
    p_info.prefix = v[1];
    producers_rec.push_back(p_info);
  }

  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(nums_nodes);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  for(int i=0;i<(int)line_rec.size();i++){
    p2p.Install(nodes.Get(line_rec[i].from_node), nodes.Get(line_rec[i].to_node));
  }

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  /*
    To DO 1 : Please use multicast !
  */
  ndn::StrategyChoiceHelper::InstallAll("/prefix","/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::InstallAll("/postfix","/localhost/nfd/strategy/multicast");
  // consumer
  /*
    To Do 2 : Implement the consumer setting
  */
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  //consumers_rec: nodeid; frequency; exe_time; prefix
  for(int i = 0;i < (int)nums_consumers;i++){
    consumerHelper.SetPrefix(consumers_rec[i].prefix);
    consumerHelper.SetAttribute("Frequency", StringValue(consumers_rec[i].interest_per_second));
    auto apps = consumerHelper.Install(nodes.Get(stoi(consumers_rec[i].consumer)));  
    apps.Stop(Seconds(stod(consumers_rec[i].app_duration_time)));
  }
  // Producer
  /*
    To Do 3 : Implement the producer setting
  */
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  for(int i = 0;i < (int)nums_producers;i++){
    producerHelper.SetPrefix(producers_rec[i].prefix);
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(nodes.Get(stoi(producers_rec[i].producer)));
  }
  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}

