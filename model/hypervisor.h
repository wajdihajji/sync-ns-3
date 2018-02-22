

#ifndef HYPERVISOR_H
#define HYPERVISOR_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/vm.h"


namespace ns3 {

	class Hypervisor : public Application 
	{
	public:

		static TypeId GetTypeId (void);

		Hypervisor ();          

		void InstallHyp(Ptr<Node> node);

		virtual ~Hypervisor();

		std::vector<Ptr <VM>> m_vmList; 

		// std::vector<Ptr <VM>> m_flowList; can be inferred from m_vmList

		Ptr <VM> CreateVM(uint32_t vmid, uint32_t cpu, uint32_t mem, Ptr <VM> vm, Ipv4Address remote);

		void DeleteVM(Ptr <VM> vm);

		void StartVM(Ptr <VM> vm);

		void StopVM(Ptr <VM> vm);

		void MigrateVM(Ptr<VM> vm, Ptr <Hypervisor> hypDestination);

		void StartAllVMs(void);

		void GetVMs(void);

		Ipv4Address GetHypAdd(void) {return m_HypAddress;};

		uint32_t m_cap_cpu;

		uint32_t m_cap_mem;

	protected:
		virtual void DoDispose (void);
	private:
		// inherited from Application base class.
		virtual void StartApplication (void);    // Called at time specified by Start
		virtual void StopApplication (void);     // Called at time specified by Stop

		bool is_started;

		Ptr <Node> m_HypServer; //server on which the hypervisor is installed

		Ipv4Address m_HypAddress; // address of HypServer
	};

} // namespace ns3

#endif /* HYPERVISOR_H */
