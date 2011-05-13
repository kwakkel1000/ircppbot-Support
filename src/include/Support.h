//
//
//  @ Project : ircppbot
//  @ File Name : Support.h
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#ifndef Support_H
#define Support_H
#include <core/ModuleBase.h>
#include <interfaces/DataInterface.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class DataInterface;
class Support : public ModuleBase
{
public:
	Support();
	~Support();
    void read();
    void stop();
    void Init(DataInterface* pData);
    void timerrun();

private:

    DataInterface* mpDataInterface;
    void parse_raw();
    void parse_privmsg();
    void ParseData(std::vector< std::string > data);
    void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);
    void work(std::string nick, std::string auth, int oa);
    void support(std::string nick, std::string auth, std::string supportstring);

    void timerlong();
    std::vector<int> timer_sec;
    std::vector< std::string > timer_command;
    std::vector<int> timer_long_sec;
    std::vector< std::string > timer_long_command;
    int longtime;

    bool run;
    std::string command_table;
    boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;


    std::map< std::string, std::string > support_nick_string_map;
    std::map< std::string, std::string >::iterator support_nick_string_it;
    std::pair< std::map< std::string, std::string >::iterator, bool > support_ret;
    std::vector< std::string > support_nick;
};

#endif // Support_H


