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

#include "AdapterStatus.h"
#include <condition_variable>
#include <mutex>

namespace adapter
{
  class IoRequest
  {
  public:
    typedef std::shared_ptr<IoRequest> Pointer;

  public:
    IoRequest()
      : m_status(0)
      , m_canceled(false)
      , m_completed(false)
    {
    }

    ~IoRequest()
    {
    }

    Status GetStatus()
      { return m_status; }

    bool Wait(uint32_t millis = std::numeric_limits<uint32_t>::max())
    {
      if (millis != std::numeric_limits<uint32_t>::max())
      {
        auto delay = std::chrono::system_clock::now() + std::chrono::milliseconds(millis);

        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_cond.wait_until(lk, delay, [this] { return this->m_canceled || this->m_completed; }))
          return true;
        else
          return false;
      }
      else
      {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_cond.wait(lk, [this] { return this->m_canceled || this->m_completed; });
      }

      return 0;
    }

    void Cancel()
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_canceled = true;
      m_completed = true;
      lk.unlock();
      m_cond.notify_all();
    }

    void SetComplete(Status status)
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_status = status;
      m_canceled = false;
      m_completed = true;
      lk.unlock();
      m_cond.notify_all();
    }

  private:
    Status             m_status;
    bool                      m_canceled;
    bool                      m_completed;
    std::mutex                m_mutex;
    std::condition_variable   m_cond;
  };
}
