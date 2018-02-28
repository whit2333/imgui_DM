#ifndef IMGUIDM_HELPERS
#define IMGUIDM_HELPERS

#include "clipp.h"
#include <iostream>
#include <fstream>

/** Test if file exists. */
bool fexists(const std::string& filename) ;

/** Copy Files to current directory. */
void copy_files(const std::vector<std::string>& files);

/** Execute command. */
std::string exec(const char* cmd);
//______________________________________________________________________________
 
template<class C>
void print_help(C cli)
{
  std::cout << make_man_page(cli, "bubble_chamber") << "\n";
}
//______________________________________________________________________________

template<typename T>
void print_usage(T cli, const char* argv0 )
{
  //used default formatting
  std::cout << "Usage:\n" << usage_lines(cli, argv0)
            << "\nOptions:\n" << documentation(cli) << '\n';
}
//______________________________________________________________________________

template<typename T>
void print_man_page(T cli, const char* argv0 ){
  //all formatting options (with their default values)
  auto fmt = clipp::doc_formatting{}
  .start_column(8)                           //column where usage lines and documentation starts
  .doc_column(30)                            //parameter docstring start col
  .indent_size(4)                            //indent of documentation lines for children of a documented group
  .line_spacing(0)                           //number of empty lines after single documentation lines
  .paragraph_spacing(1)                      //number of empty lines before and after paragraphs
  .flag_separator(", ")                      //between flags of the same parameter
  .param_separator(" ")                      //between parameters 
  .group_separator(" ")                      //between groups (in usage)
  .alternative_param_separator("|")          //between alternative flags 
  .alternative_group_separator(" | ")        //between alternative groups 
  .surround_group("(", ")")                  //surround groups with these 
  .surround_alternatives("(", ")")           //surround group of alternatives with these
  .surround_alternative_flags("", "")        //surround alternative flags with these
  .surround_joinable("(", ")")               //surround group of joinable flags with these
  .surround_optional("[", "]")               //surround optional parameters with these
  .surround_repeat("", "...");                //surround repeatable parameters with these
  //.surround_value("<", ">")                  //surround values with these
  //.empty_label("")                           //used if parameter has no flags and no label
  //.max_alternative_flags_in_usage(1)         //max. # of flags per parameter in usage
  //.max_alternative_flags_in_doc(2)           //max. # of flags per parameter in detailed documentation
  //.split_alternatives(true)                  //split usage into several lines for large alternatives
  //.alternatives_min_split_size(3)            //min. # of parameters for separate usage line
  //.merge_alternative_flags_with_common_prefix(false)  //-ab(cdxy|xy) instead of -abcdxy|-abxy
  //.merge_joinable_flags_with_common_prefix(true);    //-abc instead of -a -b -c
  auto mp = make_man_page(cli, argv0, fmt);
  mp.prepend_section("DESCRIPTION", "Bubble chamber simulation");
  mp.append_section("EXAMPLES", " $ bubble_chamber -h ");
  std::cout << mp << "\n";
}
//______________________________________________________________________________


#endif
