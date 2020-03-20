#include <memory>


class NullPtr : public std::runtime_error
{  
public:
    using std::runtime_error::runtime_error;
};

template<typename T>
class NonOwningPtr;

template<typename T>
class OwnedWrapper;

template<typename T>
NonOwningPtr<T> getConnectedNonOwningPtr(std::unique_ptr<OwnedWrapper<T>> uptr);

template<typename T>
class NonOwningPtr
{
public:
    using uptr_type = std::unique_ptr<OwnedWrapper<T>>;

    T* operator->() const
    {
        if(*this)
            return ptr;
        throw NullPtr("operator->");
    }

    T& operator*() const
    {
        if(*this)
            return *ptr;
        throw NullPtr("operator*");
    }

    operator bool() const
    {
        return ptr && !(*isExpired);
    }

    uptr_type unique_ptr()
    {
        if(uptr)
            return std::move(uptr);
        throw NullPtr("unique_ptr");
    }

private:
    friend OwnedWrapper<T>;
    friend NonOwningPtr getConnectedNonOwningPtr<T>(std::unique_ptr<OwnedWrapper<T>> uptr);

    NonOwningPtr(uptr_type&& uptr)
      : uptr(std::move(uptr))
      , ptr{this->uptr.get()}
    {
    }

    void expire()
    {
        isExpired = true;
    }

    uptr_type uptr;
    T* ptr;
    std::shared_ptr<bool> isExpired = std::make_shared<bool>(false);
};

template<typename T>
class OwnedWrapper : public T 
{
public:
    using T::T;

    void connect(NonOwningPtr<T>& ptr)
    {
        expiredFlag = ptr.isExpired;
    }

    ~OwnedWrapper()
    {
        auto sptr = expiredFlag.lock();
        if(sptr)
        {
            *sptr = true;
        }
    }
private:
    std::weak_ptr<bool> expiredFlag;
};

template<typename T>
NonOwningPtr<T> getConnectedNonOwningPtr(std::unique_ptr<OwnedWrapper<T>> uptr)
{
    auto& ref = *uptr;
    NonOwningPtr<T> ptr{std::move(uptr)};
    ref.connect(ptr);
    return ptr;
}

template<typename T, typename... Args>
NonOwningPtr<T> make_non_owning(Args... args)
{
    static_assert(std::has_virtual_destructor<T>::value, "");
    auto uptr = std::make_unique<OwnedWrapper<T>>(args...);
    return getConnectedNonOwningPtr(std::move(uptr));
}
