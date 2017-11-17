#pragma once

#include <utility>

// Wrapper class to replicate an object onto all nodelets after it has been constructed
template <typename T>
struct mirrored : public T
{
    // Wrapper constructor to copy-construct T at each nodelet after running the requested constructor
    template<typename... Args>
    explicit mirrored (Args&&... args)
    // Call T's constructor with forwarded args
    : T(std::forward<Args>(args)...)
    {
        // Get pointer to constructed T
        T* local = static_cast<T*>(mw_get_nth(this, 0));
        // Replicate to each remote nodelet
        for (long i = 1; i < NODELETS(); ++i) {
            T * remote = static_cast<T*>(mw_get_nth(this, i));
            // This calls the copy constructor to initialize remote from local
            new(remote) T(*local);
        }
    }

    // Overrides default new to always allocate replicated storage for instances of this class
    static void *
    operator new(std::size_t sz)
    {
        return mw_mallocrepl(sz);
    }

    // Overrides default delete to safely free replicated storage
    static void
    operator delete(void * ptr)
    {
        mw_free(ptr);
    }
};
