// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
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
   // delete front and back spaces
   // doesn't remove middle whitespace on purpose
   while (cur.length() > 0 && (cur.at(0) == ' ' || cur.at(0) == '\t'))
      cur = cur.substr(1);
   while (cur.length() > 0 && (cur.at(cur.length() - 1) == ' '
      || cur.at(cur.length() - 1) == '\t'))
      cur = cur.substr(0, cur.length() - 1);
   return cur;
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   for (;;) {
      try {
         cout << "-: ";
         string line;
         getline (cin, line);
         line = delete_whitespace(line);
         if (line.length() == 0) {
            throw 0;
         }
         else if (line.at(0) == '#') {
            // comment
            // do nothing
         }
         else {
            long equ = -1;
            for (int index = 0;
               index < static_cast<int>(line.length()); ++index) {
               if (line.at(index) == '=') {
                  if (equ != -1) throw 0;
                  equ = index;
               }
            }
            if (equ == -1) {
               string key = line;
               // no equals present
               // given key, find value
               cout << "given key: \"" << key
                  << "\", must find the value.\n";
            }
            else if (line.length() == 1) { // and it is an '='
               // print all the key and value pairs
               cout << "must print all key value pairs\n";
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
               string value =
                  line.substr(equ + 1, line.length() - equ - 1);
               key = delete_whitespace(key);
               value = delete_whitespace(value);
               // equals in the middle
               // create/modify key value pair
               cout << "given key: \"" << key
                  << "\", must set the value to \""
                  << value << "\".\n";
            }
         }
      }
      catch (int exe) {
         cout << "invalid input\n";
      }
   }
   
   

   

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}

