#ifdef __MINGW32__
#ifndef _GLIBCXX_HAS_GTHREADS
#include <mutex>          // std::mutex, std::unique_lock
/**
* @file mingw.mutex.h
* @brief std::mutex et al implementation for MinGW
** (c) 2013-2016 by Mega Limited, Auckland, New Zealand
* @author Alexander Vassilev
*
* @copyright Simplified (2-clause) BSD License.
* You should have received a copy of the license along with this
* program.
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* @note
* This file may become part of the mingw-w64 runtime package. If/when this happens,
* the appropriate license will be added, i.e. this code will become dual-licensed,
* and the current BSD 2-clause license will stay.
*/

#ifndef WIN32STDMUTEX_H
#define WIN32STDMUTEX_H
#ifdef _GLIBCXX_HAS_GTHREADS
#error This version of MinGW seems to include a win32 port of pthreads, and probably    \
    already has C++11 std threading classes implemented, based on pthreads.             \
    You are likely to have class redefinition errors below, and unfirtunately this      \
    implementation can not be used standalone                                           \
    and independent of the system <mutex> header, since it relies on it for             \
    std::unique_lock and other utility classes. If you would still like to use this     \
    implementation (as it is more lightweight), you have to edit the                    \
    c++-config.h system header of your MinGW to not define _GLIBCXX_HAS_GTHREADS.       \
    This will prevent system headers from defining actual threading classes while still \
    defining the necessary utility classes.
#endif
// Recursion checks on non-recursive locks have some performance penalty, so the user
// may want to disable the checks in release builds. In that case, make sure they
// are always enabled in debug builds.

#if defined(STDMUTEX_NO_RECURSION_CHECKS) && !defined(NDEBUG)
    #undef STDMUTEX_NO_RECURSION_CHECKS
#endif

#include <windows.h>
#include <chrono>
#include <system_error>
#include <cstdio>

#ifndef EPROTO
    #define EPROTO 134
#endif
#ifndef EOWNERDEAD
    #define EOWNERDEAD 133
#endif

namespace std
{
class recursive_mutex
{
protected:
    CRITICAL_SECTION mHandle;
public:
    typedef LPCRITICAL_SECTION native_handle_type;
    native_handle_type native_handle() {return &mHandle;}
    recursive_mutex() noexcept
    {
        InitializeCriticalSection(&mHandle);
    }
    recursive_mutex (const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;
    ~recursive_mutex() noexcept
    {
        DeleteCriticalSection(&mHandle);
    }
    void lock()
    {
        EnterCriticalSection(&mHandle);
    }
    void unlock()
    {
        LeaveCriticalSection(&mHandle);
    }
    bool try_lock()
    {
        return (TryEnterCriticalSection(&mHandle)!=0);
    }
};
template <class B>
class _NonRecursive: protected B
{
protected:
    typedef B base;
    DWORD mOwnerThread;
public:
    using typename base::native_handle_type;
    using base::native_handle;
    _NonRecursive() noexcept :base(), mOwnerThread(0) {}
    _NonRecursive (const _NonRecursive<B>&) = delete;
    _NonRecursive& operator= (const _NonRecursive<B>&) = delete;
    void lock()
    {
        base::lock();
        checkSetOwnerAfterLock();
    }
protected:
    void checkSetOwnerAfterLock()
    {
        DWORD self = GetCurrentThreadId();
        if (mOwnerThread == self)
        {
            std::fprintf(stderr, "FATAL: Recursive locking of non-recursive mutex detected. Throwing system exception\n");
            std::fflush(stderr);
            throw std::system_error(EDEADLK, std::generic_category());
        }
        mOwnerThread = self;
    }
    void checkSetOwnerBeforeUnlock()
    {
        DWORD self = GetCurrentThreadId();
        if (mOwnerThread != self)
        {
            std::fprintf(stderr, "FATAL: Recursive unlocking of non-recursive mutex detected. Throwing system exception\n");
            std::fflush(stderr);
            throw std::system_error(EDEADLK, std::generic_category());
        }
        mOwnerThread = 0;
    }
public:
    void unlock()
    {
        checkSetOwnerBeforeUnlock();
        base::unlock();
    }
    bool try_lock()
    {
        bool ret = base::try_lock();
        if (ret)
            checkSetOwnerAfterLock();
        return ret;
    }
};

#ifndef STDMUTEX_NO_RECURSION_CHECKS
    typedef _NonRecursive<recursive_mutex> mutex;
#else
    typedef recursive_mutex mutex;
#endif

class recursive_timed_mutex
{
protected:
    HANDLE mHandle;
public:
    typedef HANDLE native_handle_type;
    native_handle_type native_handle() const {return mHandle;}
    recursive_timed_mutex(const recursive_timed_mutex&) = delete;
    recursive_timed_mutex& operator=(const recursive_timed_mutex&) = delete;
    recursive_timed_mutex(): mHandle(CreateMutex(NULL, FALSE, NULL)){}
    ~recursive_timed_mutex()
    {
        CloseHandle(mHandle);
    }
    void lock()
    {
        DWORD ret = WaitForSingleObject(mHandle, INFINITE);
        if (ret != WAIT_OBJECT_0)
        {
            if (ret == WAIT_ABANDONED)
                throw std::system_error(EOWNERDEAD, std::generic_category());
            else
                throw std::system_error(EPROTO, std::generic_category());
        }
    }
    void unlock()
    {
        if (!ReleaseMutex(mHandle))
            throw std::system_error(EDEADLK, std::generic_category());
    }
    bool try_lock()
    {
        DWORD ret = WaitForSingleObject(mHandle, 0);
        if (ret == WAIT_TIMEOUT)
            return false;
        else if (ret == WAIT_OBJECT_0)
            return true;
        else if (ret == WAIT_ABANDONED)
            throw std::system_error(EOWNERDEAD, std::generic_category());
        else
            throw std::system_error(EPROTO, std::generic_category());
    }
    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep,Period>& dur)
    {
        DWORD timeout = (DWORD)std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

        DWORD ret = WaitForSingleObject(mHandle, timeout);
        if (ret == WAIT_TIMEOUT)
            return false;
        else if (ret == WAIT_OBJECT_0)
            return true;
        else if (ret == WAIT_ABANDONED)
            throw std::system_error(EOWNERDEAD, std::generic_category());
        else
            throw std::system_error(EPROTO, std::generic_category());
    }
    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock,Duration>& timeout_time)
    {
        return try_lock_for(timeout_time - Clock::now());
    }
};

