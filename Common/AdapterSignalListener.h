#pragma once

#include "Common/AdapterSignal.h"

namespace adapter
{
  typedef std::function<void (Signal const&, void*)> SignalListener;
}

