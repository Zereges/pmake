#pragma once
#include <vector>
#include "makefile_record.hpp"
#include "file.hpp"

class makefile_records
{
    private:
        std::vector<makefile_record> m_records;

    public:
        using size_type = decltype(m_records)::size_type;
        using difference_type = decltype(m_records)::difference_type;
        using value_type = decltype(m_records)::value_type;
        using reference = decltype(m_records)::reference;
        using pointer = decltype(m_records)::pointer;
        using const_reference = decltype(m_records)::const_reference;
        using const_pointer = decltype(m_records)::const_pointer;

        using iterator = decltype(m_records)::iterator;
        using const_iterator = decltype(m_records)::const_iterator;

        std::size_t size() { return m_records.size(); }

        std::vector<makefile_record>::iterator begin() { return m_records.begin(); }
        std::vector<makefile_record>::const_iterator begin() const { return m_records.begin(); }
        std::vector<makefile_record>::iterator end() { return m_records.end(); }
        std::vector<makefile_record>::const_iterator end() const { return m_records.end(); }

        reference front() { return m_records.front(); }
        const_reference front() const { return m_records.front(); }
        reference back() { return m_records.back(); }
        const_reference back() const { return m_records.back(); }

        void push_back(const_reference val) { m_records.push_back(val); }
        void push_back(value_type&& val) { m_records.push_back(std::move(val)); }
        template<typename... Ts> void emplace_back(Ts&&... vals) { m_records.emplace_back(std::forward<Ts>(vals)...); }

        makefile_records::iterator find_record(const file& target)
        {
            iterator iter = std::find_if(begin(), end(), [&target](const makefile_record& rec)
            {
                return rec.get_target() == target;
            });
            return iter;
        }
};
