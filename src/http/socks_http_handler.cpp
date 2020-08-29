#include <socks_http_handler.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

namespace Socks
{
namespace Network
{
namespace Http
{

constexpr char const* const NEWLINE = "\r\n";

static void parseHeaders(char const* request, std::size_t len, HeadersMap& headers_)
{
  (void)len; // TODO use for bounds checking

  std::stringstream requestStream;
  std::string line;

  requestStream.str(request);
  std::getline(requestStream, line); // omit first request line
  while (std::getline(requestStream, line))
  {
    std::string const delimiter = ": ";
    std::size_t pos = line.find(delimiter);
    if (pos == std::string::npos)
    {
      break;
    }
    std::string key = line.substr(0, pos);
    std::string value = line.erase(0, pos + delimiter.length());

    headers_.emplace(std::move(key), std::move(value));
  }
}

static bool hasHeaderField(HeadersMap const& headers, std::string const& key, std::string const& value)
{
  auto itr = headers.find(key);
  if (itr == headers.end())
  {
    return false;
  }
  return itr->second.find(value) != std::string::npos;
}

static bool hasHeaderField(HeadersMap const& headers, std::string const& key) { return headers.find(key) != headers.end(); }

static bool isWsRequest(HeadersMap const& headers)
{
  return hasHeaderField(headers, "Upgrade", "websocket") &&
         (hasHeaderField(headers, "Connection", "Upgrade") || hasHeaderField(headers, "Connection", "upgrade")) &&
         hasHeaderField(headers, "Sec-WebSocket-Key");
}

SOCKS_INLINE void RequestInfo::parse(char const* request, std::size_t len)
{
  if (len < 3)
  {
    throw std::invalid_argument("Request is too short.");
  }

  constexpr char const* const GET = "GET";
  if (std::strncmp(request, GET, std::strlen(GET)))
  {
    throw std::invalid_argument("Unsupported request type.");
  }
  _requestType = RequestType::GET;
  auto path = std::strchr(request, ' ');
  if (!path)
  {
    throw std::invalid_argument("No path given.");
  }
  path += 1; // omit space
  auto pathEnd = std::strchr(path, ' ');
  if (!pathEnd)
  {
    throw std::invalid_argument("No HTTP version given.");
  }
  auto curPos = path;
  for (; curPos < pathEnd && *curPos != '?'; curPos++)
  {
    _path[curPos - path] = *curPos;
  }
  _path[curPos - path] = '\0';

  parseHeaders(request, len, _headers);

  if (isWsRequest(_headers))
  {
    _requestType = RequestType::GET_WS;
  }
}

SOCKS_INLINE HttpConnection::HttpConnection(Socks::Network::Tcp::Connection* tcpConnection) : tcpConnection{tcpConnection} {}

SOCKS_INLINE void HttpConnection::http_200(std::streampos contentLength)
{
  http_response_code("200 OK");
  std::stringstream response;
  response << "Content-Length: " << contentLength << NEWLINE;
  tcpConnection->send(response.str().c_str(), response.str().length());
  end_headers();
}

SOCKS_INLINE void HttpConnection::http_response_code(char const* responseCode)
{
  constexpr char const* const HTTP_VERSION = "HTTP/1.1";

  std::stringstream response;
  response << HTTP_VERSION << " " << responseCode << NEWLINE << "Server: CPP Socks" << NEWLINE;
  tcpConnection->send(response.str().c_str(), response.str().length());
}
SOCKS_INLINE void HttpConnection::end_headers() { tcpConnection->send(NEWLINE, std::strlen(NEWLINE)); }
SOCKS_INLINE void HttpConnection::write(char const* buf, std::size_t len) { tcpConnection->send(buf, len); }

static std::streampos get_file_size(std::string const& path)
{
  std::ifstream in_fs(path, std::ifstream::ate | std::ifstream::binary);
  return in_fs.tellg();
}

SOCKS_INLINE void HttpFileHandler::do_GET(HttpConnection* connection, RequestInfo const& requestInfo)
{
  char const* path = requestInfo.path();

  if (path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))
  {
    path = "index.html";
  }
  else if (path[0] == '/')
  {
    path += 1;
  }
  if (std::strstr(path, "..") != nullptr)
  {
    connection->http_403();
    return;
  }

  std::fstream in(path, std::ios_base::in | std::ios_base::binary);
  if (in.fail())
  {
    spdlog::error("Could not open file: {}", path);
    connection->http_404();
    return;
  }
  connection->http_200(get_file_size(path));
  do
  {
    constexpr size_t BUF_SIZE = 4096;
    char buf[BUF_SIZE];
    in.read(&buf[0], BUF_SIZE);
    connection->write(buf, static_cast<std::size_t>(in.gcount()));
  } while (in.gcount() > 0);
}

} // namespace Http
} // namespace Network
} // namespace Socks
