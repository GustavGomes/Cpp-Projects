#ifndef BIGINT_SRC_BIGINT_H
#define BIGINT_SRC_BIGINT_H

#include <iostream>
#include <vector>
#include <string>


class bigint {
private:
    std::vector<uint8_t> _digits;

    //Sign of the num
    bool _Neg = false;

public:

    bigint (): _digits(1, 0) {}

    bigint (int num) {

        //Checking if the number is negative
        _Neg = num < 0;

        //Modulus of num
        num = (num > 0) ? num : num = -num;

        //Building the vector
        while (num != 0 || _digits.empty()) {
            _digits.push_back(num % 10);
            num /= 10;
        }
    }

    bigint (const bigint& other) = default;
    ~bigint () = default;

    bigint& operator= (const bigint& other) = default;

    bigint& operator+= (const bigint& other) {
        //Both positive or negative
        if ((_Neg && other._Neg) || (!_Neg && !other._Neg)) {
            const std::vector<uint8_t> &top = (_digits.size() >= other._digits.size()) ? _digits : other._digits;
            const std::vector<uint8_t> &bottom = (_digits.size() < other._digits.size()) ? _digits : other._digits;
            std::vector<uint8_t> sum(top.size());

            bool carry = false;
            for (size_t i = 0; i < top.size(); i++) {
                sum[i] = uint8_t(carry) + top[i] + ((i < bottom.size()) ? bottom[i] : uint8_t(0)); // Summing and increasing the size of our vector to fit
                carry = sum[i] >= uint8_t(10); // If sum[i] >= 10 then carry is true
                sum[i] %= uint8_t(10); // Final value of sum[i]
            }
            if (carry) {
                sum.push_back(uint8_t(1)); // If the final carry is 1, then we need to append 1 in the beginning
            }
            _digits = sum;
            return *this;
        }

            // One negative and the other positive
        else {
            if (_Neg && !other._Neg) { // First negative and second positive
                _Neg = false;
                *this -= other;
                if (this->_digits.size() == 1 && (int) this->_digits[0] == 0) {
                    return *this;
                }
                _Neg = !(_Neg);
                return *this;
            }
            else { // First positive and second negative
                return operator-=(-other);
            }
        }
    }

    bigint& operator-= (const bigint& other) {
        //Check the case when other is 0
        if (other._digits[0]== 0 && other._digits.size() == 1) {
            return *this;
        }
        //First positive and second negative || first negative and second positive
        if ((!_Neg && other._Neg) || (_Neg && !other._Neg)) {
            bigint add = -other;
            return (*this += add);
        }

        std::vector<uint8_t> dif;
        bigint greater;
        bigint lesser;
        //Both positives
        if (!_Neg && !other._Neg) {
            //Check if they are equal
            bool equal = true;
            if (_digits.size() == other._digits.size()) {
                for (int i = 0; i < _digits.size(); i ++) {
                    if (_digits[i] != other._digits[i]) {
                        equal = false;
                        break;
                    }
                }
            }
            else {
                equal = false;
            }
            if (*this < other) {
                _Neg = true;
                greater = other;
                lesser = *this;
                dif.resize(greater._digits.size());
            }
            else if (equal) {
                *this = bigint(0);
                return *this;
            }
            else {
                greater = *this;
                lesser = other;
                dif.resize(_digits.size());
            }
        }
            //Both negative
        else {
            if (*this < other) {
                greater = *this;
                lesser = other;
                dif.resize(_digits.size());
            }
            else {
                _Neg = false;
                greater = other;
                lesser = *this;
                dif.resize(greater._digits.size());
            }
        }

        bool carry = false;

        for (size_t i = 0; i < greater._digits.size(); i++) {
            bool need_to_carry = greater._digits[i] < (i < lesser._digits.size() ? lesser._digits[i] : uint8_t(0)) + uint8_t(carry);
            dif[i] = need_to_carry ? uint8_t(10) : uint8_t(0);
            dif[i] += greater._digits[i];
            dif[i] -= (i < lesser._digits.size() ? lesser._digits[i] : uint8_t(0));
            dif[i] -= uint8_t(carry);

            carry = need_to_carry;
        }

        _digits = dif;

        while (_digits[_digits.size() - 1] == 0 && _digits.size() > 1) {
            _digits.pop_back();
        }

        return *this;

    }


    bigint& operator*= (const bigint& other) {
        bool neg = _Neg != other._Neg;
        // Selecting the greater and smaller numbers
        const std::vector<uint8_t>& greater = (_digits.size() >= other._digits.size()) ? _digits : other._digits;
        const std::vector<uint8_t>& lesser = (_digits.size() < other._digits.size()) ? _digits : other._digits;
        std::vector<uint8_t> product(greater.size() + lesser.size());


        for (size_t i = 0; i < lesser.size(); i++) {  //Iterating through the smaller number, i =1  , [j +1] = 2
            uint8_t carry = 0;
            for (size_t j = 0; j < greater.size(); j++) {  //Iterating through the greater number
                product[j + i] += lesser[i] * greater[j] + carry;
                carry = product[j + i] / uint8_t(10);
                product[j + i] %= 10;
            }
            product[i + greater.size()] = carry;
        }
        _digits = product;
        
        _Neg = _Neg==other._Neg ? false : true;
        while (_digits[_digits.size() - 1] == 0 && _digits.size() > 0){
            _digits.pop_back();
        }
        
        return *this;
    }


