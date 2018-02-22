

#include "ns3/log.h"
#include "mb.h"
#include "ns3/vm.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE ("Middlebox");

namespace ns3 {


	TypeId
		Middlebox::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::Middlebox")
			.SetParent<Application> ()
			.AddConstructor<Middlebox> ()
			;
		return tid;
	}


	Middlebox::Middlebox ()
	{
		NS_LOG_FUNCTION (this);
	}

	Middlebox::~Middlebox()
	{
		NS_LOG_FUNCTION (this);
	}

	void Middlebox::InstallMB(Ptr<Node> node)
	{
		Ptr <Middlebox> mb = CreateObject<Middlebox>();
		m_mbHost->AddApplication(mb);
	}

	void Middlebox::DeleteMB(void)
	{
		// to be implemented
	}

	void Middlebox::Addflow(Ptr <VM> vm) // vm 1:1 flow
	{
		if (m_mbCapacity-(vm->GetDataRate().GetBitRate()/1000)>0)
		{
		m_flows.push_back(vm);
		m_mbActualCapacity -= vm->GetDataRate().GetBitRate()/1000; 
		}
		else
		NS_LOG_WARN ("This flow cannot be added.");
	}


	void Middlebox::Removeflow(Ptr <VM> vm) // vm 1:1 flow
	{
		// check if this flow is already added.
		m_flows.erase(std::remove(m_flows.begin(), m_flows.end(), vm), m_flows.end());
		m_mbActualCapacity += vm->GetDataRate().GetBitRate()/1000;
	}

		// Application Methods
	void Middlebox::StartApplication () // Called at time specified by Start
	{
		NS_LOG_FUNCTION (this);
	}

	void Middlebox::StopApplication () // Called at time specified by Stop
	{
		NS_LOG_FUNCTION (this);
	}

	void Middlebox::DoDispose (void)
	{
		NS_LOG_FUNCTION (this);

		//m_socket = 0;
		// chain up
		Application::DoDispose ();
	}


} // Namespace ns3
