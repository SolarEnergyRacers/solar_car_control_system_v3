//
// Handle config file
//

#ifndef __CONFIG_FILE_H__
#define __CONFIG_FILE_H__

// https://renenyffenegger.ch/notes/development/libraries/cpp/read-configuration-files/
// https://github.com/ReneNyffenegger/cpp-read-configuration-files

#include <map>
#include <string>

using namespace std;

class ConfigFile {
private:
  std::map<string, string> content_;

public:
  ConfigFile(const string &configFile);
  // ConfigFile();

  //  const string get(const string &section, const string &entry, string default_value) const;
  const string get(const string &section, const string &entry, char const *default_value);
  bool get(const string &section, const string &entry, bool default_value);
  int get(const string &section, const string &entry, int default_value);
  float get(const string &section, const string &entry, float default_value);
  double get(const string &section, const string &entry, double default_value);
};

#endif
