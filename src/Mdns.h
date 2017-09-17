/*
   Copyright 2017 Steven Hessing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 * Mdns.h
 *
 *  Created on: September 16, 2017
 *      Author: Steven Hessing (steven.hessing@gmail.com)
 */

#ifndef MDNS_H_
#define MDNS_H_

#include "iDeviceInfoSource.h"
#include "HostCache.h"
#include "noddos.h"

#include <regex>
#include <string>
#include <ctime>
#include <memory>
#include <sys/epoll.h>

#include "tins/dns.h"

class Mdns : public iDeviceInfoSource {
private:
    int socket_fd;
    std::string IpAddress;
    time_t Expiration;
    HostCache &hCache;
    bool Debug;
    std::regex wsdtypes_rx, wsdxaddrs_rx;
    time_t nextProbe = 0;

public:
    Mdns(HostCache &inhCache, const time_t inExpiration, const std::string inIpAddress = "", const bool inDebug = false):
            hCache{inhCache}, Expiration{inExpiration}, IpAddress{inIpAddress}, Debug{inDebug} {
        socket_fd = -1;
        wsdtypes_rx = std::regex(R"delim(<wsd:Types>(.*?)</wsd:Types>)delim",
                std::regex_constants::ECMAScript | std::regex_constants::icase | std::regex_constants::optimize);
        wsdxaddrs_rx = std::regex(R"delim(<wsd:XAddrs>(.*?)</wsd:XAddrs>)delim",
                std::regex_constants::ECMAScript | std::regex_constants::icase | std::regex_constants::optimize);

        Open (IpAddress);

    }
    ~Mdns() {
        // TODO Auto-generated destructor stub
    }
    int Open(std::string input, uint32_t inExpiration = 0);
    bool Close();
    bool processEvent(struct epoll_event &event);
    int getFileHandle();

    void ParseMdnsMessage (std::shared_ptr<MdnsHost> wsdHost, const unsigned char * msgbuf, const int nbytes);

};

#endif /* MDNS_H_ */
