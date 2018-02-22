/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation;
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/dc.h"
#include "ns3/hypervisor.h"
#include "ns3/vm.h"
#include "ns3/vm-helper.h"
#include "ns3/ipv4-sync-routing.h"
#include "ns3/flow-monitor-module.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>


// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");



int 
	main (int argc, char *argv[])
{

	char filename [] = "fattree.xml";

	Ptr <DC> fattree = CreateObject<DC>();

	fattree->CreateTopo();

	/*
	for (NodeContainer::Iterator it =fattree->m_allnodes.Begin (); it != fattree->m_allnodes.End (); ++it) {		

		Ptr<Node> node = *it;
		Ptr<GlobalRouter> gr  = node->GetObject<GlobalRouter>();
		Ptr<Ipv4GlobalRouting> grp = gr->GetRoutingProtocol();
		Ptr<SyncRouting> pr = grp->GetObject<SyncRouting>(); 
		pr->fattree = fattree;
	}
	*/
	
	fattree->CreateRandVms(1);



	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	//std::cout << fattree->CostBetween2Nodes(1,16) << "\n";
	//std::cout << fattree->CostBetween2Nodes(29,42) << "\n";
	//std::cout << fattree->CostBetween2Nodes(13,19) << "\n";

	/*std::ofstream outputFile;
	outputFile.open("routing-table.csv");

	std::string s = fattree->GetIpv4RoutingTable(15);

	outputFile << s << "\n";




	s = fattree->GetIpv4RoutingTable(15);

	outputFile << s << "\n";
	*/







	// 10.1.1.4 --> 10.0.0.4 : 35 --> 29 : 
	/*
	NetDeviceContainer ndc = fattree->GetNDC(); 

	fattree->Populateipv4ToNodeIdMap(ndc);

	fattree->RecursiveIpv4RoutePathSearch("10.1.1.4","10.0.0.4",fattree->rpElets);

	fattree->PrintRoutePath(fattree->rpElets);
	*/



	/*

	s = fattree->GetIpv4RoutingTable(9);

	std::cout << s << "\n";



	s = fattree->GetIpv4RoutingTable(12);

	std::cout << s << "\n";

	s = fattree->GetIpv4RoutingTable(43);

	std::cout << s << "\n";
	*/









	/*
	Ptr <Node> node1 = fattree->GetHost().Get(2);
	Ptr <Node> node11 = fattree->GetHost().Get(6);
	Ptr <Node> node12 = fattree->GetHost().Get(10);
	Ptr <Node> node2 = fattree->GetEdge().Get(2);
	Ptr <Node> node3 = fattree->GetAgg().Get(2);
	Ptr <Node> node4 = fattree->GetCore().Get(2);
	*/
	//NodeContainer cont = fattree->m_allnodes;
	//for (int i=0; i < cont.GetN(); i++)
	//	std::cout << cont.Get(i)->GetId() << "\n";

	//fattree->CreateRandVms(2); 

	//fattree->PrintVMCommunication();

	//Ptr <VM> vm = fattree->allVms.at(0);

	//std::cout << "\n" << vm->m_vmid << "\n";

	//std::vector<Ptr <VM>> flows = fattree->GetFlows(vm);

	//std::vector<Ptr <VM>> group = fattree->GetNextComVMGroup();

	//fattree->Print(flows);

	//fattree->Print(group);



	/*
	std::vector<int> v;
	v.push_back(0);
	v.push_back(1); 
	v.push_back(2);

	std::cout << "\n" << v.at(0) << "\n";
	v.erase(std::remove(v.begin(), v.end(), 0), v.end());
	std::cout << "\n" << v.at(0) << "\n";
	if (!(std::find(v.begin(), v.end(), 2) != v.end()))
	std::cout << "\n 6 does not exist!\n";
	*/




	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();


	//_sleep(10000);

	Simulator::Stop (Seconds(10));
	Simulator::Run ();
	monitor->SerializeToXmlFile(filename, true, true);
	Simulator::Destroy ();
	NS_LOG_UNCOND("Done");
	_sleep(3000);
	return 0;
}
