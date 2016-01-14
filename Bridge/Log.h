#pragma once

#include "Bridge/IAdapter.h"

namespace bridge
{
  class AdapterLog : public IAdapterLog
  {
  public:
    AdapterLog(std::string const& module);

    virtual void Write(AdapterLogLevel level, char const* file, int line, char const* format, ...);
    virtual bool IsLevelEnabled(AdapterLogLevel level);
  private:
    std::string m_module;
  };
}

