#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

class [[nodiscard]] Number final : private std::vector<uint32_t> {
public:
    using Digit = value_type;

private:
    class View final {
        const Digit* m_data;
        size_t m_size;

    public:
        View() noexcept {}
        View(const Digit* data, size_t n) noexcept : m_data{data}, m_size{n} {}

        View subview(this View self, size_t from) noexcept;
        View subview(this View self, size_t from, size_t to) noexcept;

        size_t size(this View self) noexcept;

        Digit operator[](this View self, size_t i) noexcept;
        Digit getOr(this View self, size_t i, Digit default_val) noexcept;

        using iterator = const Digit*;
        using reverse_iterator = std::reverse_iterator<const Digit*>;
        using const_iterator = const Digit*;
        using const_reverse_iterator = std::reverse_iterator<const Digit*>;

        iterator begin(this View self) noexcept;
        iterator end(this View self) noexcept;
        reverse_iterator rbegin(this View self) noexcept;
        reverse_iterator rend(this View self) noexcept;
        const_iterator cbegin(this View self) noexcept;
        const_iterator cend(this View self) noexcept;
        const_reverse_iterator crbegin(this View self) noexcept;
        const_reverse_iterator crend(this View self) noexcept;

        Number operator+(this View lhs, View rhs);
        Number operator/(this View lhs, View rhs);

        std::strong_ordering operator<=>(this View lhs, View rhs);
        std::strong_ordering cmpWithOffset(this View lhs, View rhs, size_t offset);

        bool operator==(this View lhs, View rhs) noexcept;

        Number mulNaive(this View lhs, View rhs);
        Number mulKaratsuba(this View lhs, View rhs);
    };

    using DoubleDigit = uint64_t;
    static_assert(sizeof(DoubleDigit) == sizeof(Digit) * 2);

    static constexpr size_t DIGIT_BITS = 8 * sizeof(Number::Digit);
    static constexpr Number::DoubleDigit BASE =
        static_cast<Number::DoubleDigit>(std::numeric_limits<Number::Digit>::max()) + 1;
    static constexpr Number::Digit MASK = std::numeric_limits<Number::Digit>::max();

    explicit Number(View v) : std::vector<Digit>{v.cbegin(), v.cend()} {}

    Number& operator+=(this Number& lhs, View rhs);
    Number& operator/=(this Number& lhs, View rhs);

    View view(this const Number& self) noexcept;
    View view(this const Number& self, size_t from) noexcept;
    View view(this const Number& self, size_t from, size_t to) noexcept;

    Digit getOr(this const Number& self, size_t i, Digit default_val) noexcept;
    Digit div10(this Number& self);

    void addWithOffset(this Number& lhs, View rhs, size_t offset);

    /// Returns the amount of leading zeros after subtraction
    size_t subWithOffset(this Number& lhs, View rhs, size_t offset);

    Number& operator-=(this Number& lhs, View rhs);
    Number& operator-=(this Number& lhs, Digit rhs);

public:
    operator View(this const Number & self) noexcept {
        return View{self.data(), self.size()};
    }

    Number() : std::vector<Digit>{} {}
    explicit Number(std::initializer_list<Digit> l) : std::vector<Digit>{l} {}

    [[nodiscard]] std::string toString(this Number const& self);
    [[nodiscard]] std::string toString(this Number&& self);

    Number& operator=(this Number& self, Digit value);

    bool operator==(this const Number& lhs, const Number& rhs) noexcept;

    Number operator+(this const Number& lhs, const Number& rhs);
    Number& operator+=(this Number& lhs, const Number& rhs);

    Number operator+(this const Number& lhs, Digit rhs);
    Number& operator+=(this Number& lhs, Digit rhs);

    Number operator/(this const Number& lhs, const Number& rhs);
    Number& operator/=(this Number& lhs, const Number& rhs);

    Number mulNaive(this const Number& lhs, const Number& rhs);
    Number mulKaratsuba(this const Number& lhs, const Number& rhs);
};