class timed_mutex: public _NonRecursive<recursive_timed_mutex>
{
protected:
    typedef _NonRecursive<recursive_timed_mutex> base;
public:
    using base::base;
    timed_mutex(const timed_mutex&) = delete;
    timed_mutex& operator=(const timed_mutex&) = delete;
    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep,Period>& dur)
    {
        bool ret = base::try_lock_for(dur);
#ifndef STDMUTEX_NO_RECURSION_CHECKS
        if (ret)
            checkSetOwnerAfterLock();
#endif
        return ret;
    }
public:
    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock,Duration>& timeout_time)
    {
        bool ret = base::try_lock_until(timeout_time);
#ifndef STDMUTEX_NO_RECURSION_CHECKS
        if (ret)
            checkSetOwnerAfterLock();
#endif
        return ret;
    }
};
// You can use the scoped locks and other helpers that are still provided by <mutex>
// In that case, you must include <mutex> before including this file, so that this
// file will not try to redefine them
#ifndef _GLIBCXX_MUTEX

/// Do not acquire ownership of the mutex.
struct defer_lock_t { };

 /// Try to acquire ownership of the mutex without blocking.
struct try_to_lock_t { };

 /// Assume the calling thread has already obtained mutex ownership
 /// and manage it.
struct adopt_lock_t { };

constexpr defer_lock_t	defer_lock { };
constexpr try_to_lock_t	try_to_lock { };
constexpr adopt_lock_t	adopt_lock { };

template <class M>
class lock_guard
{
protected:
    M& mMutex;
public:
    typedef M mutex_type;
    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
    explicit lock_guard(mutex_type& m): mMutex(m) { mMutex.lock();  }
    lock_guard(mutex_type& m, adopt_lock_t):mMutex(m){}
    ~lock_guard() {  mMutex.unlock();   }
};

#endif
}
#endif // WIN32STDMUTEX_H

/**
* @file condition_variable.h
* @brief std::condition_variable implementation for MinGW
*
* (c) 2013-2016 by Mega Limited, Auckland, New Zealand
* @author Alexander Vassilev
*
* @copyright Simplified (2-clause) BSD License.
* You should have received a copy of the license along with this
* program.
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* @note
* This file may become part of the mingw-w64 runtime package. If/when this happens,
* the appropriate license will be added, i.e. this code will become dual-licensed,
* and the current BSD 2-clause license will stay.
*/

#ifndef MINGW_CONDITIONAL_VARIABLE_H
#define MINGW_CONDITIONAL_VARIABLE_H
#include <atomic>
#include <assert.h>
#include <chrono>
#include <system_error>
#include <windows.h>

#ifdef _GLIBCXX_HAS_GTHREADS
#error This version of MinGW seems to include a win32 port of pthreads, and probably    \
    already has C++11 std threading classes implemented, based on pthreads.             \
    It is likely that you will get errors about redefined classes, and unfortunately    \
    this implementation can not be used standalone and independent of the system <mutex>\
    header, since it relies on it for                                                   \
    std::unique_lock and other utility classes. If you would still like to use this     \
    implementation (as it is more lightweight), you have to edit the                    \
    c++-config.h system header of your MinGW to not define _GLIBCXX_HAS_GTHREADS.       \
    This will prevent system headers from defining actual threading classes while still \
    defining the necessary utility classes.
#endif

namespace std
{

enum class cv_status { no_timeout, timeout };
class condition_variable_any
{
protected:
    recursive_mutex mMutex;
    atomic<int> mNumWaiters;
    HANDLE mSemaphore;
    HANDLE mWakeEvent;
public:
    typedef HANDLE native_handle_type;
    native_handle_type native_handle() {return mSemaphore;}
    condition_variable_any(const condition_variable_any&) = delete;
    condition_variable_any& operator=(const condition_variable_any&) = delete;
    condition_variable_any()
        :mNumWaiters(0), mSemaphore(CreateSemaphore(NULL, 0, 0xFFFF, NULL)),
         mWakeEvent(CreateEvent(NULL, FALSE, FALSE, NULL))
    {}
    ~condition_variable_any() {  CloseHandle(mWakeEvent); CloseHandle(mSemaphore);  }
protected:
    template <class M>
    bool wait_impl(M& lock, DWORD timeout)
    {
        {
            lock_guard<recursive_mutex> guard(mMutex);
            mNumWaiters++;
        }
        lock.unlock();
            DWORD ret = WaitForSingleObject(mSemaphore, timeout);

        mNumWaiters--;
        SetEvent(mWakeEvent);
        lock.lock();
        if (ret == WAIT_OBJECT_0)
            return true;
        else if (ret == WAIT_TIMEOUT)
            return false;
//2 possible cases:
//1)The point in notify_all() where we determine the count to
//increment the semaphore with has not been reached yet:
//we just need to decrement mNumWaiters, but setting the event does not hurt
//
//2)Semaphore has just been released with mNumWaiters just before
//we decremented it. This means that the semaphore count
//after all waiters finish won't be 0 - because not all waiters
//woke up by acquiring the semaphore - we woke up by a timeout.
//The notify_all() must handle this grafecully
//
        else
            throw std::system_error(EPROTO, std::generic_category());
    }
public:
    template <class M>
    void wait(M& lock)
    {
        wait_impl(lock, INFINITE);
    }
    template <class M, class Predicate>
    void wait(M& lock, Predicate pred)
    {
        while(!pred())
        {
            wait(lock);
        };
    }

