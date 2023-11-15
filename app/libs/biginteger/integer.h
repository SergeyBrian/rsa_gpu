#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <limits>
#include <compare>

inline const unsigned int BASE = 256;
inline const unsigned int DIGITS10BASE = 3;

template<typename T>
constexpr unsigned char abs_c(const T &n) {
    if (n < 0) {
        return -n;
    }
    return n;
}

class bigint {

    std::vector<unsigned char> container;
    bool negative = false;

    template<typename T>
    void constructFromSignInt(T n);

    template<typename T>
    void constructFromUnsignInt(T n);

    template<typename T>
    void constructFromFloat(T n);

    unsigned char getDigit(unsigned int &k) const {
        if (k >= container.size()) {
            return 0;
        }
        return container[k];
    }

    void normalize() {
        container.erase(std::find_if(container.rbegin(), container.rend(),
                                     [](const unsigned char &d) { return d != 0; }).base(), container.end());
        container.shrink_to_fit();
        if (container.size() == 0) {
            container.push_back(0);
            negative = false;
        }
        return;
    }

public:

    // Constructors
    bigint() : container{0} {}

    bigint(const bool &n) : container{n} {}

    bigint(const unsigned char &n) : container{n} {}

    bigint(const unsigned short &n) { constructFromUnsignInt<unsigned short>(n); }

    bigint(const unsigned int &n) { constructFromUnsignInt<unsigned int>(n); }

    bigint(const unsigned long &n) { constructFromUnsignInt<unsigned long>(n); }

    bigint(const unsigned long long &n) { constructFromUnsignInt<unsigned long long>(n); }

    bigint(const signed char &n) : container{abs_c<signed char>(n)}, negative{n < 0} {}

    bigint(const char &n) : container{abs_c<char>(n)}, negative{n < 0} {}

    bigint(const short &n) { constructFromSignInt<short>(n); }

    bigint(const int &n) { constructFromSignInt<int>(n); }

    bigint(const long &n) { constructFromSignInt<long>(n); }

    bigint(const long long &n) { constructFromSignInt<long long>(n); }

    explicit bigint(const float &n) { constructFromFloat<float>(n); }

    explicit bigint(const double &n) { constructFromFloat<double>(n); }

    explicit bigint(const long double &n) { constructFromFloat<long double>(n); }

    // Unary arithmetic operators
    bigint operator+() const { return *this; }

    inline bigint operator-() const;

    friend inline bigint biabs(bigint n) {
        n.negative = false;
        return n;
    }

    // Comparison operators
    friend bool operator==(const bigint &a, const bigint &b);

    friend std::strong_ordering operator<=>(const bigint &a, const bigint &b);

    // Compound assignment operators
    bigint &operator+=(const bigint &b);

    bigint &operator-=(const bigint &b);

    bigint &operator*=(const bigint &b);

    bigint &operator/=(const bigint &b);

    bigint &operator%=(const bigint &b);

    // Increment/decrement
    inline bigint &operator++();

    inline bigint &operator--();

    bigint operator++(int) {
        bigint old = *this;
        ++*this;
        return old;
    }

    bigint operator--(int) {
        bigint old = *this;
        --*this;
        return old;
    }

    // Conversion functions
    inline explicit operator bool() const;

    explicit operator std::string() const;

    friend bigint stobi(const std::string &n);

    // Debug
    void dump() const {
        if (negative) {
            std::cout << "-_";
        }
        for (int con = container.size() - 1; con >= 0; con--) {
            std::cout << +container[con] << "_";
        }
        std::cout << std::endl;
    }
};

