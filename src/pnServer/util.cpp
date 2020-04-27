#include "util.h"

namespace {

void inplaceUrlDecode( std::string& text )
{
    std::size_t j = 0;

    for (std::size_t i = 0; i < text.length(); ++i) {
        char c = text[i];

        if (c == '+') {
            text[j++] = ' ';
        } else if (c == '%' && i + 2 < text.length()) {
            std::string h = text.substr(i + 1, 2);
            char *e = 0;
            int hval = std::strtol(h.c_str(), &e, 16);

            if (*e == 0) {
                text[j++] = (char)hval;
                i += 2;
            } else {
                // not a proper %XX with XX hexadecimal format
                text[j++] = c;
            }
        } else
            text[j++] = c;
    }

    text.erase(j);
}

}

namespace pacsnode {

Util::ParameterMap Util::parseFromUrlEncoded(const QString& str)
{
    ParameterMap parameters;
    std::string s = str.toStdString();
    for (std::size_t pos = 0; pos < s.length();) {
        std::size_t next = s.find_first_of("&=", pos);

        if (next == std::string::npos || s[next] == '&') {
            if (next == std::string::npos)
                next = s.length();
            std::string key = s.substr(pos, next - pos);
            inplaceUrlDecode(key);
            parameters[key.c_str()].push_back(QString());
            pos = next + 1;
        } else {
            std::size_t amp = s.find('&', next + 1);
            if (amp == std::string::npos)
                amp = s.length();

            std::string key = s.substr(pos, next - pos);
            inplaceUrlDecode(key);

            std::string value = s.substr(next + 1, amp - (next + 1));
            inplaceUrlDecode(value);

            parameters[key.c_str()].push_back(value.c_str());
            pos = amp + 1;
        }
    }
    return parameters;
}

}
