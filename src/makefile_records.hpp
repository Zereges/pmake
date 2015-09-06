#pragma once
#include <vector>
#include "makefile_record.hpp"
#include "file.hpp"

/**!
    \brief Class for storing all \ref makefile_record in Makefile.
*/
class makefile_records
{
    private:
        std::vector<makefile_record> m_records; //!< std::vector of makefile_record.

    public:
        // So that it looks like STL container.
        using size_type = decltype(m_records)::size_type;
        using difference_type = decltype(m_records)::difference_type;
        using value_type = decltype(m_records)::value_type;
        using reference = decltype(m_records)::reference;
        using pointer = decltype(m_records)::pointer;
        using const_reference = decltype(m_records)::const_reference;
        using const_pointer = decltype(m_records)::const_pointer;

        using iterator = decltype(m_records)::iterator;
        using const_iterator = decltype(m_records)::const_iterator;

        //! \return Number of records in container.
        std::size_t size() { return m_records.size(); }

        //! Returns the iterator to begin of the container.
        //! \return \ref iterator to the begin.
        std::vector<makefile_record>::iterator begin() { return m_records.begin(); }

        //! Returns the iterator to begin of the container.
        //! \return \ref const_iterator to the begin.
        std::vector<makefile_record>::const_iterator begin() const { return m_records.begin(); }

        //! Returns the iterator past the end of the container.
        //! \return \ref iterator past the end.
        std::vector<makefile_record>::iterator end() { return m_records.end(); }

        //! Returns the iterator past the end of the container.
        //! \return \ref const_iterator past the end.
        std::vector<makefile_record>::const_iterator end() const { return m_records.end(); }

        //! Returns reference to first record in container. If the container is empty, the function has undefined behaviour.
        //! \return \ref reference to first element in container.
        reference front() { return m_records.front(); }

        //! Returns reference to first record in container. If the container is empty, the function has undefined behaviour.
        //! \return \ref const_reference to first element in container.
        const_reference front() const { return m_records.front(); }

        //! Returns reference to last record in container. If the container is empty, the function has undefined behaviour.
        //! \return \ref reference to last element in container.
        reference back() { return m_records.back(); }

        //! Returns reference to last record in container. If the container is empty, the function has undefined behaviour.
        //! \return \ref const_reference to last element in container.
        const_reference back() const { return m_records.back(); }

        //! Inserts an element of value \p val by copy constructing it at the end.
        //! \param val element to be inserted into container.
        void push_back(const_reference val) { m_records.push_back(val); }

        //! Inserts an element of value \p val by move constructing it at the end.
        //! \param val element to be inserted into container.
        void push_back(value_type&& val) { m_records.push_back(std::move(val)); }

        //! Emplaces an element by calling constructor of \ref makefile_record in place.
        //! @tparam Ts Arguments of makefile_record constructor
        //! \param vals Arguments to pass to makefile_record's constructor.
        template<typename... Ts> void emplace_back(Ts&&... vals) { m_records.emplace_back(std::forward<Ts>(vals)...); }

        //! Finds a record by target name.
        //! \param target \ref file representing target to be found.
        //! \return iterator to found target, or end() if no such target exists.
        makefile_records::iterator find_record(const file& target)
        {
            iterator iter = std::find_if(begin(), end(), [&target](const makefile_record& rec)
            {
                return rec.get_target() == target;
            });
            return iter;
        }
};