bigint stobi(const std::string &str) {
    bigint res;

    std::string::const_iterator msd = std::find_if_not(str.begin(), str.end(),
                                                       [](const char &d) { return std::isspace(d); });
    if (*msd == '+') {
        msd++;
    } else if (*msd == '-') {
        res.negative = true;
        msd++;
    }
    if (!std::isdigit(*msd)) {
        throw std::invalid_argument("stobi");
    }
    msd = std::find_if(msd, str.end(), [](const char &d) { return d != '0'; });
    if (!std::isdigit(*msd)) {
        res.negative = false;
        return res;
    }
    std::string::const_iterator alsd = std::find_if_not(msd, str.end(), [](const char &d) { return std::isdigit(d); });

    res.container.clear();
    std::string n(msd, alsd);
    while (n.size() > DIGITS10BASE || std::stoul(std::string(n, 0, DIGITS10BASE)) >= BASE) {
        std::string quot;
        unsigned int con = DIGITS10BASE;
        unsigned int partdivid = std::stoi(std::string(n, 0, DIGITS10BASE));
        if (partdivid < BASE) {
            partdivid = partdivid * 10 + (n[con] - '0');
            con += 1;
        }
        while (con < n.size()) {
            quot += partdivid / BASE + '0';
            partdivid = (partdivid % BASE) * 10 + (n[con] - '0');
            con++;
        }
        quot += partdivid / BASE + '0';
        partdivid %= BASE;
        res.container.push_back(partdivid);
        n = quot;
    }
    res.container.push_back(std::stoi(n));

    return res;
}

bigint operator "" _bi(const char *n) {
    std::string str = n;
    if (str.size() <= std::numeric_limits<unsigned long long>::digits10) {
        return bigint(std::stoull(str));
    }
    return stobi(str);
}

inline bigint bigint::operator-() const {
    bigint flip = *this;
    if (flip != 0_bi) {
        flip.negative = !(flip.negative);
    }
    return flip;
}

inline bigint &bigint::operator++() {
    *this += 1_bi;
    return *this;
}

inline bigint &bigint::operator--() {
    *this -= 1_bi;
    return *this;
}

bool operator==(const bigint &a, const bigint &b) {
    if (a.negative != b.negative) {
        return false;
    }
    return std::equal(a.container.begin(), a.container.end(), b.container.begin(), b.container.end());
}

std::strong_ordering operator<=>(const bigint &a, const bigint &b) {
    if (a.negative != b.negative) {
        return b.negative <=> a.negative;
    }
    if (a.negative == true) {
        if (a.container.size() != b.container.size()) {
            return b.container.size() <=> a.container.size();
        }
        return std::lexicographical_compare_three_way(b.container.rbegin(), b.container.rend(), a.container.rbegin(),
                                                      a.container.rend());
    }
    if (a.container.size() != b.container.size()) {
        return a.container.size() <=> b.container.size();
    }
    return std::lexicographical_compare_three_way(a.container.rbegin(), a.container.rend(), b.container.rbegin(),
                                                  b.container.rend());
}

inline bigint::operator bool() const {
    return *this != 0_bi;
}

inline bigint operator+(bigint a, const bigint &b) {
    a += b;
    return a;
}

inline bigint operator-(bigint a, const bigint &b) {
    a -= b;
    return a;
}

inline bigint operator*(bigint a, const bigint &b) {
    a *= b;
    return a;
}

inline bigint operator/(bigint a, const bigint &b) {
    a /= b;
    return a;
}

inline bigint operator%(bigint a, const bigint &b) {
    a %= b;
    return a;
}

bigint &bigint::operator+=(const bigint &b) {
    if (this == &b) {
        *this *= 2_bi;
        return *this;
    }
    if (b == 0_bi) {
        return *this;
    }
    if (negative != b.negative) {
        *this -= -b;
        return *this;
    }
    unsigned int digits = container.size();
    if (digits < b.container.size()) {
        digits = b.container.size();
    }
    unsigned int rem = 0;
    for (unsigned int k = 0; k < digits; k++) {
        unsigned int sum = rem + getDigit(k) + b.getDigit(k);
        rem = sum / BASE;
        sum %= BASE;
        if (k < container.size()) {
            container[k] = sum;
        } else {
            container.push_back(sum);
        }
    }
    if (rem != 0) {
        container.push_back(rem);
    }
    return *this;
}

bigint &bigint::operator-=(const bigint &b) {
    if (this == &b) {
        *this = 0_bi;
        return *this;
    }
    if (b == 0_bi) {
        return *this;
    }
    if (negative != b.negative) {
        *this += -b;
        return *this;
    }
    if (biabs(*this) < biabs(b)) {
        *this = -(b - *this);
        return *this;
    }
    unsigned int digits = container.size();
    unsigned int rem = 0;
    for (unsigned int k = 0; k < digits; k++) {
        int diff = container[k] - b.getDigit(k) - rem;
        rem = 0;
        if (diff < 0) {
            diff += BASE;
            rem = 1;
        }
        container[k] = diff;
    }
    normalize();
    return *this;
}

