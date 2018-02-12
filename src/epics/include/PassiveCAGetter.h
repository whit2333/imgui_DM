#ifndef epics_PassiveCAGetter_HH
#define epics_PassiveCAGetter_HH 1

#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <exception>

#if !defined(_WIN32)
#include <signal.h>
#define USE_SIGNAL
#endif

#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsGuard.h>
#include <epicsGetopt.h>

#include <pv/configuration.h>
#include <pv/caProvider.h>
#include <pv/reftrack.h>
#include <pva/client.h>

namespace pvd = epics::pvData;
namespace pva = epics::pvAccess;


namespace {

epicsEvent done;

//#ifdef USE_SIGNAL
//void alldone(int num)
//{
//    (void)num;
//    done.signal();
//}
//#endif

struct PassiveCAGetter : public pvac::ClientChannel::GetCallback,
                         public pvac::ClientChannel::ConnectCallback
{
    POINTER_DEFINITIONS(PassiveCAGetter);

    const std::string   name;
    pvac::ClientChannel channel;
    pvac::Operation     op;

    PassiveCAGetter(pvac::ClientProvider& provider, const std::string& name) :
      name(name), channel(provider.connect(name))
    {
        channel.addConnectListener(this);
    }

    virtual ~PassiveCAGetter()
    {
        channel.removeConnectListener(this);
        op.cancel();
    }

    virtual void getDone(const pvac::GetEvent& event)
    {
      switch(event.event) {
        case pvac::GetEvent::Fail:
          std::cout<<"Error "<<name<<" : "<<event.message<<"\n";
          break;
        case pvac::GetEvent::Cancel:
          std::cout<<"Cancel "<<name<<"\n";
          break;
        case pvac::GetEvent::Success:
          pvd::PVField::const_shared_pointer valfld(event.value->getSubField("value"));
          if(!valfld)
            valfld = event.value;
          std::cout<<name<<" : "<<*valfld<<"\n";
          break;
      }
    }

    virtual void connectEvent(const pvac::ConnectEvent& evt)
    {
      if(evt.connected) {
        op = channel.get(this);
      } else {
        std::cout<<"Disconnect "<<name<<"\n";
      }
    }
};

} // namespace

#endif
