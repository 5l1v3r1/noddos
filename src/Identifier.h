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

 * Identifier.h
 *
 *  Created on: Mar 25, 2017
 *      Author: Steven Hessing (steven.hessing@gmail.com)
 */

#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_

#include <vector>
#include <map>
#include <memory>

#include <glog/logging.h>

#include "json.hpp"
using nlohmann::json;

#include "noddos.h"
#include "MatchCondition.h"

class Identifier {
private:
    ConfidenceLevel IdentifyConfidenceLevel;
    ConfidenceLevel EnforceConfidenceLevel;
    std::vector<std::shared_ptr<MatchCondition>> MatchConditions;
    std::vector<std::shared_ptr<ContainCondition>> ContainConditions;
    bool Debug;

public:
    Identifier(json &j, const bool inDebug = false): Debug{inDebug} {
        if (j.find("IdentifyConfidenceLevel") == j.end()) {
            LOG(INFO) << "No IdentifyConfidenceLevel set, defaulting to `Low'";
            IdentifyConfidenceLevel = ConfidenceLevel::Low;
        } else {
            if (! j["IdentifyConfidenceLevel"].is_string()) {
                LOG(ERROR) << "IdentifyConfidenceLevel is not a string, defaulting to `Low'";
                IdentifyConfidenceLevel = ConfidenceLevel::Low;;
            } else {
                // TODO *sigh* surely there is a better way!
                std::string cl = j["IdentifyConfidenceLevel"];
                std::transform(cl.begin(), cl.end(), cl.begin(), ::tolower);

                if(cl == "low")
                    IdentifyConfidenceLevel = ConfidenceLevel::Low;
                else if (cl == "medium")
                    IdentifyConfidenceLevel = ConfidenceLevel::Medium;
                else if (cl == "high")
                    IdentifyConfidenceLevel = ConfidenceLevel::High;
            }
        }
        if (j.find("EnforceConfidenceLevel") == j.end()) {
            DLOG_IF(INFO, Debug) << "No EnforceConfidenceLevel set, defaulting to `None'";
            EnforceConfidenceLevel = ConfidenceLevel::None;
        } else {
            if (not j["EnforceConfidenceLevel"].is_string()) {
                LOG(ERROR) << "EnforceConfidenceLevel is not a string, defaulting to `None'";
                EnforceConfidenceLevel = ConfidenceLevel::None;
            } else {
                // TODO *sigh* surely there must be a better way!
                std::string cl = j["EnforceConfidenceLevel"];
                std::transform(cl.begin(), cl.end(), cl.begin(), ::tolower);
                EnforceConfidenceLevel = ConfidenceLevel::None;
                if(cl == "low") {
                    EnforceConfidenceLevel = ConfidenceLevel::Low;
                } else if (cl == "medium") {
                    EnforceConfidenceLevel = ConfidenceLevel::Medium;
                } else if (cl == "high") {
                    EnforceConfidenceLevel = ConfidenceLevel::High;
                }
            }
        }
        if (j.find("MustMatch") == j.end() && j.find("MustContain") == j.end()) {
            LOG(ERROR) << "Identifier has no MustMatch and no MustContain restrictions";
            return;
        }
        if (j.find("MustMatch") != j.end()) {
            if (not j["MustMatch"].is_object()) {
                LOG(ERROR) << "MustMatch condition is not a JSON Object";
            } else {
                for (json::iterator it = j["MustMatch"].begin(); it != j["MustMatch"].end(); ++it ) {
                    DLOG_IF(INFO, Debug) << "Adding MatchCondition " << it.key();
                    auto mc = std::make_shared<MatchCondition>(it.key(), it.value(), Debug);
                    MatchConditions.push_back(mc);
                }
            }
        }
        if (j.find("MustContain") != j.end()) {
            if (!j["MustContain"].is_object()) {
                LOG(ERROR) << "MustContain condition is not a JSON Object";
            } else {
                for (json::iterator it = j["MustContain"].begin(); it != j["MustContain"].end(); ++it) {
                    DLOG_IF(INFO, Debug) << "Adding ContainCondition " << it.key();
                    if (it.value().is_array()) {
                        auto cc = std::make_shared<ContainCondition>(it.key(), it.value(), Debug);
                        ContainConditions.push_back(cc);
                    }
                }
            }
        }
    }
    ~Identifier() {
        DLOG_IF(INFO, Debug) << "Destroying Identifier instance";
    };
    ConfidenceLevel IdentifyConfidenceLevel_get () const { return IdentifyConfidenceLevel; }
    const std::vector<std::shared_ptr<MatchCondition>>& MatchConditions_get() const { return MatchConditions; }
    const std::vector<std::shared_ptr<ContainCondition>>& ContainConditions_get() const { return ContainConditions; }
};

#endif /* IDENTIFIER_H_ */