    void notify_all() noexcept
    {
        lock_guard<recursive_mutex> lock(mMutex); //block any further wait requests until all current waiters are unblocked
        if (mNumWaiters.load() <= 0)
            return;

        ReleaseSemaphore(mSemaphore, mNumWaiters, NULL);
        while(mNumWaiters > 0)
        {
            auto ret = WaitForSingleObject(mWakeEvent, 1000);
            if (ret == WAIT_FAILED || ret == WAIT_ABANDONED)
                std::terminate();
        }
        assert(mNumWaiters == 0);
//in case some of the waiters timed out just after we released the
//semaphore by mNumWaiters, it won't be zero now, because not all waiters
//woke up by acquiring the semaphore. So we must zero the semaphore before
//we accept waiters for the next event
//See _wait_impl for details
        while(WaitForSingleObject(mSemaphore, 0) == WAIT_OBJECT_0);
    }
    void notify_one() noexcept
    {
        lock_guard<recursive_mutex> lock(mMutex);
        int targetWaiters = mNumWaiters.load() - 1;
        if (targetWaiters <= -1)
            return;
        ReleaseSemaphore(mSemaphore, 1, NULL);
        while(mNumWaiters > targetWaiters)
        {
            auto ret = WaitForSingleObject(mWakeEvent, 1000);
            if (ret == WAIT_FAILED || ret == WAIT_ABANDONED)
                std::terminate();
        }
        assert(mNumWaiters == targetWaiters);
    }
    template <class M, class Rep, class Period>
    std::cv_status wait_for(M& lock,
      const std::chrono::duration<Rep, Period>& rel_time)
    {
        long long timeout = chrono::duration_cast<chrono::milliseconds>(rel_time).count();
        if (timeout < 0)
            timeout = 0;
        bool ret = wait_impl(lock, (DWORD)timeout);
        return ret?cv_status::no_timeout:cv_status::timeout;
    }

    template <class M, class Rep, class Period, class Predicate>
    bool wait_for(M& lock,
       const std::chrono::duration<Rep, Period>& rel_time, Predicate pred)
    {
        wait_for(lock, rel_time);
        return pred();
    }
    template <class M, class Clock, class Duration>
    cv_status wait_until (M& lock,
      const chrono::time_point<Clock,Duration>& abs_time)
    {
        return wait_for(lock, abs_time - Clock::now());
    }
    template <class M, class Clock, class Duration, class Predicate>
    bool wait_until (M& lock,
      const std::chrono::time_point<Clock, Duration>& abs_time,
      Predicate pred)
    {
        auto time = abs_time - Clock::now();
        if (time < 0)
            return pred();
        else
            return wait_for(lock, time, pred);
    }
};
class condition_variable: protected condition_variable_any
{
protected:
    typedef condition_variable_any base;
public:
    using base::native_handle_type;
    using base::native_handle;
    using base::base;
    using base::notify_all;
    using base::notify_one;
    void wait(unique_lock<mutex> &lock)
    {       base::wait(lock);                               }
    template <class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred)
    {       base::wait(lock, pred);                         }
    template <class Rep, class Period>
    std::cv_status wait_for(unique_lock<mutex>& lock, const std::chrono::duration<Rep, Period>& rel_time)
    {      return base::wait_for(lock, rel_time);           }
    template <class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock, const std::chrono::duration<Rep, Period>& rel_time, Predicate pred)
    {        return base::wait_for(lock, rel_time, pred);   }
    template <class Clock, class Duration>
    cv_status wait_until (unique_lock<mutex>& lock, const chrono::time_point<Clock,Duration>& abs_time)
    {        return base::wait_until(lock, abs_time);         }
    template <class Clock, class Duration, class Predicate>
    bool wait_until (unique_lock<mutex>& lock, const std::chrono::time_point<Clock, Duration>& abs_time, Predicate pred)
    {        return base::wait_until(lock, abs_time, pred); }
};
}
#endif // MINGW_CONDITIONAL_VARIABLE_H

/**
* @file mingw.thread.h
* @brief std::thread implementation for MinGW
* (c) 2013-2016 by Mega Limited, Auckland, New Zealand
* @author Alexander Vassilev
*
* @copyright Simplified (2-clause) BSD License.
* You should have received a copy of the license along with this
* program.
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* @note
* This file may become part of the mingw-w64 runtime package. If/when this happens,
* the appropriate license will be added, i.e. this code will become dual-licensed,
* and the current BSD 2-clause license will stay.
*/

#ifndef WIN32STDTHREAD_H
#define WIN32STDTHREAD_H

#include <windows.h>
#include <functional>
#include <memory>
#include <chrono>
#include <system_error>
#include <cerrno>
#include <process.h>

#ifdef _GLIBCXX_HAS_GTHREADS
#error This version of MinGW seems to include a win32 port of pthreads, and probably    \
    already has C++11 std threading classes implemented, based on pthreads.             \
    It is likely that you will get class redefinition errors below, and unfortunately   \
    this implementation can not be used standalone                                      \
    and independent of the system <mutex> header, since it relies on it for             \
    std::unique_lock and other utility classes. If you would still like to use this     \
    implementation (as it is more lightweight), you have to edit the                    \
    c++-config.h system header of your MinGW to not define _GLIBCXX_HAS_GTHREADS.       \
    This will prevent system headers from defining actual threading classes while still \
    defining the necessary utility classes.
#endif

