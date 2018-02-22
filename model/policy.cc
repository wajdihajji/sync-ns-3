

#include "ns3/log.h"
#include "policy.h"
#include "ns3/mb.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE ("Policy");

namespace ns3 {


	TypeId
		Policy::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::Policy")
			.SetParent<Object> ()
			.AddConstructor<Policy> ()
			;
		return tid;
	}


	Policy::Policy ()
	{
		NS_LOG_FUNCTION (this);
	}

	Policy::~Policy()
	{
		NS_LOG_FUNCTION (this);
	}

	void Policy::SetPolicy(void)
	{
		// to be implemented
	}

	void Policy::AllocateMB(Ptr<Middlebox> mb)
	{
		m_mbList.push_back(mb);
	}


	void Policy::RemoveMB(Ptr<Middlebox> mb)
	{
		m_mbList.erase(std::remove(m_mbList.begin(), m_mbList.end(), mb), m_mbList.end());
	}
	
	bool Policy::IsSatisfied(void)
	{
		if (m_mbList == m_mbRequiredList)
			return true;
		else 
			return false;
	}

} // Namespace ns3
