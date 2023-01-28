#include "beetle/exception.hpp"

#include <string>

namespace beetle {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

Exception::Exception(std::string msg) : m_msg{std::move(msg)} {}

char const* Exception::what() const noexcept {
    return this->m_msg.c_str();
}

}  // namespace cpp20_v1

}  // namespace beetle
