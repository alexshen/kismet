#ifndef KISMET_UTILITY_ANY_H
#define KISMET_UTILITY_ANY_H

#include <algorithm>
#include <exception>
#include <new>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include "kismet/core/assert.h"

namespace kismet
{

class bad_any_cast : public std::exception
{
    char const* what() const throw() override
    {
        return "bad_any_cast";
    }
};

namespace detail
{
template<typename T>
struct any_cast_impl;
} // namespace detail

class any
{
    // small buffer optimization
    union small_buffer
    {
        void* p;
        double d;
        char alias;
    };

    struct vtable
    {
        void  (* copy)(small_buffer const& source, small_buffer& target);
        void  (* move)(small_buffer& source, small_buffer& target);
        void  (* destroy)(small_buffer& source);
        void* (* get_pointer)(small_buffer& source);
        std::type_info const& (* get_type_info)();
    };

    struct storage
    {
        small_buffer buffer;
        vtable*      vtbl;
    };

    // get buffer pointer depend on the whether small buffer optimization is enabled
    template<bool optimized>
    struct get_buffer_pointer
    {
        static void* get(small_buffer& source)
        {
            return source.p;
        }
    };

    template<typename T>
    struct get_type_info
    {
        static std::type_info const& get()
        {
            return typeid(T);
        }
    };

    // non-optimized manager, storage is on heap
    template<typename T, bool opt>
    struct manager
    {
        enum { optimized = false };

        static void create(small_buffer& target, T const& v)
        {
            target.p = new T(v);
        }

        static void create(small_buffer& target, T&& v)
        {
            target.p = new T(std::move(v));
        }

        static void copy(small_buffer const& source, small_buffer& target)
        {
            target.p = new T(*static_cast<T*>(source.p));
        }

        static void move(small_buffer& source, small_buffer& target)
        {
            target.p = source.p;
            source.p = nullptr;
        }

        static void destroy(small_buffer& source)
        {
            delete static_cast<T*>(get_buffer_pointer<optimized>::get(source));
            source.p = nullptr;
        }
    };

    // optimized manager
    template<typename T>
    struct manager<T, true>
    {
        enum { optimized = true };

        static void create(small_buffer& target, T const& v)
        {
            new (static_cast<void*>(&target.alias)) T(v);
        }

        static void create(small_buffer& target, T&& v)
        {
            new (static_cast<void*>(&target.alias)) T(std::move(v));
        }

        static T& get_object(small_buffer& source)
        {
            return *static_cast<T*>(static_cast<void*>(&source.alias));
        }

        static T const& get_object(small_buffer const& source)
        {
            return *static_cast<T const*>(static_cast<void const*>(&source.alias));
        }

        static void copy(small_buffer const& source, small_buffer& target)
        {
            new (static_cast<void*>(&target.alias)) T(get_object(source));
        }

        static void move(small_buffer& source, small_buffer& target)
        {
            new (static_cast<void*>(&target.alias)) T(std::move(get_object(source)));
        }

        static void destroy(small_buffer& source)
        {
            get_object(source).~T();
        }
    };

    template<typename T>
    struct allow_small_buffer_optimization
    {
        enum
        {
            value = sizeof(T) <= sizeof(small_buffer) &&
                    std::alignment_of<small_buffer>::value % std::alignment_of<T>::value == 0
        };
    };

    template<typename T>
    struct get_manager_type
    {
        using type = manager<T, allow_small_buffer_optimization<T>::value>;
    };

    template<typename T, typename E = void>
    using enable_if_not_any_t = typename std::enable_if<
                                            !std::is_same<
                                                typename std::decay<T>::type
                                                , any>::value
                                            , E>::type;
public:
    any()
    {
        m_storage.vtbl = nullptr;
    }

    any(any const& rhs)
    {
        copy(rhs);
    }

    any(any&& rhs)
    {
        move(std::move(rhs));
    }

    // conversion constructor, only enabled if T is not any&
    template<typename T>
    any(T&& v, enable_if_not_any_t<T>* = 0)
    {
        create(v);
    }

    any& operator =(any const& rhs)
    {
        if (this == &rhs) { return *this; }

        clear();
        copy(rhs);
        return *this;
    }

    any& operator =(any&& rhs)
    {
        if (this == &rhs) { return *this; }

        clear();
        move(std::move(rhs));
        return *this;
    }

