// Kyle J Burgess

#ifndef K13_BASIC_ITERATOR_H
#define K13_BASIC_ITERATOR_H

#include <cstddef>
#include <vector>

namespace k13
{
    template<class T>
    class basic_iterator
    {
    public:

        // Type Traits
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        // Constructor
        explicit basic_iterator(T* ptr = nullptr)
            : m_ptr(ptr)
        {}

        // Pre-increment Operator
        basic_iterator& operator++()
        {
            ++m_ptr;
            return *this;
        }

        // Post-increment Operator
        basic_iterator operator++(int)
        {
            const auto r = *this;
            ++m_ptr;
            return r;
        }

        // Pre-decrement Operator
        basic_iterator& operator--()
        {
            --m_ptr;
            return *this;
        }

        // Post-decrement Operator
        basic_iterator operator--(int)
        {
            const auto r = *this;
            --m_ptr;
            return r;
        }

        // Addition Operator
        [[nodiscard]]
        basic_iterator operator+(difference_type d) const
        {
            return basic_iterator(m_ptr + d);
        }

        // Subtraction Operator
        [[nodiscard]]
        basic_iterator operator-(difference_type d) const
        {
            return basic_iterator(m_ptr - d);
        }

        // Subtraction Operator
        [[nodiscard]]
        difference_type operator-(const basic_iterator& o) const
        {
            return m_ptr - o.m_ptr;
        }

        // Increment Operator
        basic_iterator& operator+=(difference_type d)
        {
            m_ptr += d;
            return *this;
        }

        // Decrement Operator
        basic_iterator& operator-=(difference_type d)
        {
            m_ptr -= d;
            return *this;
        }

        // Equality Operator
        [[nodiscard]]
        bool operator==(basic_iterator o) const
        {
            return m_ptr == o.m_ptr;
        }

        // Inequality Operator
        [[nodiscard]]
        bool operator!=(basic_iterator o) const
        {
            return m_ptr != o.m_ptr;
        }

        // Pointer Operator
        [[nodiscard]]
        T& operator*()
        {
            return *m_ptr;
        }

        // Const Pointer Operator
        [[nodiscard]]
        const T& operator*() const
        {
            return *m_ptr;
        }

        // Member Access Operator
        [[nodiscard]]
        T* operator->()
        {
            return m_ptr;
        }

        // Const Member Access Operator
        [[nodiscard]]
        const T* operator->() const
        {
            return m_ptr;
        }

    protected:
        T* m_ptr;
    };
}

#endif
