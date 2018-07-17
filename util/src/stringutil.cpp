#include <stringutil.h>
#include <logger.h>
#include <string>
#include <sstream>
#include <regex>
#include <locale>
#include <codecvt>
#include <vector>

namespace glass3 {
namespace util {

// string functions
// split a string into substrings using the provided delimiter
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}

	return (elems);
}

// remove all instances of the character provided from the string
std::string& removeChars(std::string& s, const std::string& chars) {  // NOLINT
  //DK REVIEW  These next lines are very fortran-eqsue, so let's break it out so it's a little bit more readable.
  //  John's gonna LOOOOVVVVEE this:
	s.erase(  // call erase on our in/out string "s", to remove the junk that's left at 
            // the end of the string after remove_if() is complete
          remove_if(s.begin(), s.end(),   // call remove_if() on our string - this scans each character in the
                                          // string and deletes it if the function() that is our 3rd parameter in 
                                          // remove_if() call, returns true
                    [&chars](const char& c)  // this is the start of our 3rd paramter: a lambda declaration with a capture clause[&chars] - 
                                             // pass a reference to the "chars" string object into this lambda function, and 
                                             // accept a single additional variable (const char reference to the current character in s)
                    {
		                  return (chars.find(c) != std::string::npos);  // body of our lambda function - return true if the current char (from "s")
                                                                    // is in the list of remove chars ("chars")
	                  }
                   ),
			s.end());                             // final parameter in s.erase() call, which erases from the return vaue of remove_if()
                                            // which is one spot past the last character not removed, to the end of the string
	return (s);
}
}  // namespace util
}  // namespace glass3
