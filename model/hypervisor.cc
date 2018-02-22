/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: George F. Riley<riley@ece.gatech.edu>
//

// ns3 - On/Off Data Source Application class
// George F. Riley, Georgia Tech, Spring 2007
// Adapted from ApplicationOnOff in GTNetS.


#include "ns3/log.h"
#include "ns3/vm-helper.h"
#include "hypervisor.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/core-module.h"



NS_LOG_COMPONENT_DEFINE ("Hypervisor");

namespace ns3 {


	TypeId
		Hypervisor::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::Hypervisor")
			.SetParent<Application> ()
			.AddConstructor<Hypervisor> ()
			;
		return tid;
	}


	Hypervisor::Hypervisor ()
	{
		NS_LOG_FUNCTION (this);
		//std::cout << "hypervisor is created! \n";
	}

	Hypervisor::~Hypervisor()
	{
		NS_LOG_FUNCTION (this);
		//std::cout << "hypervisor is destroyed! \n";
	}


	void
		Hypervisor::InstallHyp (Ptr<Node> node)
	{
		m_HypServer = node;

		Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
		Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1,0);
		Ipv4Address addri = iaddr.GetLocal (); 
		m_HypAddress = addri;

		node->AddApplication(this);

		//m_hypList.push_back(this);

		this->m_cap_cpu = 4;
		this->m_cap_mem = 8192;

		//std::cout << "Hypervisor is installed on the IP: " << addri << "\n";

		this->StartApplication();
	}


	void
		Hypervisor::DoDispose (void)
	{
		NS_LOG_FUNCTION (this);

		//m_socket = 0;
		// chain up
		Application::DoDispose ();
	}


	// Application Methods
	void Hypervisor::StartApplication () // Called at time specified by Start
	{
		NS_LOG_FUNCTION (this);
		is_started=true;

	}

	void Hypervisor::StopApplication () // Called at time specified by Stop
	{
		NS_LOG_FUNCTION (this);
		is_started=false;
	}


	Ptr <VM> Hypervisor::CreateVM(uint32_t vmid, uint32_t cpu, uint32_t mem, Ptr <VM> vmRemote, Ipv4Address remote) //, Ptr <Node>  host
	{
		uint32_t packetSize = 1024;
		char dataRate_OnOff [] = "1Mbps";
		char maxBytes [] = "0";	
		int port = 9;

		Ptr <VM> vm;

		if (m_cap_cpu > cpu && m_cap_mem > mem)
		{
			VmHelper vh = VmHelper("ns3::UdpSocketFactory",Address(InetSocketAddress(Ipv4Address(remote), port))); // ip address of

			vh.SetAttribute("OnTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));  
			vh.SetAttribute("OffTime",StringValue ("ns3::ConstantRandomVariable[Constant=1.0]")); 
			vh.SetAttribute("PacketSize",UintegerValue (packetSize));
			vh.SetAttribute("DataRate",StringValue (dataRate_OnOff));      
			vh.SetAttribute("MaxBytes",StringValue (maxBytes));

			vm = vh.InstallOnServer(m_HypServer);


			vm->m_vmid = vmid;
			vm->m_cpu = cpu;
			vm->m_mem = mem;
			vm->m_server = m_HypServer;
			if (vmRemote != NULL) 
				vm->m_vmRemote = vmRemote;

			m_vmList.push_back(vm);
		}
		else
		{
			NS_LOG_WARN ("The vm cannot be created on this server.");
		}
		return vm;
	}



	void Hypervisor::DeleteVM(Ptr <VM> vm)
	{
		this->m_cap_cpu+=vm->m_cpu;
		this->m_cap_mem+=vm->m_mem;
		this->m_vmList.erase(std::remove(this->m_vmList.begin(), this->m_vmList.end(), vm), this->m_vmList.end());
		//vm->DoDelete();
		//vm->~VM();
		//vm->DeleteVM();
		//std::cout << "VM deleted!\n";
	}

	void Hypervisor::StartVM(Ptr <VM> vm)
	{
		vm->StartApplication();
	}

	void Hypervisor::StopVM(Ptr <VM> vm)
	{
		vm->StopApplication();
	}

	void Hypervisor::MigrateVM(Ptr <VM> vm, Ptr <Hypervisor> hypDestination)
	{

		Ptr <Node> server = hypDestination->GetNode();
		Ptr <Node> host = this->GetNode();

		std::cout <<"Migrate from " << host->GetId() << " to " << server->GetId() << "\n";

		if (hypDestination->m_cap_cpu > vm->m_cpu && hypDestination->m_cap_mem > vm->m_mem)
		{

			m_vmList.erase(std::remove(m_vmList.begin(), m_vmList.end(), vm), m_vmList.end());

			//std::cout << vm->GetNode()->GetId() << "\n";
			vm->StopApplication();
			vm->SetNode(server);
			vm->StartApplication();

			//std::cout << vm->GetNode()->GetId() << "\n";

			vm->m_server = server;

			this->m_cap_cpu -= vm->m_cpu;
			this->m_cap_mem -= vm->m_mem;

			hypDestination->m_cap_cpu += vm->m_cpu;
			hypDestination->m_cap_mem += vm->m_mem;
			hypDestination->m_vmList.push_back(vm);
		}
		else
		{
			NS_LOG_WARN ("The vm cannot be migrated to this server.");
		}
	}



	void Hypervisor::GetVMs(void)
	{
		std::vector<Ptr <VM>>::const_iterator itr;
		for (itr = this->m_vmList.begin(); itr != this->m_vmList.end(); itr++) {
			std::cout << "VM ID: " << (*itr)->m_vmid <<  ", VM cpu: " << (*itr)->m_cpu << ", VM memory: " << (*itr)->m_mem << ", VM server: " << (*itr)->GetNode()->GetId() << "\n";

		}
	}

	void Hypervisor::StartAllVMs(void)
	{
		std::vector<Ptr <VM>>::const_iterator itr;
		for (itr = this->m_vmList.begin(); itr != this->m_vmList.end(); itr++) {
			this->StartVM((*itr));
			//std::cout << "VM ID: " << (*itr)->m_vmid <<  ", VM cpu: " << (*itr)->m_cpu << ", VM memory: " << (*itr)->m_mem << ", VM server: " << (*itr)->m_server << "\n";

		}
	}


} // Namespace ns3
