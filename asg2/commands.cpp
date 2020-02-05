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

// working
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2)
      throw command_error ("incorrect input");
   inode_ptr node = state.get_inode(words.at(1));
   if (node == nullptr) {
      throw command_error ("no such file");
   }
   if ((*node).get_file_type() != "PLAIN_TYPE") {
      throw command_error ("is a directory not a file");
   }
   wordvec& print = (*node).get_words();
   for (unsigned int counter = 0; counter < print.size(); ++counter) {
      cout << print.at(counter) << " ";
   }
   cout << "\n";
}

// working
void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2)
      throw command_error ("incorrect input");
   inode_ptr node = state.get_inode(words.at(1));
   if (node == nullptr) {
      throw command_error ("no such directory");
   }
   if ((*node).get_file_type() == "PLAIN_TYPE") {
      throw command_error ("is a file not a directory");
   }
   state.setCwd(node);
}

// working
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

// working good
void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() > 2)
      throw command_error ("incorrect input");
   inode_ptr node;
   if (words.size() == 1) node = state.get_inode("");
   else node = state.get_inode(words.at(1));
   if (node == nullptr) {
      throw command_error ("no such directory");
   }
   if ((*node).get_file_type() == "PLAIN_TYPE") {
      throw command_error ("is a file not a directory");
   }
   map<string,inode_ptr> hello = (*node).getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      cout << "     " << (*it->second).get_inode_nr();
      cout << "     " << (*it->second).get_file_size();
      cout << "  " << it->first;
      cout << "\n";
   }

}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() < 2)
      throw command_error ("incorrect input");
   
   string input = words.at(1);
   long back = input.length() - 1;

   while (back >= 0 && input.at(back) != '/') {
      --back;
   }
   string name = input.substr(back + 1);
   input.resize(back + 1);

   inode_ptr node = state.get_inode(words.at(1));
   if (node != nullptr) {
      throw command_error ("file already exists");
   }
   node = state.get_inode(input);
   if (node == nullptr) {
      throw command_error ("no such directory");
   }
   vector<string> writer;
   for (unsigned int counter = 2; counter < words.size(); ++counter) {
      writer.push_back(words.at(counter));
   }
   inode_ptr maker = make_shared<inode>(inode(file_type(0)));
   (*maker).setWords(writer);
   (*node).add_dirents(name, maker);
}

// working good!
void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2)
      throw command_error ("incorrect input");
   
   string input = words.at(1);
   long back = input.length() - 1;

   while (back >= 0 && input.at(back) != '/') {
      --back;
   }
   string name = input.substr(back + 1);
   input.resize(back + 1);

   inode_ptr node = state.get_inode(words.at(1));
   if (node != nullptr) {
      throw command_error ("file already exists");
   }
   node = state.get_inode(input);
   if (node == nullptr) {
      throw command_error ("no such directory");
   }
   (*node).add_dirents(name, make_shared<inode>(inode(file_type(1))));

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


void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if ((words.size() != 1))
      throw command_error ("incorrect input");
   inode_ptr node = state.get_inode("");
   printBefore(node);

   
}

void printBefore(inode_ptr node) {
   if((*node).getDirents().at("..") != node) {
      printBefore((*node).getDirents().at(".."));
   }
   
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   /*
   if (words.size() != 2)
      throw command_error ("rm: invalid input operand number");
   map<string,inode_ptr> hello =  state.getDirents();
   for (std::map<string, inode_ptr>::iterator it=hello.begin();
      it != hello.end(); ++it) {
      if (it->first == words.at(1)) {
         if ( (*it->second).get_file_type() == "DIRECTORY_TYPE") {
            if ((*it->second).get_file_size() <= 2) {
               cwd.rm(words.at(1));
               return;
            }
            else throw command_error (
               "rm: " + words.at(1) + ": directory is not empty");
         }
         cwd.rm(words.at(1));
         return;
      }
   }
   throw command_error ("rm: " + words.at(1) + ": no such file");
   */
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}









