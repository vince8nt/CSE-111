// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <string.h>

using namespace std;

#include "debug.h"
#include "file_sys.h"


// each inode has a unique number
int inode::next_inode_nr {1};


// class file_type -----------------------------------------------
// returns the file type
struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

// string representation of file type
ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}


// class inode_state --------------------------------------------
// inode_state constructor
inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;

   (*root->contents).addDirents(".", root);
   (*root->contents).addDirents("..", root);
}

// returns the string: "% "
const string& inode_state::prompt() const { return prompt_; }

// string representation of inode state
ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

void inode_state::setPrompt(string prompt) {
   prompt_ = prompt;
}

inode_ptr inode_state::get_inode(string input) {
   char * dir = strdup(input.c_str());
   inode_ptr ans;
   if (input == "") return cwd;
   if (input.at(0) == '/') ans = root;
   else ans = cwd;
   char * pch;
   pch = strtok (dir, "/");
   
   while (pch != NULL) {
      // TODO What if directory not existing
      try {
         ans = (*ans->contents).getDirents().at(pch);
      }
      catch (const out_of_range &) {
         return nullptr;
      }
      pch = strtok (NULL, "/");
   }
   return ans;
}



// class inode --------------------------------------------------
// constructor
inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           myType = "PLAIN_TYPE";
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           myType = "DIRECTORY_TYPE";
           contents = make_shared<directory>();
           break;
   }
   // remove this later
   // cout << "inode " << inode_nr << ", type = " << type << "\n";
}

// getter for number
int inode::get_inode_nr() const {
   return inode_nr;
}

string inode::get_file_type() {
   return myType;
}

int inode::get_file_size() {
   return (*contents).size();
}

void inode::rm(string name) {
   (*contents).remove(name);
}

map<string,inode_ptr> inode::getDirents() {
   return (*contents).getDirents();
}

void inode::add_dirents(string name, inode_ptr thing) {
   (*contents).addDirents(name, thing);
}




// file_error class ---------------------------------------------
// constructor
file_error::file_error (const string& what):
            runtime_error (what) {
}


// shouldn't have to change
// class base_file ----------------------------------------------
// all methods must be overridden
// returnst the string value of the file data
const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

// writes to the file
void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

// deletes the file
void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

// makes a directory
inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

// makes a file
inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

// does nothing - shouldn't be called
map<string,inode_ptr> base_file::getDirents() {
   cout << "wrong getDirents";
   throw file_error ("is a " + error_file_type());
}

// does nothing - shouldn't be called
void base_file::addDirents(string, inode_ptr) {
   cout << "wrong getDirents";
   throw file_error ("is a " + error_file_type());
}




// good for now
// class plain_file ------------------------------------------
// returns the file size in bytes
size_t plain_file::size() const { // obv needs to be changed
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

// returns the data in the file
const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

// writes to the file
void plain_file::writefile (const wordvec& words) {
   data = words;
   // cout << "wrote to file: " << words << "\n";
}


// good for now
// class directory -------------------------------------------
// returns the size in bytes of the directory
directory::directory() {
   dirents = map<string,inode_ptr>();
}

size_t directory::size() const {
   size_t size {0};
   // cout << "size = " << size << "\n";
   return size;
}

// removes the directory (and everything inside of it)
void directory::remove (const string& filename) {
   dirents.erase(filename);
   cout << "removed " << filename << "\n";
}

// makes a directory
inode_ptr directory::mkdir (const string& dirname) {
   inode_ptr ans = make_shared<inode>(file_type::DIRECTORY_TYPE);
   dirents.insert(
      std::pair<string,inode_ptr>(dirname, ans)
   );
   // cout << "made new directory: " << dirname << "\n";
   return ans;
}

// makes a file
inode_ptr directory::mkfile (const string& filename) {
   inode_ptr ans = make_shared<inode>(file_type::PLAIN_TYPE);
   dirents.insert(
      std::pair<string,inode_ptr>(filename, ans)
   );
   // cout << "made new file: " << filename << "\n";
   return ans;
}

// getter for dirents
map<string,inode_ptr> directory::getDirents() {
   return dirents;
}

// inserter
void directory::addDirents(string name, inode_ptr thing) {
   dirents.emplace(name, thing);
}









