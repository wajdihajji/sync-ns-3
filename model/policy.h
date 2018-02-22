

#ifndef POLICY_H
#define POLICY_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/mb.h"
#include "ns3/vm.h"

namespace ns3 {

	class Policy : public Object 
	{
	public:

		static TypeId GetTypeId (void);

		Policy ();          

		virtual ~Policy();

		void SetPolicy(void); // generate m_mbRequiredList, set src, dst and update m_mbList

		void AllocateMB(Ptr<Middlebox> mb);

		void RemoveMB(Ptr<Middlebox> mb);
		
		bool IsSatisfied(void);

		uint32_t GetPolicyLen() {return m_mbList.size();}

		Ptr <VM> flow; // = src - policy flow - policy 1:1 flow 

		Ptr <VM> src; // vm source

		Ptr <VM> dst; // vm destination

		std::vector<Ptr <Middlebox>> m_mbList; // attribute 


	private:

		std::vector<Ptr <Middlebox>> m_mbRequiredList;

	};

} // namespace ns3

#endif /* POLICY_H */