    void clear()
    {
        if (m_storage.vtbl)
        {
            m_storage.vtbl->destroy(m_storage.buffer);
            m_storage.vtbl = nullptr;
        }
    }

    template<typename T>
    enable_if_not_any_t<T, any&> operator =(T&& v)
    {
        clear();
        create(std::forward<T>(v));
        return *this;
    }

    ~any()
    {
        clear();
    }

    bool empty() const
    {
        return !m_storage.vtbl;
    }

    void swap(any& rhs)
    {
        std::swap(*this, rhs);
    }

    std::type_info const* type() const
    {
        return m_storage.vtbl ? &m_storage.vtbl->get_type_info() : nullptr;
    }

private:
    void copy(any const& rhs)
    {
        if (rhs.m_storage.vtbl)
        {
            rhs.m_storage.vtbl->copy(rhs.m_storage.buffer, m_storage.buffer);
            m_storage.vtbl = rhs.m_storage.vtbl;
        }
        else
        {
            m_storage.vtbl = nullptr;
        }
    }

    void move(any&& rhs)
    {
        if (rhs.m_storage.vtbl)
        {
            rhs.m_storage.vtbl->move(rhs.m_storage.buffer, m_storage.buffer);
            m_storage.vtbl = rhs.m_storage.vtbl;
            rhs.m_storage.vtbl = nullptr;
        }
        else
        {
            m_storage.vtbl = nullptr;
        }
    }

    template<typename T>
    void create(T&& v)
    {
        using value_type = typename std::decay<T>::type;
        using manager_type = typename get_manager_type<value_type>::type;
        static vtable vtbl = {
            &manager_type::copy,
            &manager_type::move,
            &manager_type::destroy,
            &get_buffer_pointer<manager_type::optimized>::get,
            &get_type_info<value_type>::get
        };

        manager_type::create(m_storage.buffer, std::forward<T>(v));
        m_storage.vtbl = &vtbl;
    }

    void* get_pointer()
    {
        return m_storage.vtbl ? m_storage.vtbl->get_pointer(m_storage.buffer) : nullptr;
    }

    template<typename T>
    friend struct detail::any_cast_impl;

    storage m_storage;
};

// explicit specialization must be in namespace scope
template<>
struct any::get_buffer_pointer<true>
{
    static void* get(small_buffer& source)
    {
        return &source.alias;
    }
};

namespace detail
{

// cast to reference or value type
template<typename T>
struct any_cast_impl
{
    static T unsafe_cast(any& a)
    {
        KISMET_ASSERT(!a.empty());
        using pointer = typename std::add_pointer<
                            typename std::remove_reference<T>::type
                        >::type;
        return static_cast<T>(*static_cast<pointer>(a.get_pointer()));
    }

    static T unsafe_cast(any const& a)
    {
        KISMET_ASSERT(!a.empty());
        using const_pointer = typename std::add_pointer<
                                  typename std::add_const<
                                      typename std::remove_reference<T>::type
                                  >::type
                              >::type;
        return static_cast<T>(
                    *static_cast<const_pointer>(
                        const_cast<void const*>(
                            const_cast<any&>(a).get_pointer())));
    }
};

// cast to pointer type
template<typename T>
struct any_cast_impl<T*>
{
    static T* unsafe_cast(any& a)
    {
        return static_cast<T*>(a.get_pointer());
    }

    static T* unsafe_cast(any const& a)
    {
        return static_cast<T*>(
                    const_cast<void const*>(
                        const_cast<any&>(a).get_pointer()));
    }
};

} // namespace detail

template<typename T>
inline T unsafe_any_cast(any& a)
{
    return detail::any_cast_impl<T>::unsafe_cast(a);
}

template<typename T>
inline T unsafe_any_cast(any const& a)
{
    return detail::any_cast_impl<T>::unsafe_cast(a);
}

template<typename T>
inline T any_cast(any& a)
{
    if (a.type() != &typeid(T))
    {
        throw bad_any_cast();
    }
    return unsafe_any_cast<T>(a);
}

template<typename T>
inline T any_cast(any const& a)
{
    if (a.type() != &typeid(T))
    {
        throw bad_any_cast();
    }
    return unsafe_any_cast<T>(a);
}

inline void swap(any& lhs, any& rhs)
{
    lhs.swap(rhs);
}

} // namespace kismet

#endif // KISMET_UTILITY_ANY_H
