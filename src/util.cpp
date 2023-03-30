/*
 * =====================================================================================
 *
 *       Filename:  util.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/30/2023 11:44:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cara Salter (muirrum), cara@devcara.com
 *   Organization:  Worcester Polytechnic Institute
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string>
#include <142bot/util.hpp>

std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;

	std::string subject_lc = lowercase(subject);
	std::string search_lc = lowercase(search);
	std::string replace_lc = lowercase(replace);

	while((pos = subject_lc.find(search_lc, pos)) != std::string::npos) {

		 subject.replace(pos, search.length(), replace);
		 subject_lc.replace(pos, search_lc.length(), replace_lc);

		 pos += replace.length();
	}
	return subject;
}
