/*
 * =====================================================================================
 *
 *       Filename:  db.hpp
 *
 *    Description:   
 *
 *        Version:  1.0
 *        Created:  04/06/2023 11:38:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cara Salter (muirrum), cara@devcara.com
 *   Organization:  Worcester Polytechnic Institute
 *
 * =====================================================================================
 */

#pragma once
#include <vector>
#include <map>
#include <string>
#include <variant>
#include <pqxx/pqxx>


namespace db {
    typedef std::map<std::string, std::string> row;
    typedef std::vector<row> resultset;

    typedef std::vector<std::variant<float, std::string, uint64_t, int64_t, bool, int32_t, uint32_t, double>> paramlist;

    pqxx::connection connect(const std::string &host, const std::string &user, const std::string &pass, const std::string &db, int port);
    bool close();
    resultset query(const std::string &format, const paramlist &parameters);
    const std::string& error();
}
