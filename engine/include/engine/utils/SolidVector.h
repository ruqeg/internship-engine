//
// TODO move to .inl
//
#pragma once

#include <engine/types.h>

namespace utils
{

template<class T>
class SolidVector
{
public:
    using ID = uint32;
public:
    struct ForwardIndex
    {
        uint32 index;
        bool occupied;
    };
public:
    bool occupied(ID id) const
    {
        assert(id < m_forwardMap.size());
        return m_forwardMap[id].occupied;
    }
    uint32 size() const
    {
        return static_cast<uint32>(m_data.size());
    }

    const T* data() const
    {
        return m_data.data();
    }
    T* data()
    {
        return m_data.data();
    }

    const T& at(uint32 index) const
    {
        assert(index < m_data.size());
        return m_data[index];
    }
    T& at(uint32 index)
    {
        assert(index < m_data.size());
        return m_data[index];
    }

    const T& operator[](ID id) const
    {
        assertId(id);
        return m_data[m_forwardMap[id].index];
    }
    T& operator[](ID id)
    {
        assertId(id);
        return m_data[m_forwardMap[id].index];
    }

    ID insert(const T& value)
    {
        ID id = m_nextUnused;
        assert(id <= m_forwardMap.size());

        if (id == m_forwardMap.size())
            m_forwardMap.push_back({ static_cast<uint32>(m_forwardMap.size()) + 1, false });

        ForwardIndex& forwardIndex = m_forwardMap[id];
        assert(!forwardIndex.occupied);

        m_nextUnused = forwardIndex.index;
        forwardIndex = { static_cast<uint32>(m_data.size()), true };

        m_data.emplace_back(value);
        m_backwardMap.emplace_back(id);

        return id;
    }
    void erase(ID id)
    {
        assert(id < m_forwardMap.size());

        ForwardIndex& forwardIndex = m_forwardMap[id];
        assert(forwardIndex.occupied);

        m_data[forwardIndex.index] = std::move(m_data.back());
        m_data.pop_back();

        ID backwardIndex = m_backwardMap.back();

        m_backwardMap[forwardIndex.index] = backwardIndex;
        m_backwardMap.pop_back();

        m_forwardMap[backwardIndex].index = forwardIndex.index;

        forwardIndex = { m_nextUnused, false };
        m_nextUnused = id;
    }
    void clear()
    {
        m_forwardMap.clear();
        m_backwardMap.clear();
        m_data.clear();
        m_nextUnused = 0;
    }

public:
    static SolidVector<T>& getInstance()
    {
        static SolidVector<T> instance;
        return instance;
    }

private:
    SolidVector<T>() = default;
    SolidVector<T>(SolidVector<T>&) = delete;
    SolidVector<T>& operator=(SolidVector<T>&) = delete;

protected:
    void assertId(ID id) const
    {
        assert(id < m_forwardMap.size());
        assert(m_forwardMap[id].occupied);
    }

private:
    std::vector<T> m_data;
    std::vector<ForwardIndex> m_forwardMap;
    std::vector<ID> m_backwardMap;

    ID m_nextUnused = 0;
};

}