#pragma once
#include <vector>

template <typename T>
class ObjectPool
{
public:

    void addToPool(T obj) 
    { 
        resetFunc(obj);
        pool.push_back(obj); 
    }

    T pop() 
    { 
        if (pool.size())
            return popElem();

        else if (!pool.size() && autoExpansion)
        {
            setSize(expansionSize);
            return popElem();
        } 
        return nullptr;
    }

    void setSize(size_t size)
    { 
        auto objNeedToConstruct = (int)size - (int)pool.size() > 0 ? (int)size - (int)pool.size() : 0;
        if (objNeedToConstruct)
        {
            if (pool.capacity() - objNeedToConstruct < 0)
                pool.reserve(pool.capacity() - objNeedToConstruct);
            for (size_t i = 0; i < objNeedToConstruct; ++i)
                pool.push_back(creationCallback());
        }
    }

    ~ObjectPool()
    {
        releaseFunction();
    }

    void setCreationCallback(std::function<T()> callback) { creationCallback = callback; }
    void setReleaseFunction(std::function<void()> func) { releaseFunction = func; }
    void setResetFunction(std::function<void(T)> func) { resetFunc = func; }

    void setExpansionSize(size_t size) { expansionSize = size; }
    void setAutoExpansion(bool autoExpansion) { this->autoExpansion = autoExpansion; }

private:

    std::vector<T> pool;
    size_t expansionSize = 5;

    std::function<T()> creationCallback;
    std::function<void()> releaseFunction = []() {};
    std::function<void(T)> resetFunc = [](T obj) {};

    T createObj() { return creationCallback(); }

    bool autoExpansion = true;

    T popElem()
    {
        T ret = pool.back();
        pool.pop_back();
        return ret;
    }
};

