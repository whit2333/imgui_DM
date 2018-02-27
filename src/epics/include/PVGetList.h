#ifndef epics_PVGETLIST_H
#define epics_PVGETLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "pva/client.h"

namespace ImGuiDM {

  /** PV Buffer.
   *  Actuall the buffer is twice the size of fBufferMaxSize so that when 
   *  it gets full the buffer is rotated about the middle. This avoids 
   *  constantly moving memory around. Only half the buffer (index to fBufferMaxSize)
   *  should be considered valid at any moment.
   *
   */
  class PVBuffer {
  public:
    int                fBufferMaxSize = 1000;
    int                fArrayIndex    = 0;
    std::vector<float> fBuffer;
    std::vector<float> fBufferCopy;

    PVBuffer(float init_val = 0.0);

    int                 GetIndex() const { return fArrayIndex; }
    int                 GetMaxSize() const { return fBufferMaxSize; }
    void                Add(float val);
    int                 GetOffset() const;
    std::vector<float>& GetBufferCopy();

  private:
    class Impl;
    std::shared_ptr<Impl> m_impl;
  };


  /** PV Get-List. Continuously updated list of variables.
   *
   */
  class PVGetList {
  public:
    using PV_index_name_map  = std::map<std::string,int>;
    using PV_name_map        = std::map<int,std::string>;
    using PV_value_map       = std::vector<float>;
    using PV_channel_map     = std::map<int,pvac::ClientChannel>;
    using PV_provider        = std::shared_ptr<pvac::ClientProvider>;

  public:
    int                   m_N_pvs = 0;
    PV_provider           m_provider;
    PV_index_name_map     m_pv_index;
    PV_name_map           m_pv_names;
    PV_value_map          m_pv_values;
    PV_channel_map        m_pv_channels;
    std::vector<int>      m_array_index;
    std::vector<PVBuffer> m_pv_buffers;

  public:
    PVGetList();
    PVGetList(const PVGetList&) = default ;
    PVGetList(const std::vector<std::string>& names);
    ~PVGetList();

    /** Add a new process variable to list. 
     * Returns the list index for a new or existing variable.
     */
    int AddPV(const std::string&);

    float GetValue(const std::string& n) const; 
    float GetValue(int n) const;

    std::string GetName(int n) const { return m_pv_names.at(n);}

    // This would load history at startup
    void Init() { }

    int          GetN() const { return m_N_pvs; }
    unsigned int GetNBuffers() const { m_pv_buffers.size(); }
    int          GetBufferSize(int n) const ;
    int          GetBufferOffset(int n) const ;

    PVBuffer&            GetBuffer(int n) ;
    std::vector<float>&  GetBufferCopy(int n) ;

    void PrintAll() const;

    /** This actually goes to CA and gets new values for all variables.
     */
    void Poll();

  private:
    class Impl;
    std::shared_ptr<Impl> m_impl;
  };

}
#endif
