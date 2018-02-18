#include "PVGetList.h"

#include <algorithm>
#include <sstream>
#include "pva/client.h"
#include "pv/valueBuilder.h"
#include "pv/convert.h"

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

        //channel.put()
        //        .set("value", argv[2])
        //        .exec();

namespace ImGuiDM {

  class PVGetList::Impl {
  public:
    std::mutex m;
  };
  //______________________________________________________________________________

  PVGetList::PVGetList() : 
    m_N_pvs(0), m_provider(std::make_shared<pvac::ClientProvider>("pva")), m_impl(std::make_shared<Impl>())
  { }
  //______________________________________________________________________________

  PVGetList::PVGetList(const std::vector<std::string>& names) : 
    m_N_pvs(0), m_provider(std::make_shared<pvac::ClientProvider>("pva")), m_impl(std::make_shared<Impl>())
  {
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
        auto val = ret->getSubField<epics::pvData::PVDouble>("value");
        m_pv_values.push_back( val->getAs<float>());
        std::cout << m_pv_values[index] << std::endl;
        m_pv_index[name]     = index;
        m_array_index.push_back(m_buffer_max);
        // array index starting from the front
        int current_array_index =  m_array_index.at(index);
        // create buffer of zeros
        std::vector<float> buffer(2*m_buffer_max,0);
        buffer.reserve(2*m_buffer_max);
        buffer[current_array_index] = m_pv_values.at(index);
        m_pv_buffers.push_back(buffer);
        std::vector<float> buffer2;
        m_buffer_copy.push_back(buffer2);
        m_N_pvs++;
      }
    }
  }
  //______________________________________________________________________________

  PVGetList::~PVGetList()
  { }
  //______________________________________________________________________________

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
      m_pv_values.push_back( val->getAs<float>());
      m_pv_index[name]    = index;
      std::cout << m_pv_values[index] << std::endl;
      std::vector<float> buffer;
      buffer.push_back(m_pv_values[index]);
      m_pv_buffers.push_back(buffer);
      m_N_pvs++;
      return index;
    }
    return m_pv_index[name];
  }
  //______________________________________________________________________________

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
  //______________________________________________________________________________

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
  //______________________________________________________________________________

  void PVGetList::PrintAll() const
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    for(const auto& index_pair : m_pv_index) {
      int index = index_pair.second;
      const auto& name   = index_pair.first;
      std::cout << name << " \t " <<  m_pv_values.at(index) << "\n";
    }
  }
  //______________________________________________________________________________

  void PVGetList::Poll()
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    for(auto& channel_pair : m_pv_channels) {
      int index = channel_pair.first;
      auto ret = channel_pair.second.get();
      auto val = ret->getSubField<epics::pvData::PVDouble>("value");
      m_pv_values[index] = val->getAs<float>();
      // Now we buffer the variable (to view on a strip chart)
      // Check that we are not at the end of the array
      if (m_array_index[index]+1 == m_buffer_max*2) {
        // Rotate about the middle 
        std::rotate(m_pv_buffers[index].begin(),
                    m_pv_buffers[index].begin()+m_buffer_max,
                    m_pv_buffers[index].end() );
        m_array_index[index] = m_buffer_max-1;
      }
      m_array_index[index]++;
      m_pv_buffers[index][m_array_index.at(index)] = m_pv_values[index];
      //std::cout << m_pv_buffers[index].size() << std::endl;
      //std::cout << " index  " << m_array_index.at(index) << "\n";
      //std::cout << "Rotating : " << m_pv_buffers[index][m_array_index.at(index)] <<"\n";
      //std::cout << "Rotating : " << m_pv_buffers[index][m_array_index.at(index)-1] <<"\n";

      //std::rotate(m_pv_buffers[index].begin(),
      //            m_pv_buffers[index].begin()+m_buffer_max,
      //            m_pv_buffers[index].end() );
      //m_array_index[index] = m_buffer_max-1;
      //std::cout << " rotated : " << m_pv_buffers[index][m_array_index.at(index)] <<"\n";
      //std::cout << " rotated : " << m_pv_buffers[index][m_array_index.at(index)-1] <<"\n";
      //if(m_pv_buffers[index].size() > (m_buffer_max + m_buffer_extra)) {
      //  m_pv_buffers[index].erase(m_pv_buffers[index].begin(),m_pv_buffers[index].begin()+m_buffer_extra);
      //}
    }
  }
  //______________________________________________________________________________

  std::vector<float>& PVGetList::GetBuffer(int n)
  {
    if ( (n >= 0) && (n < GetNBuffers()) ) {
      return m_pv_buffers.at(n);
    } else if (n==0) {
      std::vector<float> buffer;
      m_pv_buffers.push_back(buffer);
    }
  }
  //______________________________________________________________________________

  std::vector<float>& PVGetList::GetBufferCopy(int n)
  {
    std::lock_guard<std::mutex> lockGuard(m_impl->m);
    if ( (n >= 0) && (n < m_buffer_copy.size()) ) {
      m_buffer_copy[n].clear();
      std::copy(m_pv_buffers[n].begin()+GetBufferOffset(n),
                m_pv_buffers[n].begin()+GetBufferOffset(n)+m_buffer_max,
                std::back_inserter(m_buffer_copy[n]));
      return m_buffer_copy[n];
    }
    std::cout << "error\n";
    std::vector<float> buffer;
    m_buffer_copy.push_back(buffer);
    return buffer;
  }
  //______________________________________________________________________________

  int PVGetList::GetBufferSize(int n) const
  {
    int sz = m_array_index.at(n);
    if(sz < m_buffer_max) {
      return sz;
    } 
    return m_buffer_max;
    //auto buf_size = m_pv_buffers.at(n).size();
    //if( buf_size > m_buffer_max  ) {
    //  return m_buffer_max;
    //}
    //return buf_size;
  }
  //______________________________________________________________________________
    
  int PVGetList::GetBufferOffset(int n) const
  {
    //int off = (int)m_pv_buffers.at(n).size() - m_buffer_max;
    int current_index = m_array_index.at(n);
    if( current_index > m_buffer_max  ) {
      return current_index - m_buffer_max;
    }
    return 0;
    //if (off > 0) {
    //  return off;
    //}
    //return 0;
  }

}

