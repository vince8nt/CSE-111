// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);

// exit command
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"get" , cix_command::GET },
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

// get command
void cix_get(client_socket& server, string file) {
   ifstream check(file);
   if (check.good()) {
      outlog << "get: local file already exists" << endl;
      return;
   }
   cix_header header;
   header.command = cix_command::GET;
   for (uint ind = 0;; ++ind) {
      if (ind == 58 || ind == file.size()){
         header.filename[ind] = '\0';
         break;
      }
      header.filename[ind] = file.at(ind);
   }
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      ofstream out (file);
      out << buffer.get();
      out.close();
   }
}

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

// help command
void cix_help() {
   cout << help;
}

// ls command
void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

// put command
void cix_put(client_socket& server, string file) {
   ifstream temp(file);
   if (!temp.good()) {
      outlog << "put: local file does not exist" << endl;
      return;
   }
   string contents = "";
   while (not temp.eof()) {
      char nextChar;
      temp.get(nextChar);
      contents.push_back(nextChar);
   }
   contents.pop_back();
   cix_header header;
   header.command = cix_command::PUT;
   for (uint ind = 0;; ++ind) {
      if (ind == 58 || ind == file.size()){
         header.filename[ind] = '\0';
         break;
      }
      header.filename[ind] = file.at(ind);
   }
   header.nbytes = htonl(contents.size());
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   outlog << "sending contents" << endl;
   send_packet (server, contents.c_str(), contents.size());
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      outlog << "sent PUT, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   }
}

// rm command
void cix_rm(client_socket& server, string file) {
   cix_header header;
   header.command = cix_command::RM;
   for (uint ind = 0;; ++ind) {
      if (ind == 58 || ind == file.size()){
         header.filename[ind] = '\0';
         break;
      }
      header.filename[ind] = file.at(ind);
   }
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      outlog << "sent RM, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
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

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         outlog << "command " << line << endl;

         line = delete_whitespace(line);
         string first, second;
         int inputNum;
         int sep = -1;
         for (uint ind = 0; ind < line.length(); ++ind) {
            if (line.at(ind) == ' ' || line.at(ind) == '\t') {
               sep = ind;
               break;
            }
         }
         if (sep == -1) {
            inputNum = 1;
            first = line;
            second = "";
         }
         else {
            // when inputNum == 3, it means there are 3+ inputs
            inputNum = 2;
            first = line.substr(0, sep);
            second = line.substr(sep);
            second = delete_whitespace(second);
            for (uint ind = 0; ind < second.length(); ++ind) {
               if (second.at(ind) == ' ' || second.at(ind) == '\t') {
                  inputNum = 3;
                  break;
               }
            }
         }
         
         const auto& itor = command_map.find (first);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               if (inputNum == 1) throw cix_exit();
               else outlog << line << ": invalid command" << endl;
               break;
            case cix_command::GET:
               if (inputNum == 2) cix_get(server, second);
               else outlog << line << ": invalid command" << endl;
               break;
            case cix_command::HELP:
               if (inputNum == 1) cix_help();
               else outlog << line << ": invalid command" << endl;
               break;
            case cix_command::LS:
               if (inputNum == 1) cix_ls(server);
               else outlog << line << ": invalid command" << endl;
               break;
            case cix_command::PUT:
               if (inputNum == 2) cix_put(server, second);
               else outlog << line << ": invalid command" << endl;
               break;
            case cix_command::RM:
               if (inputNum == 2) cix_rm(server, second);
               else outlog << line << ": invalid command" << endl;
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

