#include "PVGetList.h"

#include <sstream>
#include "pva/client.h"
#include "pv/valueBuilder.h"
#include "pv/convert.h"

namespace imguiDM {

  class PVGetList::Impl {
  public:
    std::mutex m;
  };

  PVGetList::PVGetList() : 
    m_N_pvs(0), m_provider(std::make_shared<pvac::ClientProvider>("pva")), m_impl(std::make_shared<Impl>())
  {
  }

  PVGetList::PVGetList(const std::vector<std::string>& names) : 
    m_N_pvs(0), m_provider(std::make_shared<pvac::ClientProvider>("pva")), m_impl(std::make_shared<Impl>())
  {
    std::cout << " DERP\n";
    for(const auto& name : names ){
      if (m_pv_index.count(name) == 0 ) {
        int index = m_N_pvs;
        m_pv_names[index]    = name;
        m_pv_channels[index] =  m_provider->connect(name);
        auto ret = m_pv_channels[index].get(1.0);//5.0,epics::pvData::ValueBuilder()
                                            //.addNested("whit")
                                            //.addNested("circle")
                                            //.add<epics::pvData::pvDouble>("angle", 42.0)
                                            //.endNested()
                                            //.endNested()
                                            //.buildPVStructure());
        auto nfs = ret->getNumberFields(); 
        //std::cout <<  " nfs " << nfs << std::endl;
        //auto pv_fields = ret->getPVFields(); 
        //for(const auto& p : pv_fields) {
        //  std:: cout << " p " << p << "\n";
        //}
        //for (int ii=0; ii< nfs; ii++) {
        //  auto field_ii = ret->getSubField<epics::pvData::PVDouble,size_t>(ii);
        //  std::cout << field_ii <<std::endl;
        //  std::cout << "type " << field_ii << std::endl;
        //  //std::cout << " type " << ret->getSubFieldType<size_t>(ii) << std::endl;
        //   //for(int ii = 0; ii< p->getNumberFields() ; ii++) {
        //   //  auto cvert = epics::pvData::Convert::getConvert();
        //   //  std::cout <<   << "\n";
        //   //}
        //  //for(const auto& p2 : pv_fields2) {
        //  //  std:: cout << " p2 " << p2 << "\n";
        //  //}
        //}
        auto val = ret->getSubField<epics::pvData::PVDouble>("value");
        m_pv_values.push_back( val->getAs<double>());
        std::cout << m_pv_values[index] << std::endl;
        m_pv_index[name]     = index;
        std::vector<float> buffer;
        m_pv_buffers.push_back(buffer);
        m_N_pvs++;
      }
    }
  }

  PVGetList::~PVGetList()
  { }


  /** Add a new process variable to list. 
   * Returns the list index for 
   * new or already existing variable.
   */
  int PVGetList::AddPV(const std::string& name)
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    if (m_pv_index.count(name) == 0 ) {
      int index =  m_N_pvs;
      m_pv_names[index]    = name;
      m_pv_channels[index] = m_provider->connect(name);
      auto ret = m_pv_channels[index].get();
      auto val = ret->getSubField<epics::pvData::PVDouble>("value");
      m_pv_values.push_back( val->getAs<double>());
      m_pv_index[name]    = index;
      std::cout << m_pv_values[index] << std::endl;
      std::vector<float> buffer;
      m_pv_buffers.push_back(buffer);
      m_N_pvs++;
      return index;
    }
    return m_pv_index[name];
  }

  /** Thread safe get latest cached value.
   */
  float PVGetList::GetValue(int n) const
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    if (n >= m_N_pvs ) {
      //error
      return 0.0;
    }
    return m_pv_values.at(n);
  }

  /** Thread safe get latest cached value.
   */
  float PVGetList::GetValue(const std::string& n) const
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    if (m_pv_index.count(n) == 0 ) {
      //error
      return 0;
    }
    return m_pv_values.at(m_pv_index.at(n));
  }

  void PVGetList::PrintAll() const
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    for(const auto& index_pair : m_pv_index) {
      int index = index_pair.second;
      const auto& name   = index_pair.first;
      std::cout << name << " \t " <<  m_pv_values.at(index) << "\n";
    }
  }

  void PVGetList::Poll()
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    for(auto& channel_pair : m_pv_channels) {
      int index = channel_pair.first;
      auto ret = channel_pair.second.get();
      auto val = ret->getSubField<epics::pvData::PVDouble>("value");
      m_pv_values[index] = val->getAs<float>();
      m_pv_buffers[index].push_back(m_pv_values[index]);
      if(m_pv_buffers[index].size() > m_buffer_max + 100) {
        m_pv_buffers[index].erase(m_pv_buffers[index].begin(),m_pv_buffers[index].begin()+100);
      }
    }
  }

  std::vector<float>& PVGetList::GetBuffer(int n) 
  {
    return m_pv_buffers.at(n);
  }


}
