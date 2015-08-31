#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include "pmake_options.hpp"
#include "makefile_records.hpp"
#include "makefile_record.hpp"

enum class process_states
{
    MUST_REBUILD,
    UP_TO_DATE,
    QUESTION_FAILURE,
    BUILD_FAILED,
};

class pmake
{
    public:
        using pmake_variables = std::unordered_map<std::string, std::string>;

        pmake() = default;
        pmake(const std::vector<std::string>& makefile, pmake_options&& options, std::string&& exe_name);

        void add_variable(const std::string& name, const std::string& value) { m_variables[name] = value; }
        bool is_variable(const std::string& name) const { return m_variables.end() != m_variables.find(name); }
        const std::string& get_variable(const std::string& name) const { return m_variables.at(name); }

        void add_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options)
        {
            m_records.emplace_back(std::move(targets), std::move(dependencies), options);
        }

        const std::string& get_exe_name() const { return m_exe_name; }

        int run();
        
        static const std::regex var_def;
        static const std::regex var_use;
        static const std::regex target_def;
        static const std::regex command_def;
        static const std::regex item_def;
    
    private:
        pmake_options m_options;
        std::string m_exe_name;
        pmake_variables m_variables;
        makefile_records m_records;

        std::string replace_occurences(const std::string& input)
        {
            std::smatch use;
            std::string s = input;
            while (regex_search(s, use, var_use)) // dereferencing variable uses in definition
            {
                bool is_var = is_variable(use[1]);
                s = s.replace(use.position(0), use[0].length(), is_var ? get_variable(use[1]) : "");
                if (!is_var && m_options.is_warn_undefined())
                    std::cout << "Warning: variable " << use[0] << " is undefined. (--warn-undefined-variables)" << std::endl;
                use = std::smatch();
            }
            return std::move(s);
        }

        process_states process_target(const makefile_record& record);
};
