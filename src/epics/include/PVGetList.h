#ifndef epics_PVGETLIST_H
#define epics_PVGETLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "pva/client.h"

namespace imguiDM {

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

    std::vector<PV_value_map> m_pv_buffers;
    int  m_buffer_max  = 2000;

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

    int GetN() const {return m_N_pvs;}

    std::vector<float>& GetBuffer(int n) ;

    void PrintAll() const;

    /** This actually goes to CA and gets new values for all variables
     */
    void Poll();

  private:
    class Impl;
    std::shared_ptr<Impl> m_impl;
  };

}
#endif
