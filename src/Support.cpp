//
//
//  @ Project : ircppbot
//  @ File Name : Support.cpp
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


#include "include/Support.h"
#include <core/Global.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <boost/algorithm/string.hpp>
#include <core/DatabaseData.h>
extern "C" ModuleInterface* create()
{
    return new Support;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

Support::Support()
{
}

Support::~Support()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}
void Support::Init(DataInterface* pData)
{
	mpDataInterface = pData;
	mpDataInterface->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(mpDataInterface);
    timerlong();
    command_table = "SupportCommands";
    DatabaseData::Instance().DatabaseData::AddBinds(command_table);
}


void Support::stop()
{
    run = false;
    mpDataInterface->stop();
    std::cout << "Support::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void Support::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Support::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Support::parse_privmsg, this)));
}

void Support::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = mpDataInterface->GetRawQueue();
        ParseData(data);
    }
}

void Support::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = mpDataInterface->GetPrivmsgQueue();
        PRIVMSG(data, "!");
    }
}


void Support::ParseData(std::vector< std::string > data)
{
}


void Support::ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    UsersInterface& U = Global::Instance().get_Users();
    std::string auth = U.GetAuth(nick);
    std::string bind_command = DatabaseData::Instance().GetCommandByBindNameAndBind(command_table, command);
    int bind_access = DatabaseData::Instance().GetAccessByBindNameAndBind(command_table, command);
	std::cout << bind_command << " " << bind_access << std::endl;

	// work
	if (boost::iequals(bind_command, "work"))
	{
		if (args.size() >= 1)
		{
			work(nick, auth, bind_access);
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}

	// support
	if (boost::iequals(bind_command, "support"))
	{
		if (args.size() >= 1)
		{
			std::string supportstring = args[0];
			if (args.size() >= 2)
			{
				for (unsigned int i = 1; i < args.size(); i++)
				{
					supportstring = supportstring + " " + args[i];
				}
			}
			support(nick, auth, supportstring);
		}
		else
		{
			//help(bind_command);
		}
		overwatch(bind_command, command, chan, nick, auth, args);
	}
}


void Support::timerrun()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    longtime++;
    if (longtime >= 100)
    {
        timerlong();
        longtime = 0;
    }
    for (int i = timer_sec.size() - 1; i >= 0; i--)
    {
        if (timer_sec[i] < Tijd)
        {
            std::cout << timer_command[i] << std::endl;
            timer_sec.erase(timer_sec.begin()+i);
            timer_command.erase(timer_command.begin()+i);
        }
    }
}

void Support::timerlong()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    Tijd = Tijd + 100;
    for (int i = timer_long_sec.size() - 1; i >= 0; i--)
    {
        if (timer_long_sec[i] < Tijd)
        {
            std::cout << "timer_long to timer " << timer_long_command[i] << std::endl;
            timer_sec.push_back(timer_long_sec[i]);
            timer_command.push_back(timer_long_command[i]);
            timer_long_sec.erase(timer_long_sec.begin()+i);
            timer_long_command.erase(timer_long_command.begin()+i);
        }
    }
}

void Support::work(std::string nick, std::string auth, int oa)
{
	UsersInterface& U = Global::Instance().get_Users();
    std::string returnstring;
    int oaccess = U.GetOaccess(nick);
    if (oaccess >= oa)
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("say", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void Support::support(std::string nick, std::string auth, std::string supportstring)
{
    std::cout << "Support::support" << std::endl;
    support_ret = support_nick_string_map.insert (std::pair< std::string, std::string > (nick, supportstring));
    if (support_ret.second == true)
    {
        std::cout << "new ticket" << std::endl;
        support_nick.push_back(nick);
    }
    else
    {
        std::cout << "nickname has already a ticket" << std::endl;
    }
}
