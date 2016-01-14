#include "BridgeConfig.h"
#include "Common/Log.h"

#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <string.h>

#include <strings.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlsave.h>

namespace
{
  DSB_DECLARE_LOGNAME(BridgeConfig);

  void libXmlGenericErrorFunc(void* /*ctx*/, xmlErrorPtr error)
  {
    if (!error)
      return;

    common::LogLevel level = common::LogLevel::Info;
    switch (error->level)
    {
      case XML_ERR_NONE: level = common::LogLevel::Debug; break;
      case XML_ERR_WARNING: level = common::LogLevel::Warn; break;
      case XML_ERR_ERROR: level = common::LogLevel::Error; break;
      case XML_ERR_FATAL: level = common::LogLevel::Fatal; break;
    }

    // remove newline. I hate blank lines in logfiles.
    if (error->message)
    {
      char* p = strrchr(error->message, '\n');
      if (p)
        *p = '\0';
    }

    char const* msg = error->message;
    if (!msg)
      msg = "<unknown error>";

    common::Logger::Write("libxml", level, error->file, error->line, "%s", msg);
  }

  void installXmlLogger()
  {
    static bool libXmlLoggerInstalled = false;
    if (!libXmlLoggerInstalled)
    {
      xmlSetStructuredErrorFunc(NULL, &libXmlGenericErrorFunc);
      libXmlLoggerInstalled = true;
    }
  }

  inline xmlChar const* toXmlChar(char const* s)
  {
    return reinterpret_cast<xmlChar const *>(s);
  }

  inline char const* toChar(xmlChar const* s)
  {
    return reinterpret_cast<char const *>(s);
  }

  inline xmlChar const* toXmlChar(bool b)
  {
    return b ? toXmlChar("true") : toXmlChar("false");
  }

  char const* kAlljoynObjectPathPrefix = "/BridgeConfig/AdapterDevices/Device[@Id=\"";
  char const* kAdapterDevices = "/BridgeConfig/AdapterDevices";
  char const* kAlljoynObjectPathSuffix = "\"]";
  char const* kDeviceElementName = "Device";
  char const* kIdAttribute = "Id";
  char const* kVisibleAttribute = "Visible";

  std::string MakePathId(std::string const& id)
  {
    std::stringstream buff;
    buff << kAlljoynObjectPathPrefix;
    buff << id;
    buff << kAlljoynObjectPathSuffix;
    return buff.str();
  }

  xmlChar* GetNodeAttribute(const xmlNodePtr node, char const* attr)
  {
    if (node)
    {
      xmlAttr* attrs = node->properties;
      while (attrs && attrs->name && attrs->children)
      {
        if (strcmp(toChar(attrs->name), attr) == 0)
          return xmlNodeListGetString(node->doc, attrs->children, 1);
        attrs = attrs->next;
      }
    }
    return NULL;
  }

  bool ToBoolean(std::string const& s)
  {
    return strcasecmp(s.c_str(), "true") == 0 ? true : false;
  }

  xmlNodePtr SelectSingleNode(xmlXPathContextPtr ctx, char const* xpath)
  {
    xmlNodePtr node = NULL;

    DSBLOG_INFO("select: %s", xpath);

    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(toXmlChar(xpath), ctx);
    if (xpathObj)
    {
      int n = 0;
      if (xpathObj->nodesetval)
        n = xpathObj->nodesetval->nodeNr;

      if (n == 1)
        node = xpathObj->nodesetval->nodeTab[0];
    }

    return node;
  }

  std::string GetElementText(const xmlDocPtr doc, char const* xpath)
  {
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);
    if (!ctx)
      return NULL;

    std::string content;

    xmlNodePtr node = SelectSingleNode(ctx, xpath);
    if (node)
    {
      xmlChar* s = xmlNodeGetContent(node);
      content = std::string(toChar(s));
    }

    xmlXPathFreeContext(ctx);
    DSBLOG_INFO("%s == %s", xpath, content.c_str());
    return content;
  }

  class LibXmlInitializer
  {
  public:
    LibXmlInitializer()
    {
      xmlInitParser();
    }

    ~LibXmlInitializer()
    {
      xmlCleanupParser();
    }
  };

  std::once_flag libXmlInit;
  std::shared_ptr<LibXmlInitializer> libXmlInitializer;
}

bridge::BridgeConfig::BridgeConfig()
  : m_doc(NULL)
{
  installXmlLogger();
}

bridge::BridgeConfig::~BridgeConfig()
{
  if (m_doc)
    xmlFreeDoc(m_doc);
}

