#pragma once

#ifdef __APPLE__
#include <memory>
using std::shared_ptr;
using std::weak_ptr;
using std::enable_shared_from_this;
using std::dynamic_pointer_cast;
#else
#include <tr1/memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::enable_shared_from_this;
using std::tr1::dynamic_pointer_cast;
#endif

#include <assert.h>
#define DSB_ASSERT(X) assert((X))

class AllJoynBusObject;

