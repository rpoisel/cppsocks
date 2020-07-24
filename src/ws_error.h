#ifndef WS_ERROR_H
#define WS_ERROR_H

#include <exception>
#include <string>

namespace WS
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
  NotImplementedException(char const* msg) : WSException(msg) {}
};

class ResourceException final : public WSException
{
  public:
  ResourceException(char const* msg) : WSException(msg) {}
};

} // namespace WS

#endif // WS_ERROR_H