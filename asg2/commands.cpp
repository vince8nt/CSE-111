// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (cmd.at(0) == '#') {
      throw command_error (""); // need to fix but detects correctly
   }
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 2) {
      map<string,inode_ptr> hello =  state.getDirents();
      for (std::map<string, inode_ptr>::iterator it=hello.begin();
         it != hello.end(); ++it) {
         if (it->first == words.at(1)) {
            if ( (*it->second).get_file_type() == "PLAIN_TYPE") {
               // print the file
            }
            else {
               throw command_error (words.at(1) + " is a directory");
            }
         }
      }
      throw command_error (words.at(1) + " no such file");
   }
   else
      throw command_error ("incorrect input");
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

// seems to be working

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // have to modify this to work for other paths than cwd
   map<string,inode_ptr> hello =  state.getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      cout << (*it->second).get_file_size();
      cout << "   " << it->first << "\n";
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2) {
      throw command_error ("incorrect input");
   }
   map<string,inode_ptr> hello =  state.getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      if (it->first == words.at(1)) {
         throw command_error (words.at(1) + " already exists");
      }
   }
   state.mkdir(words.at(1));
}

// working yay!
void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if ((words.size() == 1))
      throw command_error ("incorrect input");
   string prompt = "";
   for (unsigned int counter = 1; counter < words.size(); ++counter) {
      prompt += words.at(counter) + " ";
   }
   state.setPrompt(prompt);
}

// sort of working
void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   map<string,inode_ptr> hello =  state.getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      cout << it->first << " ";
   }
   cout << "\n";
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2)
      throw command_error ("rm: invalid input operand number");
   map<string,inode_ptr> hello =  state.getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      if (it->first == words.at(1)) {
         if ( (*it->second).get_file_type() == "DIRECTORY_TYPE") {
            if (false) {

            }
         }
         // delete it
      }
   }
   throw command_error ("rm: " + words.at(1) + " no such file");
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}









