#pragma once

namespace utility
{
    template <typename T>
    class bst
    {
    private:
        bst<T>* m_left, m_right;

    public:

        void add(T element);
        void remove(T element);
        void contains(T element);
    };
}