//
// Copyright (c) 2016, Comcast Cable Communications Management, LLC
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or
// without fee is hereby granted, provided that the above copyright notice and this
// permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
// SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
// ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.
//
#pragma once

#include <string>
#include <alljoyn/Status.h>

struct _xmlDoc;

namespace bridge
{
  class BridgeConfig;

  class DsbObjectConfig
  {
    friend class BridgeConfig;
  public:
    std::string GetId() const
      { return m_id; }

    std::string GetDescription() const
      { return m_description; }

    bool IsVisible() const
      { return m_isVisible; }

    bool operator == (DsbObjectConfig const& rhs) const
    {
      return m_id == rhs.m_id && m_isVisible == rhs.m_isVisible &&
        m_description == rhs.m_description;
    }

    bool operator != (DsbObjectConfig const& rhs) const
    {
      return m_id != rhs.m_id || m_isVisible != rhs.m_isVisible ||
        m_description != rhs.m_description;
    }

    void SetId(std::string const& id)
      { m_id = id; }

    void SetDescription(std::string const& desc)
      { m_description = desc; }

    void SetIsVisible(bool visible)
      { m_isVisible = visible; }

  private:
    std::string   m_id;
    bool          m_isVisible;
    std::string   m_description;
  };


  class BridgeConfig
  {
  public:
    BridgeConfig();
    virtual ~BridgeConfig();

    QStatus Initialize(std::string const& fileName, bool failIfNotPresent = false);
    QStatus FromFile(std::string const& fileName);
    QStatus ToFile(std::string const& fileName = "");
    QStatus ToString(std::string& out);
    QStatus FindObject(std::string const& id, DsbObjectConfig& obj);
    QStatus AddObject(DsbObjectConfig const& obj);
    QStatus RemoveObject(std::string const& id);

    std::string GetFilename() const
      { return m_fileName; }

    std::string GetBridgeKeyX() const;
    std::string GetDeviceKeyX() const;
    std::string GetDeviceUsername() const;
    std::string GetDevicePassword() const;
    std::string GetDeviceEcdheEcdsaPrivateKey() const;
    std::string GetDeviceEcdheEcdsaCertChain() const;
    bool GetDefaultVisibility() const;

  private:
    QStatus InitXml();
    QStatus AddTextNode(std::string const& nodeName, std::string const& text, void* parentNode);
    QStatus ConvertXsBooleanToBool(std::string const& s, bool& val);

  private:
    std::string m_fileName;

    _xmlDoc* m_doc;
  };
}

