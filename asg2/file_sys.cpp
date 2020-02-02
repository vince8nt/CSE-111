// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

// each inode has a unique number
int inode::next_inode_nr {1};

// either returns the file type or size? not sure which
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

// inode_state constructor
inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

// prompt that comes in
const string& inode_state::prompt() const { return prompt_; }

// string representation of inode state
ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

// inode constructor
inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

// returns the inode number
int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


// file_error constructor
file_error::file_error (const string& what):
            runtime_error (what) {
}

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


// returns the file size in bytes
size_t plain_file::size() const {
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
   DEBUGF ('i', words);
}

// returns the size in bytes of the directory
size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

// removes the directory (and everything inside of it)
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

// makes a directory
inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   return nullptr;
}

// makes a file
inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

