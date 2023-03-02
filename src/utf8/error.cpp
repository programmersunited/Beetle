#include "beetle/utf8/error.hpp"

#include <string>
#include <system_error>

namespace beetle {

namespace utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

namespace {

struct ErrorCategory : std::error_category {
    char const* name() const noexcept override { return "UTF-8"; }

    std::string message(int condition) const override {
        switch (static_cast<utf8::Error>(condition)) {
            case utf8::Error::eLeadingByte:
                return "expected a leading byte";
                break;

            case utf8::Error::eOverlongEncoded:
                return "detected overlong encoding";
                break;

            case utf8::Error::eContinuationByte:
                return "expected a continuation byte";
                break;

            case utf8::Error::eMissingByte:
                return "expected more bytes";
                break;

            default:
                return "unknown error";
        }
    }
};

ErrorCategory const error_category{};

}  // namespace

}  // namespace cpp20_v1

}  // namespace utf8

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

std::error_condition make_error_condition(utf8::Error error) noexcept {
    return {static_cast<int>(error), utf8::error_category};
}

}  // namespace cpp20_v1

}  // namespace beetle
