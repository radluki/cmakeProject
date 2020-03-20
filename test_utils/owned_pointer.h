#pragma once

#include <cassert>
#include <exception>
#include <memory>
#include <new>
#include <tuple>
#include <type_traits>

namespace testutils
{

namespace detail
{
template <typename>
class link_ptr;
}

template <typename T>
auto link(const std::unique_ptr<T>& u) noexcept -> detail::link_ptr<T>;

template <typename R, typename T>
auto link(const std::unique_ptr<T>& u) noexcept -> detail::link_ptr<R>;

namespace detail
{
using control_block_type = std::tuple<void*, bool, bool>;

const auto ptr = +[](control_block_type& cb) -> void* { return std::get<0>(cb); };
const auto deleted = +[](control_block_type& cb) -> bool& { return std::get<2>(cb); };
const auto acquired = +[](control_block_type& cb) -> bool& { return std::get<1>(cb); };

template <typename T>
struct owned_deleter
{
    void operator()(control_block_type* const cb)
    {
#ifdef OWNED_POINTER_ASSERT_DTOR
        assert(acquired(*cb) &&
               "ASSERT: you created owned_pointer, but unique_ptr was never acquired");
#endif

        if (!acquired(*cb))
            delete static_cast<T*>(ptr(*cb));

        delete cb;
    }
};

class shared_secret
{
public:
    std::weak_ptr<control_block_type> control_block;
    virtual ~shared_secret() = default;

protected:
    void delete_event() noexcept
    {
        if (auto p{control_block.lock()})
            deleted(*p) = true;
    }
};

template <typename base>
struct destruction_notify_object : base, shared_secret
{
    using base::base;
    ~destruction_notify_object() override
    {
        delete_event();
    }
};

template <typename T>
class link_ptr
{
public:
    template <typename R>
    explicit link_ptr(const std::unique_ptr<R>& u) noexcept : ptr{u.get()}
    {
    }

    link_ptr(const link_ptr&) = delete;
    link_ptr(link_ptr&&) noexcept = default;
    link_ptr& operator=(link_ptr&&) = delete;
    link_ptr& operator=(const link_ptr&) = delete;

    auto get() const noexcept -> T*
    {
        return ptr;
    }

private:
    T* const ptr;
};

} // namespace detail

struct unique_ptr_already_acquired : public std::runtime_error
{
    unique_ptr_already_acquired()
        : std::runtime_error("owned_pointer: This pointer is already acquired by unique_ptr")
    {
    }
};

struct ptr_is_already_deleted : public std::runtime_error
{
    ptr_is_already_deleted() : std::runtime_error("owned_pointer: This pointer is already deleted")
    {
    }
};

template <typename Tp>
class owned_pointer : std::shared_ptr<detail::control_block_type>
{
    static_assert(!std::is_array<Tp>::value && !std::is_pointer<Tp>::value, "no array nor pointer supported");
    using base_type = std::shared_ptr<detail::control_block_type>;

    template <typename>
    friend class owned_pointer;

public:
    using element_type = Tp;
    using base_type::use_count;
    using uptr_type = std::unique_ptr<element_type>;

    constexpr owned_pointer() noexcept = default;
    constexpr owned_pointer(std::nullptr_t) noexcept
    {
    }

    template <typename T>
    owned_pointer(detail::link_ptr<T>&& p) : owned_pointer(p.get(), true)
    {
    }

    template <typename T>
    owned_pointer(std::unique_ptr<T>&& p) : owned_pointer(p.release(), false)
    {
    }

    auto get() const -> element_type*
    {
        throw_when_ptr_expired_and_object_has_virtual_dtor();
        return stored_address();
    }

    auto operator->() const -> element_type*
    {
        return get();
    }

    auto operator*() const -> element_type&
    {
        return *get();
    }

    auto unique_ptr() const -> uptr_type
    {
        if (!get())
            return uptr_type{nullptr};

        if (acquired())
            throw unique_ptr_already_acquired();

        return detail::acquired(base_type::operator*()) = true, uptr_type{stored_address()};
    }

    auto raw_ptr() const -> element_type*
    {
        return unique_ptr().release();
    }

    auto acquired() const noexcept -> bool
    {
        return base_type::operator bool() && detail::acquired(base_type::operator*());
    }

    auto expired() const noexcept -> bool
    {
        return base_type::operator bool() && detail::deleted(base_type::operator*());
    }

    explicit operator uptr_type() const
    {
        return unique_ptr();
    }

    explicit operator bool() const noexcept
    {
        return stored_address();
    }

    template <typename T>
    operator owned_pointer<T>() const noexcept
    {
        static_assert(std::is_convertible<element_type*, T*>::value,
                      "Casting to pointer of different or non-derived type");

        owned_pointer<T> casted{nullptr};
        return (static_cast<base_type&>(casted) = *this, casted);
    }

    template <typename T>
    auto compare(const T& ptr) const noexcept -> std::int8_t
    {
        static_assert(std::is_convertible<T, element_type*>::value ||
                          std::is_convertible<element_type*, T>::value,
                      "Comparing pointer of different or non-derived type");

        const auto addr{stored_address()};
        return addr == ptr ? 0 : (reinterpret_cast<std::ptrdiff_t>(addr) < reinterpret_cast<std::ptrdiff_t>(ptr) ? -1 : +1);
    }