QStatus
bridge::BridgeConfig::FromFile(std::string const& fileName)
{
  QStatus st = ER_OK;

  m_fileName = fileName;
  installXmlLogger();

  std::call_once(libXmlInit, [] {
    if (!libXmlInitializer) libXmlInitializer.reset(new LibXmlInitializer());
  });

  xmlDocPtr doc = xmlParseFile(m_fileName.c_str());
  if (!doc)
  {
    DSBLOG_WARN("failed to parse: %s", m_fileName.c_str());
    st = ER_FAIL;
  }
  if (m_doc)
    xmlFreeDoc(m_doc);

  m_doc = doc;
  return st;
}

QStatus
bridge::BridgeConfig::ToFile(std::string const& fileName)
{
  if (!m_doc)
    return ER_FAIL;

  char const* outfile = m_fileName.c_str();
  if (!fileName.empty())
    outfile = fileName.c_str();

  // TODO: use TextWriter to actually indent this stuff
//  xmlSaveCtxtPtr ctx = xmlSaveToFilename(outfile, NULL, XML_SAVE_AS_HTML);
//  xmlSaveDoc(ctx, m_doc);
//  xmlSaveClose(ctx);
  xmlKeepBlanksDefault(1);
  xmlSaveFormatFile(outfile, m_doc, 1);

  return ER_OK;
}

QStatus
bridge::BridgeConfig::ToString(std::string& out)
{
  xmlBufferPtr buff = xmlBufferCreate();

  // TODO: use TextWriter to actually indent this stuff
  xmlSaveCtxtPtr ctx = xmlSaveToBuffer(buff, NULL, XML_SAVE_FORMAT);
  xmlSaveDoc(ctx, m_doc);
  xmlSaveClose(ctx);

  out = std::string(toChar(buff->content));
  xmlBufferFree(buff);

  return ER_OK;
}

QStatus
bridge::BridgeConfig::AddObject(DsbObjectConfig const& obj)
{
  xmlXPathContextPtr ctx = xmlXPathNewContext(m_doc);
  if (!ctx)
    return ER_FAIL;

  xmlNodePtr parent = SelectSingleNode(ctx, kAdapterDevices);
  if (parent)
  {
    xmlNodePtr node = xmlNewTextChild(parent, NULL,  toXmlChar(kDeviceElementName),
      toXmlChar(obj.GetDescription().c_str()));
    xmlNewProp(node, toXmlChar(kIdAttribute), toXmlChar(obj.GetId().c_str()));
    xmlNewProp(node, toXmlChar(kVisibleAttribute), toXmlChar(obj.IsVisible()));
  }

  xmlXPathFreeContext(ctx);
  return ER_OK;
}

QStatus
bridge::BridgeConfig::RemoveObject(std::string const& id)
{
  xmlXPathContextPtr ctx = xmlXPathNewContext(m_doc);
  if (!ctx)
    return ER_FAIL;

  xmlNodePtr node = SelectSingleNode(ctx, MakePathId(id).c_str());
  if (node)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  xmlXPathFreeContext(ctx);
  return ER_OK;
}

QStatus
bridge::BridgeConfig::FindObject(std::string const& id, DsbObjectConfig& obj)
{
  if (!m_doc)
    return ER_FAIL;

  xmlXPathContextPtr ctx = xmlXPathNewContext(m_doc);
  if (!ctx)
    return ER_FAIL;

  std::string path = MakePathId(id);
  xmlNodePtr node = SelectSingleNode(ctx, path.c_str());
  if (!node)
    return ER_BUS_OBJ_NOT_FOUND;

  obj.SetId(path);

  xmlChar* attr = GetNodeAttribute(node, kVisibleAttribute);
  if (attr)
  {
    bool b = strcasecmp(toChar(attr), "true") == 0
      ? true
      : false;
    obj.SetIsVisible(b);
    xmlFree(attr);
  }

  xmlChar* desc = xmlNodeGetContent(node);
  obj.SetDescription(std::string(toChar(desc)));

  xmlXPathFreeContext(ctx);
  return ER_OK;
}

std::string
bridge::BridgeConfig::GetBridgeKeyX() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Bridge/KEYX");
}

std::string
bridge::BridgeConfig::GetDeviceKeyX() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Device/KEYX");
}

std::string
bridge::BridgeConfig::GetDeviceUsername() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Device/USERNAME");
}

std::string
bridge::BridgeConfig::GetDevicePassword() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Device/PASSWORD");
}

std::string
bridge::BridgeConfig::GetDeviceEcdheEcdsaPrivateKey() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Device/ECDHEECDSAPRIVATEKEY");
}

std::string
bridge::BridgeConfig::GetDeviceEcdheEcdsaCertChain() const
{
  return GetElementText(m_doc, "/BridgeConfig/Settings/Device/ECDHEECDSACERTCHAIN");
}

bool
bridge::BridgeConfig::GetDefaultVisibility() const
{
  std::string s = GetElementText(m_doc, "/BridgeConfig/Settings/Device/DefaultVisibility");
  return ToBoolean(s);
}
