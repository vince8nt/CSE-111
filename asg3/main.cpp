// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break; // reached end of file
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

string delete_whitespace (string cur) {
   // delete front and back whitespace
   // doesn't remove middle whitespace on purpose
   while (cur.length() > 0 && (cur.at(0) == ' ' || cur.at(0) == '\t'))
      cur = cur.substr(1);
   while (cur.length() > 0 && (cur.at(cur.length() - 1) == ' '
      || cur.at(cur.length() - 1) == '\t'))
      cur = cur.substr(0, cur.length() - 1);
   return cur;
}

void do_line (string line, string file, int line_num,
   listmap<const string, const string, xless<const string>>* map) {
   cout << file << ": " << line_num << ": " << line << "\n";
   line = delete_whitespace(line);
   int equ = -1;
   for (int ind = 0; ind < static_cast<int>(line.length()); ++ind) {
      if (line.at(ind) == '=') {
         if (equ != -1) {
            cout << "invalid input: must only contain one '='\n";
            return;
         }
         equ = ind;
      }
   }
   if (line.length() == 0 || line.at(0) == '#') {
      // left blank because it should do nothing
   }
   else if (equ == -1) {
      string key = line;
      // given key, find value
      if (map->find(key) == map->end())
         cout << key << ": key not found\n";
      else
         cout << *map->find(key);
   }
   else if (line.length() == 1) {
      // print all the key and value pairs
      auto iter = map->begin();
      while (iter != map->end()) {
         cout << *iter << "\n";
         ++iter;
      }
   }
   else if (equ == 0) {
      string value = line.substr(1);
      value = delete_whitespace(value);
      // equals at begining
      // given value, find key
      cout << "given value: \"" << value
         << "\", must find key.\n";
   }
   else if (equ == static_cast<int>(line.length()) - 1) {
      string key = line.substr(0, line.length() - 1);
      key = delete_whitespace(key);
      // equals at end
      // given key, delete key value pair
      cout << "given key: \"" << key
         << "\", must delete the pair.\n";
   }
   else {
      string key = line.substr(0, equ);
      string value = line.substr(equ + 1, line.length() - equ - 1);
      key = delete_whitespace(key);
      value = delete_whitespace(value);
      // equals in the middle
      // create/modify key value pair
      cout << "given key: \"" << key
         << "\", must set the value to \""
         << value << "\".\n";
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   int status = EXIT_SUCCESS;
   listmap<const string, const string, xless<const string>>* map
      = new listmap<const string, const string, xless<const string>>;

   // very ugly but it works
   if (argc == 1) {
      string line;
      for (int line_num = 1;; ++line_num) {
         getline(cin, line);
         if (cin.eof()) break;
         do_line(line, "-", line_num, map);
      }
   }
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      string file_name = *argp;
      string line;
      if (file_name == "-") {
         for (int line_num = 1;; ++line_num) {
            getline(cin, line);
            if (cin.eof()) break;
            do_line(line, "-", line_num, map);
         }
      }
      else {
         ifstream inFile;
         inFile.open(*argp);
         if (inFile.rdstate() & ifstream::failbit) {
            cerr << *argp << ": file not found\n";
            status = EXIT_FAILURE;
         }
         else for (int line_num = 1;; ++line_num) {
            if (!inFile.good()) break;
            getline(inFile, line);
            do_line(line, file_name, line_num, map);
         }
         inFile.close();
      }
   }

   //cout << "EXIT_SUCCESS" << endl;
   return status;
}

