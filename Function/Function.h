#include <memory>

// 定义接口、用来隐藏真是的类型，
template <typename R, typename... Args>
struct ICallable
{
    virtual ~ICallable() {}
    virtual R invoke(Args &&...args) = 0;
};

// 实现桥接类
template <typename T, typename R, typename... Args>
class ICallableImpl : public ICallable<R, Args...>
{
private:
    T callable;

public:
    ICallableImpl(T &&_callable)
        : callable(std::forward<T>(_callable))
    {
    }

    R invoke(Args &&...args) override
    {
        return callable(std::forward<Args>(args)...);
    }
};

// 签名 模板的前向声明
// 为了 后续写 可调用对象的 特化版本
template <typename _Signature>
class function;

template <typename R, typename... Args>
class function<R(Args...)>
{
private:
    std::unique_ptr<ICallable<R, Args...>> funcptr;

public:
    template <typename T>
    function(T &&_callable)
        : funcptr(std::make_unique<ICallableImpl<T, R, Args...>>(
              std::forward<T>(_callable)))
    {
    }

    R operator()(Args &&...args) const
    {
        return funcptr->invoke(std::forward<Args>(args)...);
    }
};