//instead of INVALID_HANDLE_VALUE _beginthreadex returns 0
#define _STD_THREAD_INVALID_HANDLE 0
namespace std
{

class thread
{
public:
    class id
    {
        DWORD mId;
        void clear() {mId = 0;}
        friend class thread;
    public:
        explicit id(DWORD aId=0):mId(aId){}
        bool operator==(const id& other) const {return mId == other.mId;}
    };
protected:
    HANDLE mHandle;
    id mThreadId;
public:
    typedef HANDLE native_handle_type;
    id get_id() const noexcept {return mThreadId;}
    native_handle_type native_handle() const {return mHandle;}
    thread(): mHandle(_STD_THREAD_INVALID_HANDLE){}

    thread(thread&& other)
    :mHandle(other.mHandle), mThreadId(other.mThreadId)
    {
        other.mHandle = _STD_THREAD_INVALID_HANDLE;
        other.mThreadId.clear();
    }

    thread(const thread &other)=delete;

    template<class Function, class... Args>
    explicit thread(Function&& f, Args&&... args)
    {
        typedef decltype(std::bind(f, args...)) Call;
        Call* call = new Call(std::bind(f, args...));
        mHandle = (HANDLE)_beginthreadex(NULL, 0, threadfunc<Call>,
            (LPVOID)call, 0, (unsigned*)&(mThreadId.mId));
        if (mHandle == _STD_THREAD_INVALID_HANDLE)
        {
            int errnum = errno;
            delete call;
            throw std::system_error(errnum, std::generic_category());
        }
    }
    template <class Call>
    static unsigned int __stdcall threadfunc(void* arg)
    {
        std::unique_ptr<Call> upCall(static_cast<Call*>(arg));
        (*upCall)();
        return (unsigned long)0;
    }
    bool joinable() const {return mHandle != _STD_THREAD_INVALID_HANDLE;}
    void join()
    {
        if (get_id() == id(GetCurrentThreadId()))
            throw std::system_error(EDEADLK, std::generic_category());
        if (mHandle == _STD_THREAD_INVALID_HANDLE)
            throw std::system_error(ESRCH, std::generic_category());
        if (!joinable())
            throw std::system_error(EINVAL, std::generic_category());
        WaitForSingleObject(mHandle, INFINITE);
        CloseHandle(mHandle);
        mHandle = _STD_THREAD_INVALID_HANDLE;
        mThreadId.clear();
    }

    ~thread()
    {
        if (joinable())
            std::terminate();
    }
    thread& operator=(const thread&) = delete;
    thread& operator=(thread&& other) noexcept
    {
        if (joinable())
          std::terminate();
        swap(std::forward<thread>(other));
        return *this;
    }
    void swap(thread&& other) noexcept
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mThreadId.mId, other.mThreadId.mId);
    }
    static unsigned int hardware_concurrency() noexcept
    {
        static int ncpus = -1;
        if (ncpus == -1)
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            ncpus = sysinfo.dwNumberOfProcessors;
        }
        return ncpus;
    }
    void detach()
    {
        if (!joinable())
            throw std::system_error(EINVAL, std::generic_category());
        if (mHandle != _STD_THREAD_INVALID_HANDLE)
        {
            CloseHandle(mHandle);
            mHandle = _STD_THREAD_INVALID_HANDLE;
        }
        mThreadId.clear();
    }
};

namespace this_thread
{
    inline thread::id get_id() {return thread::id(GetCurrentThreadId());}
    inline void yield() {Sleep(0);}
    template< class Rep, class Period >
    void sleep_for( const std::chrono::duration<Rep,Period>& sleep_duration)
    {
        Sleep(std::chrono::duration_cast<std::chrono::milliseconds>(sleep_duration).count());
    }
    template <class Clock, class Duration>
    void sleep_until(const std::chrono::time_point<Clock,Duration>& sleep_time)
    {
        sleep_for(sleep_time-Clock::now());
    }
}

}
#endif // WIN32STDTHREAD_H

#endif // _GLIBCXX_HAS_GTHREADS
#endif // __MINGW32__


//--------------------------------------------
//--------------------------------------------