    template <typename T>
    auto compare(const owned_pointer<T>& p) const noexcept -> std::int8_t
    {
        return compare(p.stored_address());
    }

    auto get(std::nothrow_t) const noexcept -> element_type*
    {
        return !expired() ? stored_address() : nullptr;
    }

private:
    auto stored_address() const noexcept -> element_type*
    {
        return base_type::operator bool() ? static_cast<element_type*>(detail::ptr(base_type::operator*())) :
                                            nullptr;
    }

    void throw_when_ptr_expired_and_object_has_virtual_dtor() const
    {
        if (expired())
            throw ptr_is_already_deleted();
    }

    owned_pointer(element_type* const p, const bool acquired)
    {
        if (!p)
            return;
        const auto ss = get_secret_when_possible(p);

        if (!base_type::operator bool())
        {
            base_type::reset(new detail::control_block_type(p, {}, {}),
                             detail::owned_deleter<element_type>());
            set_shared_secret_when_possible(ss);
        }
        detail::acquired(base_type::operator*()) = acquired;
    }

    template <typename T, typename = typename std::enable_if<std::is_polymorphic<T>::value, void>::type>
    auto get_secret_when_possible(T* const p) noexcept -> detail::shared_secret*
    {
        if (auto ss = dynamic_cast<detail::shared_secret*>(p))
        {
            base_type::operator=(ss->control_block.lock());
            return ss;
        }
        return nullptr;
    }

    auto get_secret_when_possible(...) noexcept -> detail::shared_secret*
    {
        return nullptr;
    }

    void set_shared_secret_when_possible(detail::shared_secret* const ss) const
    {
        if (ss != nullptr)
            ss->control_block = *this;
    }
};

#if __cplusplus == 201703L
template <typename T>
owned_pointer(std::unique_ptr<T> &&)->owned_pointer<T>;

template <typename T>
owned_pointer(detail::link_ptr<T> &&)->owned_pointer<T>;
#endif

template <>
struct owned_pointer<void>
{
};

template <typename Object, typename... Args>
inline auto make_owned(Args&&... args) -> owned_pointer<Object>
{
    constexpr bool has_vdtor = std::has_virtual_destructor<Object>::value;
    using T = std::conditional<has_vdtor, detail::destruction_notify_object<Object>, Object>;

    return std::unique_ptr<Object>(new typename T::type{std::forward<Args>(args)...});
}

template <typename T>
auto link(const std::unique_ptr<T>& u) noexcept -> detail::link_ptr<T>
{
    return detail::link_ptr<T>{u};
}

template <typename R, typename T>
auto link(const std::unique_ptr<T>& u) noexcept -> detail::link_ptr<R>
{
    return detail::link_ptr<R>{u};
}

template <typename A>
inline bool operator==(const owned_pointer<A>& p1, std::nullptr_t) noexcept
{
    return p1.compare(nullptr) == 0;
}

template <typename A>
inline bool operator!=(const owned_pointer<A>& p1, std::nullptr_t) noexcept
{
    return p1.compare(nullptr) != 0;
}

template <typename A>
inline bool operator==(std::nullptr_t, const owned_pointer<A>& p1) noexcept
{
    return p1 == nullptr;
}

template <typename A>
inline bool operator!=(std::nullptr_t, const owned_pointer<A>& p1) noexcept
{
    return p1 != nullptr;
}

template <typename A, typename B>
inline bool operator==(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p1.compare(p2) == 0;
}

template <typename A, typename B>
inline bool operator!=(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return !(p1 == p2);
}

template <typename A, typename B>
inline bool operator<(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p1.compare(p2) < 0;
}

template <typename A, typename B>
inline bool operator<=(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p1.compare(p2) <= 0;
}

template <typename A, typename B>
inline bool operator>(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p1.compare(p2) > 0;
}

template <typename A, typename B>
inline bool operator>=(const owned_pointer<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p1.compare(p2) >= 0;
}

template <typename A, typename B>
inline bool operator==(const owned_pointer<A>& p1, const B* p2) noexcept
{
    return p1.compare(p2) == 0;
}

template <typename A, typename B>
inline bool operator==(const A* p1, const owned_pointer<B>& p2) noexcept
{
    return p2.compare(p1) == 0;
}

template <typename A, typename B>
inline bool operator!=(const owned_pointer<A>& p1, const B* p2) noexcept
{
    return p1.compare(p2) != 0;
}

template <typename A, typename B>
inline bool operator!=(const A* p1, const owned_pointer<B>& p2) noexcept
{
    return p2.compare(p1) != 0;
}

template <typename A, typename B>
inline bool operator==(const owned_pointer<A>& p1, const std::unique_ptr<B>& p2) noexcept
{
    return p1.compare(p2.get()) == 0;
}

template <typename A, typename B>
inline bool operator==(const std::unique_ptr<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p2.compare(p1.get()) == 0;
}

template <typename A, typename B>
inline bool operator!=(const owned_pointer<A>& p1, const std::unique_ptr<B>& p2) noexcept
{
    return p1.compare(p2.get()) != 0;
}

template <typename A, typename B>
inline bool operator!=(const std::unique_ptr<A>& p1, const owned_pointer<B>& p2) noexcept
{
    return p2.compare(p1.get()) != 0;
}

template <typename To, typename From>
auto ptr_static_cast(const owned_pointer<From>& from) noexcept -> owned_pointer<To>
{
    return {from};
}

}
