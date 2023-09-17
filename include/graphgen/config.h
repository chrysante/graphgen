#ifndef GRAPHGEN_CONFIG_H_
#define GRAPHGEN_CONFIG_H_

#include <string>

namespace graphgen {

/// \Returns the name of the currently set default font
std::string defaultFont();

/// Set the default font to \p fontname
void defaultFont(std::string fontname);

} // namespace graphgen

#endif // GRAPHGEN_CONFIG_H_
