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
#include <142bot/date.h>


namespace db {
    typedef std::map<std::string, std::string> row;
    typedef std::vector<row> resultset;

    typedef std::vector<std::variant<float, std::string, uint64_t, int64_t, bool, int32_t, uint32_t, double>> paramlist;

    pqxx::connection connect(const std::string cn_s);
    bool close();
    resultset query(const std::string &format, const paramlist &parameters);
    const std::string& error();
}


/**
 * https://gist.github.com/JadeMatrix/ef9c5b0292d3370c674233355850528b
*/
namespace asdf {
    using timestamp = date::sys_time< std::chrono::microseconds >;
    
    timestamp from_iso8601_str( const std::string&             );
    bool      from_iso8601_str( const std::string&, timestamp& );
    std::string to_iso8601_str( const timestamp& );
    std::string to_http_ts_str( const timestamp& );
    
    timestamp  from_unix_time( unsigned int );
    unsigned int to_unix_time( const timestamp& );
}

// Template specialization of `pqxx::string_traits<>(&)` for
// `asdf::timestamp`, which allows use of `pqxx::field::to<>(&)` and
// `pqxx::field::as<>(&)`
namespace pqxx
{
    template<> struct string_traits< asdf::timestamp >
    {
        using subject_type = asdf::timestamp;
        
        static constexpr const char* name() noexcept {
            return "asdf::timestamp";
        }
        
        static constexpr bool has_null() noexcept { return false; }
        
        static bool is_null( const asdf::timestamp& ) { return false; }
        
        [[noreturn]] static asdf::timestamp null()
        {
            internal::throw_null_conversion( name() );
        }
        
        static asdf::timestamp from_string( std::string_view &text )
        {
            asdf::timestamp ts;
            if( !asdf::from_iso8601_str( std::string{ text }, ts ) )
                throw argument_error{
                    "Failed conversion to "
                    + static_cast< std::string >( name() )
                    + ": '"
                    + static_cast< std::string >( text )
                    + "'"
                };

            return ts;
        }
        
        static std::string to_string( const asdf::timestamp& ts )
        {
            return asdf::to_iso8601_str( ts );
        }

        static char* into_buf(char* begin, char* end, asdf::timestamp const &value) {
            return pqxx::string_traits<std::string>::into_buf(begin, end, asdf::to_iso8601_str(value));
        }

        static zview to_buf(char* begin, char* end, asdf::timestamp const &value) {
            return pqxx::string_traits<std::string>::to_buf(begin, end, asdf::to_iso8601_str(value));
        }

        static std::size_t size_buffer(asdf::timestamp const &value) noexcept {
            return pqxx::string_traits<std::string>::size_buffer(asdf::to_iso8601_str(value));
        }
    };

    template<> struct nullness<asdf::timestamp> {
        static constexpr bool has_null = false;
        static constexpr bool always_null = false;
        static constexpr bool is_null(asdf::timestamp t) noexcept {
            return false;
        }
        static constexpr asdf::timestamp *null() { return nullptr; }
    };
}