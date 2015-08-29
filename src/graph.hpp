#pragma once
#include <vector>
#include <memory>

template <typename T>
class graph
{
    public:
        class node
        {
            private:
                T m_value;
                std::vector<std::unique_ptr<node>> m_next;

            public:
                node(T&& value) : m_value(std::forward<T>(value)) { }
                node& emplace(T&& value)
                {
                    auto ptr = std::make_unique<node>(std::forward<T>(value));
                    m_next.push_back(std::move(ptr));
                    return *m_next.back();
                }

                void emplace(graph<T>&& g)
                {
                    m_next.push_back(std::move(g.m_root));
                }

                auto begin() -> decltype(m_next.begin()) { return m_next.begin(); }
                auto end() -> decltype(m_next.end()) { return m_next.end(); }
                auto begin() const -> decltype(m_next.begin()) { return m_next.begin(); }
                auto end() const -> decltype(m_next.end()) { return m_next.end(); }
        };

        graph(T&& value) : m_root(std::make_unique<node>(std::forward<T>(value))) { }

        node& get_root() { return *m_root; }

    private:
        std::unique_ptr<node> m_root;
};