/* Redis uses the CRC64 variant with "Jones" coefficients and init value of 0.
 *
 * Specification of this CRC64 variant follows:
 * Name: crc-64-jones
 * Width: 64 bites
 * Poly: 0xad93d23594c935a9
 * Reflected In: True
 * Xor_In: 0xffffffffffffffff
 * Reflected_Out: True
 * Xor_Out: 0x0
 * Check("123456789"): 0xe9c6d914c4b8d9ca
 *
 * Copyright (c) 2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

#ifndef CRC64_H
#define CRC64_H

#include <stdint.h>
#include <iostream>

static const uint64_t crc64_tab[256] = { UINT64_C(0x0000000000000000), UINT64_C(
		0x7ad870c830358979), UINT64_C(0xf5b0e190606b12f2), UINT64_C(
		0x8f689158505e9b8b), UINT64_C(0xc038e5739841b68f), UINT64_C(
		0xbae095bba8743ff6), UINT64_C(0x358804e3f82aa47d), UINT64_C(
		0x4f50742bc81f2d04), UINT64_C(0xab28ecb46814fe75), UINT64_C(
		0xd1f09c7c5821770c), UINT64_C(0x5e980d24087fec87), UINT64_C(
		0x24407dec384a65fe), UINT64_C(0x6b1009c7f05548fa), UINT64_C(
		0x11c8790fc060c183), UINT64_C(0x9ea0e857903e5a08), UINT64_C(
		0xe478989fa00bd371), UINT64_C(0x7d08ff3b88be6f81), UINT64_C(
		0x07d08ff3b88be6f8), UINT64_C(0x88b81eabe8d57d73), UINT64_C(
		0xf2606e63d8e0f40a), UINT64_C(0xbd301a4810ffd90e), UINT64_C(
		0xc7e86a8020ca5077), UINT64_C(0x4880fbd87094cbfc), UINT64_C(
		0x32588b1040a14285), UINT64_C(0xd620138fe0aa91f4), UINT64_C(
		0xacf86347d09f188d), UINT64_C(0x2390f21f80c18306), UINT64_C(
		0x594882d7b0f40a7f), UINT64_C(0x1618f6fc78eb277b), UINT64_C(
		0x6cc0863448deae02), UINT64_C(0xe3a8176c18803589), UINT64_C(
		0x997067a428b5bcf0), UINT64_C(0xfa11fe77117cdf02), UINT64_C(
		0x80c98ebf2149567b), UINT64_C(0x0fa11fe77117cdf0), UINT64_C(
		0x75796f2f41224489), UINT64_C(0x3a291b04893d698d), UINT64_C(
		0x40f16bccb908e0f4), UINT64_C(0xcf99fa94e9567b7f), UINT64_C(
		0xb5418a5cd963f206), UINT64_C(0x513912c379682177), UINT64_C(
		0x2be1620b495da80e), UINT64_C(0xa489f35319033385), UINT64_C(
		0xde51839b2936bafc), UINT64_C(0x9101f7b0e12997f8), UINT64_C(
		0xebd98778d11c1e81), UINT64_C(0x64b116208142850a), UINT64_C(
		0x1e6966e8b1770c73), UINT64_C(0x8719014c99c2b083), UINT64_C(
		0xfdc17184a9f739fa), UINT64_C(0x72a9e0dcf9a9a271), UINT64_C(
		0x08719014c99c2b08), UINT64_C(0x4721e43f0183060c), UINT64_C(
		0x3df994f731b68f75), UINT64_C(0xb29105af61e814fe), UINT64_C(
		0xc849756751dd9d87), UINT64_C(0x2c31edf8f1d64ef6), UINT64_C(
		0x56e99d30c1e3c78f), UINT64_C(0xd9810c6891bd5c04), UINT64_C(
		0xa3597ca0a188d57d), UINT64_C(0xec09088b6997f879), UINT64_C(
		0x96d1784359a27100), UINT64_C(0x19b9e91b09fcea8b), UINT64_C(
		0x636199d339c963f2), UINT64_C(0xdf7adabd7a6e2d6f), UINT64_C(
		0xa5a2aa754a5ba416), UINT64_C(0x2aca3b2d1a053f9d), UINT64_C(
		0x50124be52a30b6e4), UINT64_C(0x1f423fcee22f9be0), UINT64_C(
		0x659a4f06d21a1299), UINT64_C(0xeaf2de5e82448912), UINT64_C(
		0x902aae96b271006b), UINT64_C(0x74523609127ad31a), UINT64_C(
		0x0e8a46c1224f5a63), UINT64_C(0x81e2d7997211c1e8), UINT64_C(
		0xfb3aa75142244891), UINT64_C(0xb46ad37a8a3b6595), UINT64_C(
		0xceb2a3b2ba0eecec), UINT64_C(0x41da32eaea507767), UINT64_C(
		0x3b024222da65fe1e), UINT64_C(0xa2722586f2d042ee), UINT64_C(
		0xd8aa554ec2e5cb97), UINT64_C(0x57c2c41692bb501c), UINT64_C(
		0x2d1ab4dea28ed965), UINT64_C(0x624ac0f56a91f461), UINT64_C(
		0x1892b03d5aa47d18), UINT64_C(0x97fa21650afae693), UINT64_C(
		0xed2251ad3acf6fea), UINT64_C(0x095ac9329ac4bc9b), UINT64_C(
		0x7382b9faaaf135e2), UINT64_C(0xfcea28a2faafae69), UINT64_C(
		0x8632586aca9a2710), UINT64_C(0xc9622c4102850a14), UINT64_C(
		0xb3ba5c8932b0836d), UINT64_C(0x3cd2cdd162ee18e6), UINT64_C(
		0x460abd1952db919f), UINT64_C(0x256b24ca6b12f26d), UINT64_C(
		0x5fb354025b277b14), UINT64_C(0xd0dbc55a0b79e09f), UINT64_C(
		0xaa03b5923b4c69e6), UINT64_C(0xe553c1b9f35344e2), UINT64_C(
		0x9f8bb171c366cd9b), UINT64_C(0x10e3202993385610), UINT64_C(
		0x6a3b50e1a30ddf69), UINT64_C(0x8e43c87e03060c18), UINT64_C(
		0xf49bb8b633338561), UINT64_C(0x7bf329ee636d1eea), UINT64_C(
		0x012b592653589793), UINT64_C(0x4e7b2d0d9b47ba97), UINT64_C(
		0x34a35dc5ab7233ee), UINT64_C(0xbbcbcc9dfb2ca865), UINT64_C(
		0xc113bc55cb19211c), UINT64_C(0x5863dbf1e3ac9dec), UINT64_C(
		0x22bbab39d3991495), UINT64_C(0xadd33a6183c78f1e), UINT64_C(
		0xd70b4aa9b3f20667), UINT64_C(0x985b3e827bed2b63), UINT64_C(
		0xe2834e4a4bd8a21a), UINT64_C(0x6debdf121b863991), UINT64_C(
		0x1733afda2bb3b0e8), UINT64_C(0xf34b37458bb86399), UINT64_C(
		0x8993478dbb8deae0), UINT64_C(0x06fbd6d5ebd3716b), UINT64_C(
		0x7c23a61ddbe6f812), UINT64_C(0x3373d23613f9d516), UINT64_C(
		0x49aba2fe23cc5c6f), UINT64_C(0xc6c333a67392c7e4), UINT64_C(
		0xbc1b436e43a74e9d), UINT64_C(0x95ac9329ac4bc9b5), UINT64_C(
		0xef74e3e19c7e40cc), UINT64_C(0x601c72b9cc20db47), UINT64_C(
		0x1ac40271fc15523e), UINT64_C(0x5594765a340a7f3a), UINT64_C(
		0x2f4c0692043ff643), UINT64_C(0xa02497ca54616dc8), UINT64_C(
		0xdafce7026454e4b1), UINT64_C(0x3e847f9dc45f37c0), UINT64_C(
		0x445c0f55f46abeb9), UINT64_C(0xcb349e0da4342532), UINT64_C(
		0xb1eceec59401ac4b), UINT64_C(0xfebc9aee5c1e814f), UINT64_C(
		0x8464ea266c2b0836), UINT64_C(0x0b0c7b7e3c7593bd), UINT64_C(
		0x71d40bb60c401ac4), UINT64_C(0xe8a46c1224f5a634), UINT64_C(
		0x927c1cda14c02f4d), UINT64_C(0x1d148d82449eb4c6), UINT64_C(
		0x67ccfd4a74ab3dbf), UINT64_C(0x289c8961bcb410bb), UINT64_C(
		0x5244f9a98c8199c2), UINT64_C(0xdd2c68f1dcdf0249), UINT64_C(
		0xa7f41839ecea8b30), UINT64_C(0x438c80a64ce15841), UINT64_C(
		0x3954f06e7cd4d138), UINT64_C(0xb63c61362c8a4ab3), UINT64_C(
		0xcce411fe1cbfc3ca), UINT64_C(0x83b465d5d4a0eece), UINT64_C(
		0xf96c151de49567b7), UINT64_C(0x76048445b4cbfc3c), UINT64_C(
		0x0cdcf48d84fe7545), UINT64_C(0x6fbd6d5ebd3716b7), UINT64_C(
		0x15651d968d029fce), UINT64_C(0x9a0d8ccedd5c0445), UINT64_C(
		0xe0d5fc06ed698d3c), UINT64_C(0xaf85882d2576a038), UINT64_C(
		0xd55df8e515432941), UINT64_C(0x5a3569bd451db2ca), UINT64_C(
		0x20ed197575283bb3), UINT64_C(0xc49581ead523e8c2), UINT64_C(
		0xbe4df122e51661bb), UINT64_C(0x3125607ab548fa30), UINT64_C(
		0x4bfd10b2857d7349), UINT64_C(0x04ad64994d625e4d), UINT64_C(
		0x7e7514517d57d734), UINT64_C(0xf11d85092d094cbf), UINT64_C(
		0x8bc5f5c11d3cc5c6), UINT64_C(0x12b5926535897936), UINT64_C(
		0x686de2ad05bcf04f), UINT64_C(0xe70573f555e26bc4), UINT64_C(
		0x9ddd033d65d7e2bd), UINT64_C(0xd28d7716adc8cfb9), UINT64_C(
		0xa85507de9dfd46c0), UINT64_C(0x273d9686cda3dd4b), UINT64_C(
		0x5de5e64efd965432), UINT64_C(0xb99d7ed15d9d8743), UINT64_C(
		0xc3450e196da80e3a), UINT64_C(0x4c2d9f413df695b1), UINT64_C(
		0x36f5ef890dc31cc8), UINT64_C(0x79a59ba2c5dc31cc), UINT64_C(
		0x037deb6af5e9b8b5), UINT64_C(0x8c157a32a5b7233e), UINT64_C(
		0xf6cd0afa9582aa47), UINT64_C(0x4ad64994d625e4da), UINT64_C(
		0x300e395ce6106da3), UINT64_C(0xbf66a804b64ef628), UINT64_C(
		0xc5bed8cc867b7f51), UINT64_C(0x8aeeace74e645255), UINT64_C(
		0xf036dc2f7e51db2c), UINT64_C(0x7f5e4d772e0f40a7), UINT64_C(
		0x05863dbf1e3ac9de), UINT64_C(0xe1fea520be311aaf), UINT64_C(
		0x9b26d5e88e0493d6), UINT64_C(0x144e44b0de5a085d), UINT64_C(
		0x6e963478ee6f8124), UINT64_C(0x21c640532670ac20), UINT64_C(
		0x5b1e309b16452559), UINT64_C(0xd476a1c3461bbed2), UINT64_C(
		0xaeaed10b762e37ab), UINT64_C(0x37deb6af5e9b8b5b), UINT64_C(
		0x4d06c6676eae0222), UINT64_C(0xc26e573f3ef099a9), UINT64_C(
		0xb8b627f70ec510d0), UINT64_C(0xf7e653dcc6da3dd4), UINT64_C(
		0x8d3e2314f6efb4ad), UINT64_C(0x0256b24ca6b12f26), UINT64_C(
		0x788ec2849684a65f), UINT64_C(0x9cf65a1b368f752e), UINT64_C(
		0xe62e2ad306bafc57), UINT64_C(0x6946bb8b56e467dc), UINT64_C(
		0x139ecb4366d1eea5), UINT64_C(0x5ccebf68aecec3a1), UINT64_C(
		0x2616cfa09efb4ad8), UINT64_C(0xa97e5ef8cea5d153), UINT64_C(
		0xd3a62e30fe90582a), UINT64_C(0xb0c7b7e3c7593bd8), UINT64_C(
		0xca1fc72bf76cb2a1), UINT64_C(0x45775673a732292a), UINT64_C(
		0x3faf26bb9707a053), UINT64_C(0x70ff52905f188d57), UINT64_C(
		0x0a2722586f2d042e), UINT64_C(0x854fb3003f739fa5), UINT64_C(
		0xff97c3c80f4616dc), UINT64_C(0x1bef5b57af4dc5ad), UINT64_C(
		0x61372b9f9f784cd4), UINT64_C(0xee5fbac7cf26d75f), UINT64_C(
		0x9487ca0fff135e26), UINT64_C(0xdbd7be24370c7322), UINT64_C(
		0xa10fceec0739fa5b), UINT64_C(0x2e675fb4576761d0), UINT64_C(
		0x54bf2f7c6752e8a9), UINT64_C(0xcdcf48d84fe75459), UINT64_C(
		0xb71738107fd2dd20), UINT64_C(0x387fa9482f8c46ab), UINT64_C(
		0x42a7d9801fb9cfd2), UINT64_C(0x0df7adabd7a6e2d6), UINT64_C(
		0x772fdd63e7936baf), UINT64_C(0xf8474c3bb7cdf024), UINT64_C(
		0x829f3cf387f8795d), UINT64_C(0x66e7a46c27f3aa2c), UINT64_C(
		0x1c3fd4a417c62355), UINT64_C(0x935745fc4798b8de), UINT64_C(
		0xe98f353477ad31a7), UINT64_C(0xa6df411fbfb21ca3), UINT64_C(
		0xdc0731d78f8795da), UINT64_C(0x536fa08fdfd90e51), UINT64_C(
		0x29b7d047efec8728), };

class hashExt {
public:
	static uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);
};

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * credit to https://github.com/nlohmann
 */
