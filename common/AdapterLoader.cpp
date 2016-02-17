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
#include "Adapter.h"
#include "AdapterLoader.h"
#include "AdapterLog.h"
#include <dlfcn.h>

namespace
{
  DSB_DECLARE_LOGNAME(AdapterLoader);
}

adapter::Loader::Loader(char const* libname)
  : m_create(nullptr)
  , m_destroy(nullptr)
  , m_libname(libname)
{
}

bool
adapter::Loader::bind()
{
  void* lib = dlopen(m_libname.c_str(), RTLD_LAZY);
  if (!lib)
  {
    DSBLOG_ERROR("failed to load adapter lib '%s'. %s", m_libname.c_str(), dlerror());
    return false;
  }

  m_create = reinterpret_cast<adapter::creator>(dlsym(lib, ALLJOYN_ADAPTER_CREATE_FUNC));
  if (!m_create)
  {
    DSBLOG_ERROR("failed to find '%s' function in %s. %s", ALLJOYN_ADAPTER_CREATE_FUNC,
      m_libname.c_str(), dlerror());
    dlclose(lib);
    return false;
  }

  m_destroy = reinterpret_cast<adapter::destroyer>(dlsym(lib, ALLJOYN_ADAPTER_DESTROY_FUNC));
  if (!m_destroy)
  {
    DSBLOG_ERROR("failed to find '%s function in %s, %s", ALLJOYN_ADAPTER_DESTROY_FUNC,
      m_libname.c_str(), dlerror());
    return false;
  }
  return true;
}

std::shared_ptr<adapter::Adapter>
adapter::Loader::create()
{
  m_adapter.reset(m_create(), m_destroy);
  return m_adapter;
}
