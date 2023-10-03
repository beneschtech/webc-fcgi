#include <config.h>
#ifdef HAVE_STRING
#include <string>
#endif

namespace FCGI
{
/**
 * @brief urldecode - Takes a url encoded string and decodes it into its clean,
 * binary representation if needed. Decode is an easy operation, encoding
 * has a standard to follow, but we can accomodate both in this file
 * @param s string to urldecode
 * @return std::string with the urldecoded data
 */
std::string urldecode(std::string s)
{
    std::string rv;
    char *p = s.data();
    const char *ep = &s.data()[s.length()];
    while (*p && p <= ep)
    {
        switch (*p)
        {
        case '+': {
            rv.append(" ");
            break;
        }
        case '%': {
            p++;
            char hexv[3] = {0,};
            hexv[0] = *p;
            p++;
            hexv[1] = *p;
            char c = std::strtol(hexv,nullptr,16) & 0xff;
            rv.append({c});
            break;
        }
        default:
            rv.append({*p});
        }
        p++;
    }
    return rv;
} // urldecode

// Complaint with RFC 3986
std::string urlencode(std::string s)
{
  std::string resvdChars = ":/?#[]@!$&'()*+,;= ";
  /* ":" / "/" / "?" / "#" / "[" / "]" / "@"
   * "!" / "$" / "&" / "'" / "(" / ")"
  / "*" / "+" / "," / ";" / "=" */

  std::string rv;
  for (char c: s)
  {
    if (resvdChars.find(c) == std::string::npos) // not a reserved char
    {
      rv.append({c});
    } else if (c == ' ') {
      rv.append("+");
    } else {
      char buf[8] = { 0, };
      snprintf(buf,sizeof(buf),"%%%02x",c);
      rv.append(buf);
    }
  }
  return rv;
} // urlencode

} // namespace FCGI
