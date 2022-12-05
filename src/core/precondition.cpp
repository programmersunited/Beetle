#include "beetle/precondition.hpp"

namespace beetle {

inline namespace cpp20_v1 {

void precondition(bool condition, std::optional<std::string_view> message, 
                  std::source_location location) {
}

}  // namespace cpp20_v1

}  // namespace beetle