    bigint operator+ () const {
        return *this;
    }

    bigint operator- () const {
        bigint cpy(*this);
        cpy._Neg = !cpy._Neg;
        return cpy;
    }

    bigint& operator++ () {
        return operator+=(bigint(1));
    }

    bigint& operator-- () {
        return operator-=(bigint(1));
    }

    const bigint operator++ (int) {
        bigint cpy(*this);
        operator++();
        return cpy;
    }

    const bigint operator-- (int) {
        bigint cpy(*this);
        operator--();
        return cpy;
    }

    std::string to_string() const{
        std::string str;
        if (_Neg) {
            str = '-';
        }

        for (ssize_t i = _digits.size() - 1; i >= 0; i--)
            str.push_back('0' + _digits[i]);


        return str;
    }

    explicit operator bool() const {
        return _digits.size() > 1 || _digits[0] != 0;
    }

private:
    friend std::istream& operator>> (std::istream&, bigint&);
    friend std::ostream& operator<< (std::ostream&, const bigint&);
    friend bool operator< (const bigint&, const bigint&);
    friend bool operator> (const bigint&, const bigint&);
    friend bool operator<= (const bigint&, const bigint&);
    friend bool operator>= (const bigint&, const bigint&);
    friend bool operator!= (const bigint&, const bigint&);
    friend bool operator== (const bigint&, const bigint&);
};

/// end of class bigint.

bigint operator+ (const bigint& first, const bigint& second) { return bigint(first) += second; }
bigint operator- (const bigint& first, const bigint& second) { return bigint(first) -= second; }
bigint operator* (const bigint& first, const bigint& second) { return bigint(first) *= second; }

bool operator< (const bigint& first, const bigint& second) {
    if (first._digits.size() != second._digits.size()) {
        if (first._Neg == 0 && second._Neg == 0){
            return first._digits.size() < second._digits.size();
        }
        else if (first._Neg == 1 && second._Neg == 1){
            return second._digits.size() < first._digits.size();
        }
        else if (first._Neg == 1 && second._Neg == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    for (int i = first._digits.size() - 1; i >= 0; i--) {
        if (first._digits[i] != second._digits[i] && !(first._Neg != second._Neg)) {
            return (first._Neg && second._Neg) == 0 ?
                   first._digits[i] < second._digits[i] : second._digits[i] < first._digits[i];
        }
        if (first._Neg == 1 && second._Neg == 0) {
            return true;
        }
    }

    return false;
}

bool operator<= (const bigint& first, const bigint& second) {
    return !(second < first);
}

bool operator> (const bigint& first, const bigint& second) {
    return second < first;
}

bool operator>= (const bigint& first, const bigint& second) {
    return !(first < second);
}

bool operator== (const bigint& first, const bigint& second) {
    return !(first < second) && !(second < first);
}

bool operator!= (const bigint& first, const bigint& second) {
    return (first < second) || (second < first);
}

std::istream& operator>> (std::istream& is, bigint& num) {
    /// Need to clear the old digits.
    num._digits.clear();

    std::string str;
    is >> str;

    num._Neg = false;

    while(str[0] == '-'){
        num._Neg = !num._Neg;
        str.erase(0, 1);
    }

    bool inputted_non_zero_digit = false;

    for (size_t i = 0; i < str.size(); i ++) {
        if (str[i] >= '1' && str[i] <= '9') {
            num._digits.push_back(str[i] - '0');
            inputted_non_zero_digit = true;
        }

        else if (str[i] == '0') {
            if (inputted_non_zero_digit)
                num._digits.push_back(0);
        }

        else
            break;
    }

    /// reverse:
    for (size_t i = 0; i < num._digits.size() / 2; i ++) {
        const uint8_t tmp = num._digits[i];
        num._digits[i] = num._digits[num._digits.size() - 1 - i];
        num._digits[num._digits.size() - 1 - i] = tmp;
    }

    /// The case when user inputted only zeros:
    if (num._digits.size() == 0)
        num._digits.push_back(0);

    if(str == "0")
        num._Neg = false;

    return is;
}

std::ostream& operator<< (std::ostream& os, const bigint& num) {
    bool neg_temp = num._Neg;
    for (ssize_t i = num._digits.size() - 1; i >= 0; i --) {
        while (neg_temp) {
            os << '-';
            neg_temp = false;
        }
        os << (int)num._digits[i];
    }

    return os;
}

#endif /// BIGINT_SRC_BIGINT_H.