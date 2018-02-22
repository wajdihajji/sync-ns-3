

#ifndef MB_H
#define MB_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/vm.h"


namespace ns3 {

	enum MBType {m_IPS, m_FW, m_RE};

	class Middlebox : public Application 
	{
	public:

		static TypeId GetTypeId (void);

		Middlebox ();          

		virtual ~Middlebox();

		void InstallMB(Ptr<Node> node);

		void DeleteMB(void);

		Ptr <Node> m_mbHost; // node on which the middlebox is Installed

		uint32_t m_mbID; // middlebox id

		MBType m_mbType; // middlebox type

		uint16_t m_mbState; // middlebox state

		uint64_t m_mbCapacity; // capacity of middlebox

		uint64_t m_mbActualCapacity; 

		std::vector<Ptr <VM>>  m_flows;

		void Addflow(Ptr <VM>); // vm 1:1 flow

		void Removeflow(Ptr <VM>); // vm 1:1 flow

	protected:
		virtual void DoDispose (void);
	private:
		// inherited from Application base class.
		virtual void StartApplication (void);    // Called at time specified by Start
		virtual void StopApplication (void);     // Called at time specified by Stop

	};

} // namespace ns3

#endif /* MB_H */
