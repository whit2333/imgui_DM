#ifndef epics_PVMonitor_HH
#define epics_PVMonitor_HH

#include <set>
#include <queue>
#include <vector>
#include <string>
#include <memory>
#include <exception>

#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsGuard.h>
#include <epicsGetopt.h>

#include <pv/configuration.h>
#include <pv/caProvider.h>
#include <pv/reftrack.h>
#include <pv/thread.h>
#include <pva/client.h>

namespace pvd = epics::pvData;
namespace pva = epics::pvAccess;

namespace imguiDM {


  using Guard   = epicsGuard<epicsMutex>;
  using UnGuard = epicsGuardRelease<epicsMutex>;

  /** Simple work queue with thread.
   *  moves monitor queue handling off of PVA thread(s).
   */
  template<typename T>
  struct WorkQueue : public epicsThreadRunable {

    using type       = T;
    using value_type = std::shared_ptr<T> ;
    using weak_type  = std::weak_ptr<T>;
    // work queue holds only weak_ptr, so jobs must be kept alive seperately
    using queue_t = std::deque<std::pair<weak_type, pvac::MonitorEvent> >;

    epicsMutex  mutex;
    queue_t     queue;
    epicsEvent  event;
    bool        running;
    pvd::Thread worker;

    WorkQueue() : running(true), worker(pvd::Thread::Config()
                                        .name("Monitor handler")
                                        .autostart(true)
                                        .run(this)) { }
    ~WorkQueue(){ close(); }

    void close()
    {
      {
        Guard G(mutex);
        running = false;
      }
      event.signal();
      worker.exitWait();
    }

    void push(const weak_type& cb, const pvac::MonitorEvent& evt)
    {
      bool wake;
      {
        Guard G(mutex);
        if(!running) return; // silently refuse to queue during/after close()
        wake = queue.empty();
        queue.push_back(std::make_pair(cb, evt));
      }
      if(wake)
        event.signal();
    }

    virtual void run() OVERRIDE FINAL
    {
      Guard G(mutex);

      while(running) {
        if(queue.empty()) {
          UnGuard U(G);
          event.wait();
        } else {
          typename queue_t::value_type ent(queue.front());
          value_type cb(ent.first.lock());
          queue.pop_front();
          if(!cb) continue;

          try {
            UnGuard U(G);
            cb->process(ent.second);
          }catch(std::exception& e){
            std::cout<<"Error in monitor handler : "<<e.what()<<"\n";
          }
        }
      }
    }
  };

  template<typename T>
  struct MonTracker : public pvac::ClientChannel::MonitorCallback,
                      public T::type,
                      public std::enable_shared_from_this<MonTracker<T>>
  {
  public:
    POINTER_DEFINITIONS(MonTracker<T>);

    T&                 workQueue;
    const std::string  name;
    pvac::Monitor      mon;

    MonTracker(const std::string& n, T& wq) : name(n), workQueue(wq) { }
    virtual ~MonTracker() { mon.cancel(); }

    virtual void monitorEvent(const pvac::MonitorEvent& evt) OVERRIDE FINAL
    {
      // shared_from_this() will fail as Cancel is delivered in our dtor.
      if( evt.event == pvac::MonitorEvent::Cancel ) return;

      // running on internal provider worker thread
      // minimize work here.
      // TODO: bound queue size
      workQueue.push(this->weak_from_this(), evt);
    }

    virtual void process(const pvac::MonitorEvent& evt)
    {
      // running on our worker thread
      switch(evt.event) {
        case pvac::MonitorEvent::Fail:
          std::cout<<"Error "<<name<<" "<<evt.message<<"\n";
          break;
        case pvac::MonitorEvent::Cancel:
          std::cout<<"Cancel "<<name<<"\n";
          break;
        case pvac::MonitorEvent::Disconnect:
          std::cout<<"Disconnect "<<name<<"\n";
          break;
        case pvac::MonitorEvent::Data:
          {
            unsigned n;
            for(n=0; n<2 && mon.poll(); n++) {
              pvd::PVField::const_shared_pointer fld(mon.root->getSubField("value"));
              if(!fld)
                fld = mon.root;

              std::cout<<"Event "<<name<<" "<<fld
              <<" Changed:"<<mon.changed
              <<" overrun:"<<mon.overrun<<"\n";
            }
            if(n==2) {
              // too many updates, re-queue to balance with others
              workQueue.push(this->shared_from_this(), evt);
            }
          }
          break;
      }
    }
  };

  //class Worker;
  //using WorkQueue1      = WorkQueue<Worker>;
  //using MonTracker1     = MonTracker<WorkQueue1>;
  //template<typename T>

  int test_pvmonitor2();
  int test_pvmonitor();


}

#endif

