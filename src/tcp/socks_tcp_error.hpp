#ifndef SOCKS_TCP_ERROR_HPP
#define SOCKS_TCP_ERROR_HPP

#include <exception>
#include <string>

namespace Socks
{

class WSException : public std::exception
{
  public:
  explicit WSException(char const* msg) : msg(msg) {}
  virtual ~WSException() {}
  virtual char const* what() const throw() { return msg.c_str(); }

  private:
  std::string const msg;
};

class NotImplementedException final : public WSException
{
  public:
  explicit NotImplementedException(char const* msg) : WSException(msg) {}
};

class ResourceException final : public WSException
{
  public:
  explicit ResourceException(char const* msg) : WSException(msg) {}
};

} // namespace Socks

#endif /* SOCKS_TCP_ERROR_HPP */
