#include "ConfigFile.h"

#include <fmt/core.h>
#include <fmt/printf.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <Console.h>
#include <SDCard.h>

extern Console console;
extern SDCard sdCard;

using namespace std;

string trim(string const &source, char const *delims = " \t\r\n") {
  string result(source);
  string::size_type index = result.find_last_not_of(delims);
  if (index != string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if (index != string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

ConfigFile::ConfigFile(const string &configFile) {
  if (sdCard.update_sd_card_detect()) {
    console << "Start reading CONFIG.INI:" << configFile.c_str() << "\n";
    if (sdCard.mount()) {
      try {
        File configFileStream = SD.open(configFile.c_str(), FILE_READ);
        if (!configFileStream) { 
          cerr << "ERROR: opening config file '" << configFile.c_str() << "'" << NL; 
          return; 
        }
        string inSection = "UNKNOWN";
        string line;
        console << "Start reading..." << NL;
        for (int lineNr = 0; configFileStream.available(); lineNr++) {
          line = configFileStream.readStringUntil('\n').c_str();
          console << lineNr << ": " << line << NL;
          
          if (!line.length())
            continue;

          if (line[0] == '#')
            continue;
          if (line[0] == ';')
            continue;

          if (line[0] == '[') {
            inSection = trim(line.substr(1, line.find(']') - 1));
            continue;
          }

          int posEqual = line.find('=');
          string name = trim(line.substr(0, posEqual));
          string value = trim(line.substr(posEqual + 1));

          // remove line end comment
          posEqual = value.find('#');
          value = trim(value.substr(0, posEqual));
          content_[inSection + '/' + name] = value;
        }
      } catch (exception &ex) {
        console << "WARN: No readable configfile: '" << configFile << "' found: " << ex.what() << "\n";
      }
    } else {
      content_["no_config_info/not_found"] = "no value";
      console << "WARN: No readable configfile: '" << configFile << "' found. Using coded settings.\n";
    }
  } else {
    console << "WARN: No SD card found.\n";
  }
}

string const ConfigFile::get(const string &section, const string &entry, const char *default_value) {
  std::map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end())
    return default_value;

  return ci->second;
}

bool ConfigFile::get(const string &section, const string &entry, bool default_value) {
  std::map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end()) {
    return default_value;
  }
  bool value = default_value;
  istringstream(ci->second) >> value;
  return value;
}

int ConfigFile::get(const string &section, const string &entry, int default_value) {
  std::map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end()) {
    return default_value;
  }
  return stoi(ci->second);
}

float ConfigFile::get(const string &section, const string &entry, float default_value) {
  std::map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end()) {
    return default_value;
  }
  return stof(ci->second);
}

double ConfigFile::get(const string &section, const string &entry, double default_value) {
  std::map<string, string>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end()) {
    return default_value;
  }
  return stod(ci->second);
}
