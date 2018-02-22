
#include "ns3/node.h"
#include "ns3/log.h"

#include "ns3/inet-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/string.h"
#include "ns3/data-rate.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/random-variable-stream.h"

#include "ns3/bridge-helper.h"
#include "ns3/bridge-net-device.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-sync-routing.h"
#include "ns3/ipv4-sync-routing-helper.h"
#include "ns3/csma-module.h"
#include "dc.h"
#include "ns3/hypervisor.h"
#include "ns3/vm.h"
#include "math.h"


using namespace std;


namespace ns3 {

	DC::DC () 
	{
		NS_LOG_FUNCTION (this);
		costLayer0 = 1;
		costLayer1 =3;
		costLayer2=5;
		cout << "fat tree topology created.\n";
	}

	DC::~DC () 
	{
		NS_LOG_FUNCTION (this);
		cout << "fat tree topology is destroyed.\n";
	}

	void DC::GetIps(NodeContainer container)
	{
		uint32_t id;
		for (NodeContainer::Iterator it = container.Begin (); it != container.End (); ++it) {
			Ptr<Node> node = *it;
			Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
			Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1,0);
			Ipv4Address addri = iaddr.GetLocal (); 

			id = node->GetId();
			//Address add = node->GetDevice.
			std::cout << id << "--" << addri << "\n";
		}
	}

	void DC::PrintHypList(void)
	{
		std::vector<Ptr <Hypervisor>>::const_iterator itr;

		for (itr = m_hypList.begin(); itr != m_hypList.end(); itr++) 
		{
			std::cout << "Hypervisor cpu: " << (*itr)->m_cap_cpu <<  ", Hypervisor mem: " << (*itr)->m_cap_mem << ", Hypervisor IP: " << (*itr)->GetHypAdd() <<", Hypervisor IP: " << (*itr)->GetNode()->GetId() <<  "\n";

		}
	}

	void DC::PrintVMCommunication(void)
	{
		std::vector<Ptr <VM>>::const_iterator itr;
		std::cout << "alvms count is:  " << allVms.size() << "\n";
		for (itr = allVms.begin(); itr != allVms.end(); itr++) 
		{
			std::cout << (*itr)->m_vmid << " ----> " << (*itr)->m_vmRemote->m_vmid << "\n";

		}
	}

	Ipv4Address DC::GetRandAddr(NodeContainer container, Ipv4Address nothisip)
	{
		uint32_t n = container.GetN();
		Ipv4Address remote;
		uint32_t r;

		do{
			r = rand() % n + 0;

			Ptr<Node> node = container.Get(r);

			Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
			Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1,0);
			remote = iaddr.GetLocal (); 


		} while (nothisip == remote);

		return remote;
	}



	void DC::CreateRandVms(uint32_t n)
	{

		// create hypervisors on all hosts
		NodeContainer container = this->GetHost();

		for (NodeContainer::Iterator it = container.Begin (); it != container.End (); ++it) {

			Ptr<Node> node = *it;

			this->CreateHyp(node);
		}

		std::vector<Ptr <Hypervisor>>::const_iterator itr;
		std::vector<Ptr <VM>>::const_iterator itrv;


		// create vm
		Ptr <VM> vm;
		Ptr <VM> vmRemote; 
		Ipv4Address remoteTest = Ipv4Address("10.3.1.4"); // node 43
		Ipv4Address remote = Ipv4Address("0.0.0.0");
		uint32_t vmIndex;
		Ptr <Node> node;
		Ptr<Ipv4> ipv4;
		Ipv4InterfaceAddress iaddr;


		for (uint32_t i=0; i<n; i++)
		{



			if (allVms.size()==0) // in case there is no vm installed, we create a vm and let it communicate with itself
				vmRemote = NULL; 
			else
			{
				vmIndex = rand() % allVms.size(); // get random vm from vm list
				vmRemote = allVms.at(vmIndex);
				node = vmRemote->GetNode();
				ipv4 = node->GetObject<Ipv4>();
				iaddr = ipv4->GetAddress (1,0);
				remote = iaddr.GetLocal (); // get the ip of the random chosen vm
			}

			uint32_t h = rand() % this->m_hypList.size(); // to get random hypervisor
			Ptr <Hypervisor> hyp = this->m_hypList.at(h);


			if (vmRemote == NULL) 
			{
				vm = hyp->CreateVM(i,1,512, vmRemote, remoteTest); // "CreateVM" method does not consider the 4th argument because it is null 
				vm->m_vmRemote = vm;
				allVms.push_back(vm);

				std::cout << "create vm in " <<hyp->GetNode()->GetId() << " communicating with: " << remoteTest << std::endl  ;
			}
			else
			{
				vm = hyp->CreateVM(i,1,512, vmRemote, remote);
				allVms.push_back(vm);
				std::cout << "create vm in: "<< hyp->GetNode()->GetId() << " communicating with: " << remote << std::endl; ;
			}
		}

		/*
		for (itrv = allVms.begin(); itrv != allVms.end(); itrv++) // remove the vm that communicates with itself from the list of vm (??)
		{
		Ptr <VM> vm = (*itrv);
		if (vm->m_vmid == vm->m_vmRemote->m_vmid)
		allVms.erase(std::remove(allVms.begin(), allVms.end(), vm), allVms.end());
		}
		*/


	}


	std::vector<Ptr <VM>> DC::GetFlows(Ptr <VM> vi)
	{
		std::vector<Ptr <VM>> flows;
		std::vector<Ptr <VM>>::const_iterator itr;
		for (itr = allVms.begin(); itr != allVms.end(); itr++) 
		{
			Ptr <VM> vm = (*itr);
			if (vm == vi)
				flows.push_back(vm->m_vmRemote);
			else if (vm->m_vmRemote == vi)
				flows.push_back(vm);
		}
		return flows;
	}

	std::vector<Ptr <VM>> DC::GetNextComVMGroup()
	{
		std::vector<Ptr <VM>> group;
		std::vector<Ptr <VM>> flows;
		std::vector<Ptr <VM>> flows1;
		std::vector<Ptr <VM>> vms = allVms;

		std::vector<Ptr <VM>>::const_iterator itrv;
		for (itrv = vms.begin(); itrv != vms.end(); itrv++) // remove the vm that communicates with itself from the list of vm (??)
		{
			Ptr <VM> vm = (*itrv);
			if (vm->m_vmid == 0)
				vms.erase(std::remove(vms.begin(), vms.end(), vm), vms.end());
		}


		Ptr <VM> vi;
		Ptr <VM> vp;
		Ptr <VM> flow;

		if (vms.size() != 0)
		{
			vi = vms.at(0);
			group.push_back(vi);

			// determine flows related to vi
			flows = GetFlows(vi);

			// while loop
			while(flows.size() != 0)
			{
				flow = flows.at(0);
				if (!(std::find(group.begin(), group.end(), flow) != group.end()))
					vp = flow;
				else if (!(std::find(group.begin(), group.end(), flow->m_vmRemote) != group.end()))
					vp = flow->m_vmRemote;
				else
					break;

				group.push_back(vp);
				flows1 = GetFlows(vp);
				flows.reserve(flows.size() + flows1.size());
				flows.insert(flows.end(), flows1.begin(), flows1.end());
				flows.erase(std::remove(flows.begin(), flows.end(), flow), flows.end());
			}
		}

		return group;
	}

	void DC::Print(std::vector<Ptr <VM>> group)
	{
		std::cout << "\n----------\n";
		Ptr <VM> vm;
		std::vector<Ptr <VM>>::const_iterator itr;
		for (itr = group.begin(); itr != group.end(); itr++) 
		{
			vm = (*itr);
			std::cout << "\n" << vm->m_vmid << " --> " << vm->m_vmRemote->m_vmid << "\n";
		}
	}



	char * DC::toString(int a,int b, int c, int d)
	{

		char * ip = new char[30];

		char firstOctet[30], secondOctet[30], thirdOctet[30], fourthOctet[30];


		int first = a;
		int second = b;
		int third = c;
		int fourth = d;

		memset(ip,0,30);
		_snprintf(firstOctet,10,"%d",first);
		strcat(firstOctet,".");

		_snprintf(secondOctet,10,"%d",second);
		strcat(secondOctet,".");
		_snprintf(thirdOctet,10,"%d",third);
		strcat(thirdOctet,".");
		_snprintf(fourthOctet,10,"%d",fourth);

		strcat(thirdOctet,fourthOctet);
		strcat(secondOctet,thirdOctet);
		strcat(firstOctet,secondOctet);

		strcat(ip,firstOctet);


		return ip;
	}


	void DC::CreateTopo(void)
	{

		int i = 0;
		int j = 0;
		int h = 0;

		const int k = 4;
		const int num_pod = k;                // number of pod
		const int num_host = (k/2);           // number of hosts under a switch
		const int num_edge = (k/2);           // number of edge switch in a pod
		const int num_bridge = num_edge;      // number of bridge in a pod
		const int num_agg = (k/2);            // number of aggregation switch in a pod
		const int num_group = k/2;            // number of group of core switches
		const int num_core = (k/2);           // number of core switch in a group
		const int total_host = k*k*k/4;       // number of hosts in the entire network

		// Output some useful information //
		std::cout << "Value of k =  "<< k<<"\n";
		std::cout << "Total number of hosts =  "<< total_host<<"\n";
		std::cout << "Number of hosts under each switch =  "<< num_host<<"\n";
		std::cout << "Number of edge switch under each pod =  "<< num_edge<<"\n";
		std::cout << "------------- "<<"\n";


		InternetStackHelper internet;
		




		//======= Creation of Node Containers ===========//
		//
		NodeContainer core[num_group];                          // NodeContainer for core switches
		for (i=0; i<num_group;i++){
			core[i].Create (num_core);
			internet.Install (core[i]);
			m_core.Add(core[i]);
		}
		m_allnodes.Add(m_core);

		NodeContainer agg[num_pod];                             // NodeContainer for aggregation switches
		for (i=0; i<num_pod;i++){
			agg[i].Create (num_agg);
			internet.Install (agg[i]);
			m_agg.Add(agg[i]);
		}
		m_allnodes.Add(m_agg);

		NodeContainer edge[num_pod];                            // NodeContainer for edge switches
		for (i=0; i<num_pod;i++){
			edge[i].Create (num_bridge);
			internet.Install (edge[i]);
			m_edge.Add(edge[i]);
		}
		m_allnodes.Add(m_edge);

		NodeContainer bridge[num_pod];                          // NodeContainer for edge bridges
		for (i=0; i<num_pod;i++){
			bridge[i].Create (num_bridge);
			internet.Install (bridge[i]);
			m_bridge.Add(bridge[i]);
		}
		//m_allnodes.Add(m_core);

		NodeContainer host[num_pod][num_bridge];                // NodeContainer for hosts
		for (i=0; i<num_pod;i++){
			for (j=0;j<num_bridge;j++){
				host[i][j].Create (num_host);
				internet.Install (host[i][j]);
				m_host.Add(host[i][j]);
			}
		}
		m_allnodes.Add(m_host);



		// Inintialize Address Helper
		//
		Ipv4AddressHelper address;


		// Initialize parameters for Csma and PointToPoint protocol
		//
		char dataRate [] = "1000Mbps";	// 1Gbps
		int delay = 0.001;		// 0.001 ms


		// Initialize PointtoPoint helper
		//
		PointToPointHelper p2p;
		p2p.SetDeviceAttribute ("DataRate", StringValue (dataRate));
		p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (delay)));

		// Initialize Csma helper
		//
		CsmaHelper csma;
		csma.SetChannelAttribute ("DataRate", StringValue (dataRate));
		csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (delay)));



		//=========== Connect edge switches to hosts ===========//
		//

		//NetDeviceContainer hostSw[num_pod][num_bridge];
		NetDeviceContainer hostSw[num_pod][num_edge][num_host];
		//NetDeviceContainer bridgeDevices[num_pod][num_bridge];
		//Ipv4InterfaceContainer ipContainer[num_pod][num_bridge];
		Ipv4InterfaceContainer ipContainer[num_pod][num_edge][num_host];

		for (i=0;i<num_pod;i++){
			for (j=0;j<num_edge; j++){
				//NetDeviceContainer link1 = csma.Install(NodeContainer (edge[i].Get(j), bridge[i].Get(j)));
				//hostSw[i][j].Add(link1.Get(0));
				//bridgeDevices[i][j].Add(link1.Get(1));
				for (h=0; h< num_host;h++){
					hostSw[i][j][h] = p2p.Install(edge[i].Get(j), host[i][j].Get(h));
					//NetDeviceContainer link2 = csma.Install (NodeContainer (host[i][j].Get(h), bridge[i].Get(j)));
					//hostSw[i][j].Add(link2.Get(0));
					//bridgeDevices[i][j].Add(link2.Get(1));
					m_alldevices.Add(hostSw[i][j][h]);

					//BridgeHelper bHelper;
					//bHelper.Install (bridge[i].Get(j), bridgeDevices[i][j]);
					//Assign address
					//char *subnet;
					//subnet = toString(10, i, j, 0);
					//address.SetBase (subnet, "255.255.255.0");
					int second_octet = i;
					int third_octet = j;
					int fourth_octet;
					if (h==0) fourth_octet = 1;
					else fourth_octet = h*2+1;
					//Assign subnet
					char *subnet;
					subnet = toString(10, second_octet, third_octet, 0);
					//Assign base
					char *base;
					base = toString(0, 0, 0, fourth_octet);
					address.SetBase (subnet, "255.255.255.0",base);
					ipContainer[i][j][h] = address.Assign(hostSw[i][j][h]);
				}
			}
		}
		//std::cout << "!!!!!" << m_alldevices.GetN() << "!!!!\n";
		std::cout << "Finished connecting edge switches and hosts  "<< "\n";


		//=========== Connect aggregate switches to edge switches ===========//
		//
		NetDeviceContainer ae[num_pod][num_agg][num_edge];
		Ipv4InterfaceContainer ipAeContainer[num_pod][num_agg][num_edge];
		for (i=0;i<num_pod;i++){
			for (j=0;j<num_agg;j++){
				for (h=0;h<num_edge;h++){
					ae[i][j][h] = p2p.Install(agg[i].Get(j), edge[i].Get(h));

					m_alldevices.Add(ae[i][j][h]);

					int second_octet = i;
					int third_octet = j+(k/2);
					int fourth_octet;
					if (h==0) fourth_octet = 1;
					else fourth_octet = h*2+1;
					//Assign subnet
					char *subnet;
					subnet = toString(10, second_octet, third_octet, 0);
					//Assign base
					char *base;
					base = toString(0, 0, 0, fourth_octet);
					address.SetBase (subnet, "255.255.255.0",base);
					ipAeContainer[i][j][h] = address.Assign(ae[i][j][h]);
				}
			}
		}
		std::cout << "Finished connecting aggregation switches and edge switches  "<< "\n";

		//=========== Connect core switches to aggregate switches ===========//
		//
		NetDeviceContainer ca[num_group][num_core][num_pod];
		Ipv4InterfaceContainer ipCaContainer[num_group][num_core][num_pod];
		int fourth_octet =1;

		for (i=0; i<num_group; i++){
			for (j=0; j < num_core; j++){
				fourth_octet = 1;
				for (h=0; h < num_pod; h++){
					ca[i][j][h] = p2p.Install(core[i].Get(j), agg[h].Get(i));

					m_alldevices.Add(ca[i][j][h]);

					int second_octet = k+i;
					int third_octet = j;
					//Assign subnet
					char *subnet;
					subnet = toString(10, second_octet, third_octet, 0);
					//Assign base
					char *base;
					base = toString(0, 0, 0, fourth_octet);
					address.SetBase (subnet, "255.255.255.0",base);
					ipCaContainer[i][j][h] = address.Assign(ca[i][j][h]);
					fourth_octet +=2;
				}
			}
		}
		std::cout << "Finished connecting core switches and aggregation switches  "<< "\n";
		std::cout << "------------- "<<"\n";



	}




	void DC::CreateHyp(Ptr <Node> node)
	{
		Ptr <Hypervisor> hyp = CreateObject<Hypervisor>();
		hyp->InstallHyp(node);
		m_hypList.push_back(hyp);
	}



	bool DC::IsHost(Ptr <Node> node)
	{
		bool exist = false;
		for (uint32_t i=0; i<m_host.GetN(); i++)
		{
			if (m_host.Get(i) == node)
				exist = true;
		}

		return exist;
	}

	bool DC::IsEdge(Ptr <Node> node)
	{
		bool exist = false;
		for (uint32_t i=0; i<m_edge.GetN(); i++)
		{
			if (m_edge.Get(i) == node)
				exist = true;
		}

		return exist;
	}

	bool DC::IsAgg(Ptr <Node> node)
	{
		bool exist = false;
		for (uint32_t i=0; i<m_agg.GetN(); i++)
		{
			if (m_agg.Get(i) == node)
				exist = true;
		}

		return exist;
	}

	bool DC::IsCore(Ptr <Node> node)
	{
		bool exist = false;
		for (uint32_t i=0; i<m_core.GetN(); i++)
		{
			if (m_core.Get(i) == node)
				exist = true;
		}

		return exist;
	}




	int DC::GetPodId(Ptr <Node> node)
	{
		int pod;
		int k = 4;

		int num_pod = k;                // number of pod
		int num_host = (k/2);           // number of hosts under a switch
		int num_edge = (k/2);           // number of bridge in a pod
		int num_agg = (k/2);            // number of aggregation switch in a pod
		int num_group = k/2;            // number of group of core switches
		int num_core = (k/2);     

		if (IsAgg(node))
			pod = (node->GetId() - (num_core*num_group)) / num_core;
		else if (IsEdge(node))
			pod = (node->GetId() - (num_core*num_group) - (num_pod*num_agg)) / num_core;
		else if (IsHost(node))
			pod = (node->GetId() - (num_core*num_group) - 3*(num_pod*num_agg)) / (num_host*num_edge);
		else pod = -1;

		return pod;
	}



	int DC::GetCoreGroup(Ptr <Node> node)
	{
		int group;
		int k = 4;
		int num_pod = k;                // number of pod
		int num_host = (k/2);           // number of hosts under a switch
		int num_edge = (k/2);           // number of bridge in a pod
		int num_agg = (k/2);            // number of aggregation switch in a pod
		int num_group = k/2;            // number of group of core switches
		int num_core = (k/2);   

		if (IsCore(node))
			group = node->GetId() / num_core;
		else if (IsAgg(node))
			group = (node->GetId()-(num_core*num_group)) % num_core;
		else if (IsEdge(node))
			group = (node->GetId() / (num_group*num_core) - (num_pod*num_agg)) % num_core;
		else if (IsHost(node))
			group = ((node->GetId() / (num_group*num_core) - 2*(num_pod*num_agg)) / num_host) % num_core;
		else group = -1;

		return group;
	}


	Ptr <Node> DC::GetNode(uint32_t n)
	{
		Ptr <Node> node = NULL;

		for (uint32_t i=0; i<m_allnodes.GetN(); i++)
			if (m_allnodes.Get(i)->GetId() == n)
				node = m_allnodes.Get(i);

		return node;
	}

	uint32_t DC::CostBetween2Nodes(uint32_t i, uint32_t j)
	{
		Ptr <Node> node1=GetNode(i);
		Ptr <Node> node2=GetNode(j);

		uint32_t d = abs(GetCoreGroup(node1)-GetCoreGroup(node2));;

		uint32_t cost = 0;

		if ( node1 == node2) cost = 0;

		else if ((IsHost( node1) && IsHost( node2)))
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				if (d==0)
					cost = costLayer0 *2;
				else
					cost = 2*costLayer0 + 2*costLayer1;
			}
			else
			{
				cost = 2*costLayer0 + 2*costLayer1 + 2*costLayer2;
			}
		}
		else if ((IsHost( node1) && IsEdge( node2)) || (IsHost( node2) && IsEdge(node1)))
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				if (d==0)
					cost = costLayer0;
				else 
					cost = costLayer0 + 2*costLayer1;
			}
			else
			{
				cost = costLayer0 + 2* costLayer1 + 2*costLayer2;
			}
		}
		else if ((IsHost(node1) && IsAgg( node2)) || (IsHost( node2) && IsAgg(node1)))
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				cost = costLayer0 + costLayer1;
			}
			else
			{
				cost = costLayer0 + costLayer1 + 2*costLayer2;
			}
		}
		else if ((IsHost( node1) && IsCore( node2)) || (IsHost( node2) && IsCore(node1)))
		{
			cost = costLayer0 + costLayer1 + costLayer2;
		}
		else if ((IsEdge( node1) && IsAgg( node2)) || (IsEdge( node2) && IsAgg(node1))) //
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				cost=costLayer1;
			}
			else
			{
				cost=costLayer1 + 2*costLayer2;
			}
		}
		else if ((IsEdge( node1) && IsEdge( node2)))
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				cost = 2*costLayer1;
			}
			else
			{
				cost = 2*costLayer1 + 2*costLayer2;
			}
		}
		else if ((IsEdge( node1) && IsCore( node2)) || (IsEdge( node2) && IsCore( node1)) )
		{
			cost = costLayer1 + costLayer2;
		}
		else if ((IsAgg( node1) && IsCore( node2)) || (IsAgg( node2) && IsCore(node1)))
		{
			if (d==0)
				cost = costLayer2;
			else 
				cost = 2*costLayer1 + costLayer2;


		}             
		else if ((IsAgg( node1) && IsAgg( node2)))
		{
			if (GetPodId( node1) == GetPodId( node2))
			{
				cost = 2*costLayer1;
			}
			else
			{
				if (d==0)
					cost = 2*costLayer2;
				else
					cost = 2*costLayer1 + 2 * costLayer2;
			}
		}             
		else if ((IsCore( node1) && IsCore( node2)))
		{
			if (d==0)
				cost = 2*costLayer2;
			else
				cost = 2*costLayer1 + 2*costLayer2;
		}

		return cost;

	}



	/*
	uint32_t DC::CostBetween2Nodes(uint32_t i, uint32_t j)
	{
	Ptr <Node> node1=GetNode(i);
	Ptr <Node> node2=GetNode(j);

	int d = abs(GetCoreGroup(node1)-GetCoreGroup(node2));;

	uint32_t cost;
	if ( node1 == node2) cost = 0;
	else if ((IsHost( node1) && IsHost( node2)))
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	if (d==0)
	cost = 2*costLayer0;
	else if (d%2==0)
	cost = 2*costLayer0 + (d/2)*(2*costLayer1);
	else if ( d%2 != 0)
	cost = 2*costLayer0 + (((d-1)/2)+1)*(2*costLayer1);
	}
	else
	{
	if (d==0)
	cost = 2*costLayer0 + 2*costLayer2 + 2*costLayer1;
	else if (d%2==0)
	cost = 2*costLayer0 + costLayer1 + 2*costLayer2 +  ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = 2*costLayer0 + costLayer1 + 2*costLayer2 + ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	}
	else if ((IsHost( node1) && IsEdge( node2)) || (IsHost( node2) && IsEdge(node1)))
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	if (d==0)
	cost = costLayer0;
	else if (d%2==0)
	cost = costLayer0 + (d/2)*(2*costLayer1);
	else if ( d%2 != 0)
	cost = costLayer0 + (((d-1)/2)+1)*(2*costLayer1);
	}
	else
	{
	if (d==0)
	cost = costLayer0 + 2*costLayer2 + 2*costLayer1;
	else if (d%2==0)
	cost = costLayer0 + costLayer1 + 2*costLayer2 +  ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = costLayer0 + costLayer1 + 2*costLayer2 + ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	}
	else if ((IsHost(node1) && IsAgg( node2)) || (IsHost( node2) && IsAgg(node1)))
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	if (d==0)
	cost = costLayer0 + costLayer1;
	else if (d%2==0)
	cost = costLayer0 +  ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = costLayer0 + ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	else
	{
	if (d==0)
	cost = costLayer0 + 2*costLayer2 + costLayer1;
	else if (d%2==0)
	cost = costLayer0 +  2*costLayer2 + ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = costLayer0 + 2*costLayer2 + ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	}
	else if ((IsHost( node1) && IsCore( node2)) || (IsHost( node2) && IsCore(node1)))
	{
	if (d==0)
	cost = costLayer0 + costLayer2 + costLayer1;
	else if (d%2==0)
	cost = costLayer0 +  costLayer2 + ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = costLayer0 + costLayer2 + ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	else if ((IsEdge( node1) && IsAgg( node2)) || (IsEdge( node2) && IsAgg(node1))) //
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	if (d==0)
	cost = costLayer1;
	else if (d%2==0)
	cost = ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	else
	{
	if (d==0)
	cost = costLayer1 + 2* costLayer2;
	else if (d%2==0)
	cost = 2* costLayer2 + ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = 2* costLayer2+ ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	}
	else if ((IsEdge( node1) && IsEdge( node2)))
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	if (d==0)
	cost = 2*costLayer1;
	else if (d%2==0)
	cost = (d/2)*(2*costLayer1);
	else if ( d%2 != 0)
	cost = (((d-1)/2)+1)*(2*costLayer1);
	}
	else
	{
	if (d==0)
	cost = 2*costLayer1 + 2* costLayer2;
	else if (d%2==0)
	cost = costLayer1 + 2* costLayer2 + ((d/2)*(2*costLayer1)) + costLayer1 ;
	else if ( d%2 != 0)
	cost = costLayer1 + 2* costLayer2+ ((((d-1)/2)+1)*(2*costLayer1)) - costLayer1;
	}
	}
	else if ((IsAgg( node1) && IsCore( node2)) || (IsAgg( node2) && IsCore(node1)))
	{
	if (d==0)
	cost = costLayer2;
	else if (d%2==0)
	cost = (d/2)*(2*costLayer1);
	else if ( d%2 != 0)
	cost = (((d-1)/2)+1)*(2*costLayer1);

	if (GetCoreGroup(node1)==GetCoreGroup(node2))
	cost = costLayer2;
	else
	cost = 2*costLayer1 + costLayer2;
	}             
	else if ((IsAgg( node1) && IsAgg( node2)))
	{
	if (GetPodId( node1) == GetPodId( node2))
	{
	cost = 2*costLayer1;
	}
	else
	{
	if (GetCoreGroup(node1) == GetCoreGroup(node2))
	cost = 2*costLayer2;
	else
	cost = 2*costLayer1 + costLayer2;
	}
	}             
	else if ((IsCore( node1) && IsCore( node2)))
	{
	if (GetCoreGroup(node1) == GetCoreGroup(node2))
	cost = 2*costLayer1 + 2*costLayer2;
	else
	cost = 2*costLayer2;
	}

	return cost;

	}
	*/


	std::string DC::GetIpv4RoutingTable (uint32_t i)
	{
		Ptr <Node> n = GetNode(i);
		NS_ASSERT (n);
		Ptr <ns3::Ipv4> ipv4 = n->GetObject <ns3::Ipv4> ();
		if (!ipv4)
		{
			NS_LOG_WARN ("Node " << n->GetId () << " Does not have an Ipv4 object");
			return "";
		}
		std::stringstream stream;
		Ptr<OutputStreamWrapper> routingstream = Create<OutputStreamWrapper> (&stream);
		ipv4->GetRoutingProtocol ()->PrintRoutingTable (routingstream);
		return stream.str();

	}



	void DC::RecursiveIpv4RoutePathSearch (std::string from, std::string to, Ipv4RoutePathElements & rpElements)
	{
		//NS_LOG_INFO ("RecursiveIpv4RoutePathSearch from:" << from.c_str () << " to:" << to.c_str ());
		std::cout << "RecursiveIpv4RoutePathSearch from:" << from.c_str () << " to:" << to.c_str () << endl;

		if ((from == "0.0.0.0") || (from == "127.0.0.1"))
		{
			NS_LOG_INFO ("Got " << from.c_str () << " End recursion");
			return;
		}
		Ptr <Node> fromNode = NodeList::GetNode (m_ipv4ToNodeIdMap[from]);
		Ptr <Node> toNode = NodeList::GetNode (m_ipv4ToNodeIdMap[to]);



		if (fromNode->GetId () == toNode->GetId ())
		{
			Ipv4RoutePathElement elem = { fromNode->GetId (), "L" };
			rpElements.push_back (elem);
			return;
		}
		if (!fromNode)
		{
			NS_FATAL_ERROR ("Node: " << m_ipv4ToNodeIdMap[from] << " Not found");
			return;
		}
		if (!toNode)
		{
			NS_FATAL_ERROR ("Node: " << m_ipv4ToNodeIdMap[to] << " Not found");
			return;
		}
		Ptr <ns3::Ipv4> ipv4 = fromNode->GetObject <ns3::Ipv4> ();
		if (!ipv4)
		{
			NS_LOG_WARN ("ipv4 object not found");
			return;
		}
		Ptr <Ipv4RoutingProtocol> rp = ipv4->GetRoutingProtocol ();
		if (!rp)
		{
			NS_LOG_WARN ("Routing protocol object not found");
			return;
		}
		Ptr<Packet> pkt = Create<Packet> ();
		Ipv4Header header;
		header.SetDestination (Ipv4Address (to.c_str ()));
		Socket::SocketErrno sockerr;
		Ptr <Ipv4Route> rt = rp->RouteOutput (pkt, header, 0, sockerr);
		if (!rt)
		{
			return;
		}
		//NS_LOG_DEBUG ("Node: " << fromNode->GetId () << " G:" << rt->GetGateway ());
		//std::cout << "Node: " << fromNode->GetId () << " G:" << rt->GetGateway ();
		std::ostringstream oss;
		oss << rt->GetGateway ();

		if (oss.str () == "0.0.0.0" && (sockerr != Socket::ERROR_NOROUTETOHOST))
		{
			NS_LOG_INFO ("Null gw");
			Ipv4RoutePathElement elem = { fromNode->GetId (), "C" };
			rpElements.push_back (elem);
			if ( m_ipv4ToNodeIdMap.find (to) != m_ipv4ToNodeIdMap.end ())
			{
				Ipv4RoutePathElement elem2 = { m_ipv4ToNodeIdMap[to], "L" };
				rpElements.push_back (elem2);
			}
			return;
		}
		//NS_LOG_INFO ("Node:" << fromNode->GetId () << "-->" << rt->GetGateway ()); 
		//std::cout << "Node:" << fromNode->GetId () << "-->" << rt->GetGateway ();
		Ipv4RoutePathElement elem = { fromNode->GetId (), oss.str () };
		rpElements.push_back (elem);
		RecursiveIpv4RoutePathSearch (oss.str (), to, rpElements);

	}



	void DC::PrintRoutePath(Ipv4RoutePathElements rpElements)
	{
		//NS_LOG_INFO ("Printing Route Path From :" << nodeId << " To: " << destination.c_str ());
		//std::cout << "Printing Route Path From :" << nodeId << " To: " << destination.c_str ();


		for (Ipv4RoutePathElements::const_iterator i = rpElements.begin ();
			i != rpElements.end ();
			++i)
		{
			Ipv4RoutePathElement rpElement = *i;
			//NS_LOG_INFO ("Node:" << rpElement.nodeId << "-->" << rpElement.nextHop.c_str ());
			std::cout << "Node:" << rpElement.nodeId << "-->" << rpElement.nextHop.c_str () << "\n";
			//WriteN (GetXMLOpenClose_rp (rpElement.node, GetIpv4RoutingTable (n)), m_routingF);

		}
	}


	void DC::Populateipv4ToNodeIdMap (NetDeviceContainer ndc)
	{
		uint32_t id;
		Ptr<Ipv4> ipv4; 
		int32_t ifIndex;
		Ipv4InterfaceAddress addr;

		Ptr <NetDevice> nd;


		ofstream outputFile;
		outputFile.open("ips.csv");


		std::cout << ndc.GetN() << "\n";

		for(uint32_t i = 0; i < ndc.GetN(); i++)
		{
			Ptr <NetDevice> nd = ndc.Get(i);
			id = NodeList::GetNode (nd->GetNode ()->GetId ())->GetId();
			ipv4 = NodeList::GetNode (nd->GetNode ()->GetId ())->GetObject <Ipv4> ();
			if (!ipv4)
			{
				NS_LOG_WARN ("Node: " << nd->GetNode ()->GetId () << " No ipv4 object found");
				//return "0.0.0.0";
			}
			ifIndex = ipv4->GetInterfaceForDevice (nd);
			if (ifIndex == -1)
			{
				NS_LOG_WARN ("Node :" << nd->GetNode ()->GetId () << " Could not find index of NetDevice");
				//return "0.0.0.0";
			}
			addr = ipv4->GetAddress (ifIndex, 0);
			std::ostringstream oss;
			oss << addr.GetLocal ();
			m_ipv4ToNodeIdMap[oss.str()] = id;
			//std::cout << oss.str() << "---->" << m_ipv4ToNodeIdMap[oss.str()] << "\n"; // log
			outputFile << oss.str() << ";" << m_ipv4ToNodeIdMap[oss.str()] << endl;
			//return oss.str ();
		}

		outputFile.close();





	}

} // namespace ns3
