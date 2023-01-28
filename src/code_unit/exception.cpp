#include "beetle/code_unit/exception.hpp"

#include <string>

namespace beetle {

inline namespace exceptions {

namespace utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

UnexpectedCodeUnit::UnexpectedCodeUnit(char8_t code_unit, std::string msg)
    : UnexpectedCodeUnit::base_type{std::move(msg)}, m_code_unit{code_unit} {}

UnexpectedCodeUnit::UnexpectedCodeUnit(char8_t code_unit) 
    : UnexpectedCodeUnit{code_unit, "Beetle Error: Found unexpected code unit " + std::to_string(code_unit) + "."} {}

char8_t UnexpectedCodeUnit::get_code_unit() const noexcept {
    return this->m_code_unit;
}

ExpectingLeadingByte::ExpectingLeadingByte(char8_t code_unit) 
    : ExpectingLeadingByte::base_type{code_unit, "Beetle Error: Expecting leading code unit byte but found " + 
                                        std::to_string(code_unit) + "."} {}

}  // namespace cpp20_v1

}  // namespace utf8

}  // namespace exceptions

}  // namespace beetle