bigint &bigint::operator*=(const bigint &b) {
    if (*this == 0_bi) {
        return *this;
    }
    if (b == 0_bi) {
        *this = 0_bi;
        return *this;
    }
    bool sign = (negative != b.negative);
    bigint sum = 0_bi;
    for (unsigned int k = 0; k < b.container.size(); k++) {
        bigint part;
        part.container = std::vector<unsigned char>(k, 0);
        unsigned int rem = 0;
        for (unsigned int j = 0; j < container.size() || rem != 0; j++) {
            unsigned int prod = (b.container[k] * getDigit(j)) + rem;
            rem = prod / BASE;
            prod %= BASE;
            part.container.push_back(prod);
        }
        sum += part;
    }
    *this = sum;
    negative = sign;
    return *this;
}

bigint &bigint::operator/=(const bigint &b) {
    if (b == 0_bi) {
        throw std::domain_error("Division by zero");
    }
    if (biabs(*this) < biabs(b)) {
        *this = 0_bi;
        return *this;
    }
    bool sign = (negative != b.negative);
    bigint quot, partdivid;
    unsigned int con = b.container.size();
    quot.container.clear();
    partdivid.container = std::vector<unsigned char>(container.end() - con, container.end());
    con++;
    if (partdivid < b) {
        partdivid.container.insert(partdivid.container.begin(), *(container.end() - con));
        con++;
    }
    while (con <= container.size()) {
        unsigned int partquot = 0;
        while (partdivid >= 0_bi) {
            partdivid -= b;
            partquot++;
        }
        partdivid += b;
        partquot--;
        quot.container.push_back(partquot);
        partdivid.container.insert(partdivid.container.begin(), *(container.end() - con));
        partdivid.normalize();
        con++;
    }
    unsigned int partquot = 0;
    while (partdivid >= 0_bi) {
        partdivid -= b;
        partquot++;
    }
    partquot--;
    quot.container.push_back(partquot);
    std::reverse(quot.container.begin(), quot.container.end());
    *this = quot;
    negative = sign;
    return *this;
}

bigint &bigint::operator%=(const bigint &b) {
    *this = *this - (*this / b) * b;
    return *this;
}

bigint::operator std::string() const {
    std::string str;
    if (*this == 0_bi) {
        str += '0';
        return str;
    }
    bigint n = *this;
    n.negative = false;
    while (n > 0_bi) {
        str += (n % 10_bi).container[0] + '0';
        n /= 10_bi;
    }
    if (negative) {
        str += '-';
    }
    std::reverse(str.begin(), str.end());
    return str;
}

std::ostream &operator<<(std::ostream &os, const bigint &n) {
    os << static_cast<std::string>(n);
    return os;
}

std::istream &operator>>(std::istream &is, bigint &n) {
    std::string str;
    is >> str;
    try {
        n = stobi(str);
    } catch (std::invalid_argument &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

template<typename T>
void bigint::constructFromSignInt(T n) {
    if (n == 0) {
        container.push_back(0);
        return;
    }
    if (n < 0) {
        negative = true;
        n = -n;
    }
    while (n > 0) {
        container.push_back(n % BASE);
        n /= BASE;
    }
    return;
}

template<typename T>
void bigint::constructFromUnsignInt(T n) {
    if (n == 0) {
        container.push_back(0);
        return;
    }
    while (n > 0) {
        container.push_back(n % BASE);
        n /= BASE;
    }
    return;
}

template<typename T>
void bigint::constructFromFloat(T n) {
    if (n > -1 && n < 1) {
        container.push_back(0);
        return;
    }
    if (n < 0) {
        negative = true;
        n = -n;
    }
    n = std::floor(n);
    while (n > 0) {
        container.push_back(std::fmod(n, BASE));
        n /= BASE;
    }
    return;
}