#ifndef NLOHMANN_JSON_FWD_HPP
#define NLOHMANN_JSON_FWD_HPP

#include <cstdint> // int64_t, uint64_t
#include <map> // map
#include <memory> // allocator
#include <string> // string
#include <vector> // vector

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
namespace nlohmann
{
/*!
@brief default JSONSerializer template argument
This serializer ignores the template arguments and uses ADL
([argument-dependent lookup](http://en.cppreference.com/w/cpp/language/adl))
for serialization.
*/
template<typename = void, typename = void>
struct adl_serializer;

template<template<typename U, typename V, typename... Args> class ObjectType =
         std::map,
         template<typename U, typename... Args> class ArrayType = std::vector,
         class StringType = std::string, class BooleanType = bool,
         class NumberIntegerType = std::int64_t,
         class NumberUnsignedType = std::uint64_t,
         class NumberFloatType = double,
         template<typename U> class AllocatorType = std::allocator,
         template<typename T, typename SFINAE = void> class JSONSerializer =
         adl_serializer>
class basic_json;

/*!
@brief JSON Pointer
A JSON pointer defines a string syntax for identifying a specific value
within a JSON document. It can be used with functions `at` and
`operator[]`. Furthermore, JSON pointers are the base for JSON patches.
@sa [RFC 6901](https://tools.ietf.org/html/rfc6901)
@since version 2.0.0
*/
template<typename BasicJsonType>
class json_pointer;

/*!
@brief default JSON class
This type is the default specialization of the @ref basic_json class which
uses the standard template types.
@since version 1.0.0
*/
using json = basic_json<>;
}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_ASSET_H
#define RFU_ASSET_H

