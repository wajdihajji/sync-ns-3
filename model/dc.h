
#ifndef DC_H
#define DC_H

#include <stdint.h>
#include <string>
#include "ns3/core-module.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/string.h"
#include "ns3/data-rate.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/random-variable-stream.h"
#include "ns3/net-device-container.h"

#include "ns3/node-container.h"
#include "ns3/hypervisor.h"



namespace ns3 {


	class DC : public Object
	{
	public:

		DC ();

		~DC ();

		void CreateTopo(void);

		char * toString(int a,int b, int c, int d);

		void GetIps(NodeContainer container);

		NodeContainer GetCore(void) {return m_core;};

		NodeContainer GetAgg(void) {return m_agg;};

		NodeContainer GetEdge(void) {return m_edge;};

		NodeContainer GetBridge(void) {return m_bridge;};

		NodeContainer GetHost(void) {return m_host;};

		//NetDeviceContainer GetNDC(void) {return m_alldevices;};

		Ipv4Address GetRandAddr(NodeContainer container, Ipv4Address notthisip); // get random address from container different from nothisip

		void CreateRandVms(uint32_t n); // n number of vm to create

		void CreateHyp(Ptr <Node>);

		std::vector<Ptr <Hypervisor>> m_hypList; // list of hypervisors

		std::vector<Ptr <VM>> DC::GetFlows(Ptr <VM> vm);

		void PrintHypList(void);

		void PrintVMCommunication(void);

		std::vector<Ptr <VM>> allVms;

		std::vector<Ptr <VM>> GetNextComVMGroup();

		void Print(std::vector<Ptr <VM>>);

		uint32_t CostBetween2Nodes(uint32_t i, uint32_t j);

		int GetPodId(Ptr <Node>);

		int GetCoreGroup(Ptr <Node>);

		bool IsHost(Ptr <Node>);

		bool IsEdge(Ptr <Node>);

		bool IsAgg(Ptr <Node>);

		bool IsCore(Ptr <Node>);

		typedef struct {
			uint32_t nodeId;
			std::string nextHop;

		} Ipv4RoutePathElement;


		std::string GetIpv4RoutingTable (uint32_t i); // i is the node id

		typedef std::vector <Ipv4RoutePathElement> Ipv4RoutePathElements;

		std::map <std::string, uint32_t> m_ipv4ToNodeIdMap;

		void DC::Populateipv4ToNodeIdMap (NetDeviceContainer ndc);

		void PrintRoutePath(Ipv4RoutePathElements rpElements);

		Ipv4RoutePathElements rpElets;

		void RecursiveIpv4RoutePathSearch (std::string fromIpv4, std::string toIpv4, Ipv4RoutePathElements &);


		uint32_t costLayer0;
		uint32_t costLayer1;
		uint32_t costLayer2;


		//std::vector<Ptr <Hypervisor>> GetHypList() {return m_hypList;}

		NodeContainer m_allnodes;

		NetDeviceContainer m_alldevices;

		Ptr <Node> GetNode(uint32_t);

	private:
		NodeContainer m_core; // NodeContainer for core switches

		NodeContainer m_agg; // NodeContainer for aggregation switches

		NodeContainer m_edge; // NodeContainer for edge switches

		NodeContainer m_bridge; // NodeContainer for edge bridges

		NodeContainer m_host; // NodeContainer for hosts

	};



} // namespace ns3

#endif /* DC_H */

