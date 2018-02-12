#include "PVMonitor.h"

#if !defined(_WIN32)
#include <signal.h>
#define USE_SIGNAL
#endif


namespace {

  using namespace imguiDM ;

  struct Worker {
    virtual ~Worker() {}
    virtual void process(const pvac::MonitorEvent& event) =0;
  };

  using WorkQueue1 = WorkQueue<Worker>;
  WorkQueue1         monwork;
  epicsMutex         mutex;
  epicsEvent         done;
  volatile size_t    waitingFor;

#ifdef USE_SIGNAL
  void sigdone(int num)
  {
    (void)num;
    waitingFor = 0;
    done.signal();
  }
#endif

  using MonTracker1 = MonTracker<WorkQueue1>;

} // namespace

namespace imguiDM {

  int test_pvmonitor2()
  {
    std::string       providerName("ca");
    std::string       requestStr("field()");
    std::vector<std::string> pvs = { std::string("whit:circle:angle") };

    pvd::PVStructure::shared_pointer pvReq(pvd::createRequest(requestStr));
    // explicitly select configuration from process environment
    pva::Configuration::shared_pointer conf(pva::ConfigurationBuilder().push_env().build());

    // "pva" provider automatically in registry
    // add "ca" provider to registry
    pva::ca::CAClientFactory::start();

    std::cout << "Use provider: " << providerName << "\n";
    pvac::ClientProvider provider(providerName, conf);

    using Monitors_vector = std::vector<MonTracker1::shared_pointer>;
    auto  monitors = new Monitors_vector();

    {
      Guard G(mutex);
      waitingFor = pvs.size();
    }

    for(const auto& pv : pvs ){
      MonTracker1::shared_pointer mon(new MonTracker1(pv,monwork));

      pvac::ClientChannel chan(provider.connect(pv));

      mon->mon = chan.monitor(mon.get(), pvReq);

      monitors->push_back(mon);
    }
    return 0;
  }

  int test_pvmonitor()
  {
    epics::RefMonitor refmon;
    double            waitTime = -1.0;
    std::string       providerName("ca");
    std::string       requestStr("field()");
    using pvs_t = std::vector<std::string>;
    pvs_t pvs;
    pvs.push_back( std::string("whit:circle:angle") );

    //int opt;
    //while((opt = getopt(argc, argv, "hRp:w:r:")) != -1) {
    //  switch(opt) {
    //    case 'R':
    //      refmon.start(5.0);
    //      break;
    //    case 'p':
    //      providerName = optarg;
    //      break;
    //    case 'w':
    //      waitTime = pvd::castUnsafe<double, std::string>(optarg);
    //      break;
    //    case 'r':
    //      requestStr = optarg;
    //      break;
    //    case 'h':
    //      std::cout<<"Usage: "<<argv[0]<<" [-p <provider>] [-w <timeout>] [-r <request>] [-R] <pvname> ...\n";
    //      return 0;
    //    default:
    //      std::cerr<<"Unknown argument: "<<opt<<"\n";
    //      return -1;
    //  }
    //}

    //for(int i=optind; i<argc; i++)
    //  pvs.push_back(argv[i]);

#ifdef USE_SIGNAL
    signal(SIGINT, sigdone);
    signal(SIGTERM, sigdone);
    signal(SIGQUIT, sigdone);
#endif

    // build "pvRequest" which asks for all fields
    pvd::PVStructure::shared_pointer pvReq(pvd::createRequest(requestStr));

    // explicitly select configuration from process environment
    pva::Configuration::shared_pointer conf(pva::ConfigurationBuilder()
                                            .push_env()
                                            .build());

    // "pva" provider automatically in registry
    // add "ca" provider to registry
    pva::ca::CAClientFactory::start();

    std::cout<<"Use provider: "<<providerName<<"\n";
    pvac::ClientProvider provider(providerName, conf);

    std::vector<MonTracker1::shared_pointer> monitors;

    {
      Guard G(mutex);
      waitingFor = pvs.size();
    }

    for(const auto& pv : pvs ){

      MonTracker1::shared_pointer mon(new MonTracker1(pv,monwork));

      pvac::ClientChannel chan(provider.connect(pv));

      mon->mon = chan.monitor(mon.get(), pvReq);

      monitors.push_back(mon);
    }

    int ret = 0;
    {
      Guard G(mutex);
      while(waitingFor) {
        UnGuard U(G);
        if(waitTime<0.0) {
          done.wait();
        } else if(!done.wait(waitTime)) {
          std::cerr<<"Timeout\n";
          ret = 1;
          break; // timeout
        }
      }
    }

    if(refmon.running()) {
      refmon.stop();
      // drop refs to operations, but keep ref to ClientProvider
      monitors.clear();
      // show final counts
      refmon.current();
    }

    monwork.close();
    return ret;
  }

}

