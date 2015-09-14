#include "thread_manager.hpp"
size_t static_counter = 1; //!< Counts number of threads currently in program. Does not count threads which are waiting.