#include <iostream>
#include <string>
#include <chrono>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>

using namespace boost;

namespace ResourceFileUtility {

class Asset {
private:
	bool fileExist, fileWritten, fileProcessing;
	unsigned long long filePosCurrent, filePosNew, fileLenCurrent, fileLenNew,
			processBytes, fileBytes, fileReadBytesLast;
	std::chrono::microseconds processTime, fileReadTimeLast,
			fileReadTimePerByteLast;
	std::string handle, inType, outType;
	filesystem::path filePath;
	uint64_t crc64;
public:
	Asset();
	Asset(std::string handle_, filesystem::path filePath_, std::string inType_,
			std::string outType_);
	void init();
	std::string getHandle();
	filesystem::path getFilePath();
	std::string getInType();
	std::string getOutType();
	void setExist(bool flag);
	void setProcess(bool flag);
	bool getProcess();
	unsigned long long getProcessBytes();
	unsigned long long getFileBytes();
	void setProcessBytes(unsigned long long val);
	void setFileBytes(unsigned long long val);
	void setProcessTime(std::chrono::microseconds val);
	uint64_t getCRC64();
	void setCRC64(uint64_t val);
};

}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <iostream>
#include <string>
#include <fstream>

namespace ResourceFileUtility {

class CallbackHandler {
public:
	CallbackHandler() {

	}
	virtual ~CallbackHandler() {
	}
	virtual int fileComplete(char* filePath);
	virtual int packComplete(char* filePath);
	virtual int estimateFileComplete(int fileID);
};

}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_PARSER_H
#define RFU_PARSER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>





using namespace boost;

