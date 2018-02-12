#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <typeinfo>
#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "PVGetList.h"

SCENARIO( "PVGetList is empty", "[PVGetList]" ) {

  GIVEN( "no constructor arguments " ) {
    WHEN( " constructor" ) {
      imguiDM::PVGetList get_list0;
      REQUIRE( 1 == 1 );
    }
    WHEN( "copy constructor" ) {
      auto get_list1 = imguiDM::PVGetList();
      REQUIRE( 1 == 1 );
    }
  GIVEN( "A vector with one PV name" ) {
    std::vector<std::string> pvs = {"whit:circle:angle"};
    WHEN( "normal constructor" ) {
      imguiDM::PVGetList get_list2(pvs);
      REQUIRE( 1 == 1 );
    }
    WHEN( "copy constructor" ) {
      auto  get_list3  = imguiDM::PVGetList(pvs);
      REQUIRE( 1 == 1 );
    }
  }
  GIVEN( "A vector with multiple PV name" ) {

    std::vector<std::string> pvs = {"whit:circle:angle","whit:circle:period"};
    WHEN( "normal constructor" ) {
      imguiDM::PVGetList get_list2(pvs);
      REQUIRE( 1 == 1 );
    }
    WHEN( "copy constructor" ) {
      auto  get_list3  = imguiDM::PVGetList(pvs);
      REQUIRE( 1 == 1 );
    }
  }
  GIVEN( "A vector with repeated PV name" ) {
    std::vector<std::string> pvs = {"whit:circle:angle","whit:circle:period","whit:circle:angle","whit:circle:period"};
    WHEN( "normal constructor" ) {
      imguiDM::PVGetList get_list2(pvs);
      REQUIRE( get_list2.GetN() == 2 );
    }
    WHEN( "copy constructor" ) {
      auto  get_list3  = imguiDM::PVGetList(pvs);
      REQUIRE( get_list3.GetN() == 2 );
      THEN( " when adding more PVs " ) {
        get_list3.AddPV("whit:circle:x");
        get_list3.PrintAll();
      }
    }
  }
      //THEN( "the first arg array size is 0" ) {
      //  REQUIRE( arg_array_size<decltype(f),0> == 0 );
      //}


  }
}

