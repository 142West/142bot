/*
 * =====================================================================================
 *
 *       Filename:  db.cpp
 *
 *    Description:  Connection to a postgres database 
 *
 *        Version:  1.0
 *        Created:  04/06/2023 11:40:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cara Salter (muirrum), cara@devcara.com
 *   Organization:  Worcester Polytechnic Institute
 *
 * =====================================================================================
 */
#include <fmt/core.h>
#include <mutex>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <142bot/db.hpp>
#include <pqxx/pqxx>
#include <fmt/format.h>
#include <cstdarg>
#include <sentry.h>

using namespace std;

namespace db { 
    std::mutex db_mutex;
    std::string _error;

    /**
     * Connects to a postgres database, returns false if error
     **/
    pqxx::connection connect(const std::string &host, const std::string &user, const std::string &pass, const std::string &db, int port) {
        std::lock_guard<std::mutex> db_lock(db_mutex);

        
        std::string cn_s = "postgresql://";

        if (!user.empty()) {
            cn_s = cn_s + user;
        }
        if (!pass.empty() && !user.empty()) {
            cn_s = cn_s + ":" + pass;
        }

        if ((!user.empty() || !pass.empty())) {
            cn_s = cn_s + "@";
        }

        if (!host.empty()) {
            cn_s = cn_s + "localhost";
        }
        if (port != 0 && !host.empty()) {
            cn_s = cn_s + ":" + std::to_string(port);
        }
        if (!db.empty()) {
            cn_s = cn_s + "/" + db;
        } 

        sentry_value_t crumb = sentry_value_new_breadcrumb("default", "Started Database Connection");
        sentry_value_set_by_key(crumb, "level", sentry_value_new_string("db"));
        sentry_value_set_by_key(crumb, "data", sentry_value_new_string(cn_s.c_str()));
        sentry_add_breadcrumb(crumb);

        try {
            pqxx::connection c{cn_s};
            return c;
        } catch (std::exception const &e) {
            _error = e.what();
            throw e;
        }
    }

    const std::string& error() {
        return _error;
    }

}

#include <142bot/date.h>

#include <142bot/iso_week.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>    // std::invalid_argument

namespace asdf
{
    timestamp from_iso8601_str( const std::string& s )
    {
        timestamp ts;
        if( !from_iso8601_str( s, ts ) )
            throw std::invalid_argument{
                "failed to parse "
                + s
                + " as an ISO 8601 timestamp"
            };
        return ts;
    }
    
    bool from_iso8601_str( const std::string& s, timestamp& ts )
    {
        std::istringstream stream{ s };
        stream >> date::parse( "%F %T", ts );
        return !stream.fail();
    }
    
    std::string to_iso8601_str( const timestamp& ts )
    {
        return date::format( "%F %T", ts );
    }
    
    std::string to_http_ts_str( const timestamp& ts )
    {
        std::stringstream weekday_abbreviation;
        weekday_abbreviation << static_cast< iso_week::year_weeknum_weekday >(
            std::chrono::time_point_cast< date::days >( ts )
        ).weekday();
        
        return (
            weekday_abbreviation.str()
            // timestamps serialize to UTC/GMT by default
            + date::format(
                " %d-%m-%Y %H:%M:%S GMT",
                std::chrono::time_point_cast< std::chrono::seconds >( ts )
            )
        );
    }
    
    timestamp from_unix_time( unsigned int unix_time )
    {
        return timestamp{ std::chrono::duration_cast<
            std::chrono::microseconds
        >( std::chrono::seconds{ unix_time } ) };
    }
    
    unsigned int to_unix_time( const timestamp& ts )
    {
        return std::chrono::duration_cast<
            std::chrono::seconds
        >( ts.time_since_epoch() ).count();
    }
}