namespace ResourceFileUtility {

class Asset;
class Directory;
class ResourceFile;

class Parser {
public:
	Parser() {

	}
	virtual ~Parser() {
	}
	static int readDirectoryJSON(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int readDirectory(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int getSize(ResourceFile& directoryObj);
	static int getSize(Asset* assetPtr);
	static int estimate(Asset* assetPtr);
	static int writeDirectory(filesystem::fstream& resourceFile,
			ResourceFile& directoryObj);
	static int insertAsset(filesystem::fstream& resourceFile, Asset& assetObj);
	static int removeAsset(filesystem::fstream& resourceFile, Asset& assetObj);
	static unsigned char* ullToBytes(unsigned long long val);
	static char* ullToBytesSigned(unsigned long long val);
	static char* ullToBytesSigned(unsigned long long val,
			unsigned long long size);
	static unsigned long long bytesToUll(unsigned char* val);
	static unsigned long long bytesToUll(char* val);
	static void ofstreamWrite(nowide::ofstream& outStream,
			unsigned long long val, unsigned long long size);
	static int assetListToDirectory(std::vector<Asset*>& assetList,
			Directory& directory);
};

}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef RFU_RESOURCE_FILE_H
#define RFU_RESOURCE_FILE_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>





using namespace boost;

namespace ResourceFileUtility {

/*
 * @class Directory
 * A pre-processed resource file directory.\n
 * Each Directory entry takes up the bytes
 * 8 bytes = file CRC64
 * 8 bytes = file start position (byte)
 * 8 bytes = file length (byte)
 * 8 bytes = insert time
 * 8 bytes = file type (8 digit ascii)
 * 32 bytes = file handle (32 digit ascii)
 * 58 bytes = custom
 * 128 bytes total
 */
class Directory {
private:
	class Entry {
	public:
		uint64_t CRC64, assetPosition // relative offset from file data start position
				, assetLength, assetInsertTime;
		char type[8], handle[32];
		Asset* assetPtr;
	};
	std::vector<Entry*> entryList;
	std::vector<std::pair<uint64_t,uint64_t>*> spaceList;
	uint64_t spaceLast, offsetPosition;
	unsigned int entryReserve;
public:
	Directory();
	unsigned char* toBytes(unsigned int& size);
	int addFromAsset(Asset& assetObject);
	uint64_t findSpace(uint64_t length);
	void setOffsetPosition(uint64_t pos);
	void setEntryReserve(unsigned int val);
};

/**
 * @class ResourceFile
 * Resource File Meta
 * 8 bytes = file version
 * 8 bytes = file compatibility version
 * 8 bytes = last write time (seconds since unix epoch)
 * 8 bytes = directory start position (byte)
 * 8 bytes = directory length (byte)
 * 8 bytes = directory CRC64
 * 8 bytes = data start position (byte)
 * 8 bytes = data length (byte)
 * 8 bytes = data CRC64
 * 72 bytes = reserved
 * 128 bytes total
 **/
class ResourceFile {
private:
	Directory directory;
	std::vector<Asset*> assetList;
	unsigned long long versionStatic, compatibilityVersionStatic, version,
			compatibilityVersion // equal compatibility versions can be read/written
			, directoryStartByte, DataStartByte;
	std::chrono::time_point<std::chrono::system_clock> writeTimeLast;
	filesystem::path directoryPath;
	std::unordered_map<char*, Asset*> hashTable;
	unsigned int metaSize, directoryEntryReserve, directoryEntrySize;
public:
	ResourceFile();
	virtual ~ResourceFile() {
	}
	void addFile(std::string handle, std::string filePathString,
			std::string inType, std::string outType);
	unsigned int assetListSize();
	Asset* asset(unsigned int assetID);
	unsigned long long getProcessingBytesTotal();
	unsigned long long getProcessingBytes();
	unsigned long long getSizeTotal();
	unsigned long long getVersion();
	unsigned long long getCompatibilityVersion();
	std::string infoToString();
	std::string estimateToString();
	void setDirectory(filesystem::path path);
	unsigned int open(std::string resourceFileName);
	unsigned int open(std::wstring resourceFileName);
	unsigned int open(filesystem::path resourceFilePath);
	unsigned int write(std::string resourceFileName);
	unsigned int write(std::wstring resourceFileName);
	unsigned int write(filesystem::path resourceFilePath);
	int buildDirectory();
};

}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef COMPILER_H
#define COMPILER_H

//#define __cplusplus 201103L
//#define _GLIBCXX_USE_C99 1
//#define _GLIBCXX_USE_WCHAR_T 1
//#define _GLIBCXX_HAVE_WCSTOF 1
//#undef _GLIBCXX_HAVE_BROKEN_VSWPRINTF
//#define _GLIBCXX_HAVE_WCHAR_H 1

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>





//

using namespace boost;

namespace ResourceFileUtility {

class ResourceFile;
class Asset;
typedef int (*CBintString)(char* text);
typedef void (*CBvoidResourceFile)(ResourceFile* resourceFilePtr);

class Compiler {
private:
	ResourceFile resourceFileObj;
	CBintString callbackFileComplete;
	CBintString callbackPackComplete;
	ResourceFileUtility::CallbackHandler* callbackHandlerPtr;
	std::vector<thread*> estimateThreadList;
	int metaDirectorySize;
	/* reads current resource file directory and attempts to append new files and delete old ones.
	 * An entire recompile will be triggered if new directory size > old directory size */
	bool tryToUpdate;
public:
	Compiler();
	virtual ~Compiler() {
	}
	void info(std::string fileName);
	void estimate();
	void estimate(CBvoidResourceFile handler_);
	unsigned int pack(std::string resourceFileName);
	unsigned int pack(std::string resourceFileName,
			CBvoidResourceFile handler_);
	Asset resourceFileGetFile(int fileID);
	void setCallbackFileComplete(CBintString handler_);
	void setCallbackPackComplete(CBintString handler_);
	void setCallback(CallbackHandler* handler_);
	ResourceFile* getResourceFile();
};

}

#endif


//--------------------------------------------
//--------------------------------------------

/*
 * ResourceFileUtility
 * By: Brad Lee
 */
#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <string>




namespace ResourceFileUtility {

class ResourceFile;
class Asset;
class Loader {
private:
	ResourceFile resourceFileObj;
public:
	Loader();
	virtual ~Loader() {
	}
	/*
	 * Opens the resource file and populates the Resource File Object
	 */
	unsigned int data(std::string resourceFileName);
	Asset* info(std::string assetHandle);
	/*
	 * Opens the resource file asset byte array
	 * @return asset byte array
	 */
	unsigned char* open(std::string assetHandle);
};

}

#endif


//--------------------------------------------
//--------------------------------------------

#ifndef STREAM_H
#define STREAM_H

#include <iostream>
#include <string>
#include <fstream>

namespace ResourceFileUtility {

class Stream {
public:
	Stream();
	~Stream();
};

}

#endif


//--------------------------------------------
//--------------------------------------------

