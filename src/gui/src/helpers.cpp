#include "helpers.h"

bool fexists(const std::string& filename) {
   std::ifstream ifile(filename.c_str());
   if( ifile ) return true;
   return false;
}
//______________________________________________________________________________

std::string exec(const char* cmd) {
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) return "ERROR";
  char buffer[128];
  std::string result = "";
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
  }
  return result;
}
//______________________________________________________________________________

void copy_files(const std::vector<std::string>& files)
{
  for(const auto& f: files) {
    std::string cmd = std::string("cp ") + f + " .";
    if( fexists(f) ) {
      std::cout << "copying " << f << std::endl;
      exec(cmd.c_str());
    }
  }
}
//______________________________________________________________________________

//______________________________________________________________________________

