#ifndef _POLYNOMIAL_H_
#define _POLYNOMIAL_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <deque>

#define NODISCARD_ [[nodiscard]]

template<class T>
class Polynomial {
private:

    // структура для хранения элемента полинома: степени и коэффициента
	struct Elem {
	    Elem() : degree(0), coeff(0) {}

		Elem(const size_t deg, const T cf) : degree(deg), coeff(cf) {}

		size_t degree;
		T coeff;

        NODISCARD_ friend bool operator==(const Elem& lhs, const Elem& rhs) {
            if (lhs.degree != rhs.degree) {
                return false;
            }
			return lhs.coeff == rhs.coeff;
		}
	};

public:
	using coeff_type     = T;
	using const_iterator = typename std::deque<Elem>::const_iterator;

	Polynomial(const std::vector<coeff_type>& coeffs) {
		for (size_t i = 0; i < coeffs.size(); ++i) {
		    if (coeffs[i] != coeffTypeZero) {
                _polynomial.push_back(Elem(i, coeffs[i]));
		    }
		}
	}

	Polynomial(const coeff_type& coeff = {}) {
		if (coeff != coeffTypeZero) {
			_polynomial.push_back(Elem(0, coeff));
		}
	}

	template<class Iterator>	
	Polynomial(Iterator first, Iterator last) {
		size_t degree = 0;
		for (; first != last; ++first) {
		    if (*first != coeffTypeZero) {
                _polynomial.push_back(Elem(degree, *first));
		    }
			++degree;
		}
	}

	const_iterator begin() const {
		return _polynomial.cbegin();
	}

	const_iterator end() const {
		return _polynomial.cend();
	}

	std::int64_t Degree() const {
		if (_polynomial.size() == 0) {
			return -1;
		}
		return static_cast<std::int64_t>(_polynomial.back().degree);
	}

	// метод, возвращающий контейнер элементов полинома
	const std::deque<Elem>& GetPolynomial() const {
	    return _polynomial;
	}

	NODISCARD_ coeff_type operator[](const size_t power) const {
		if (power > Degree()) {
			return coeffTypeZero;
		}
		for (const auto& item : _polynomial) {
			if (item.degree == power) {
				return item.coeff;
			}
		}
        return coeffTypeZero;
	}

	NODISCARD_ coeff_type operator()(const coeff_type& val) const {
		coeff_type res = coeffTypeZero;
		for (const auto& item : _polynomial) {
			res += item.coeff * std::pow(val, item.degree);
		}
		return res;
	}

	Polynomial& operator+=(const Polynomial& right) {
        if (right.Degree() > Degree()) {
            for (size_t i = Degree() + 1; i <= right.Degree(); ++i) {
                _polynomial.push_back(Elem(i, 0));
            }
        }

		const auto& right_data = right._polynomial;
        if (_polynomial[0].degree != 0 && right_data[0].degree == 0) {
            _polynomial.insert(_polynomial.begin(), Elem(0, 0));
        }

        for (size_t i = 0; i < _polynomial.size(); ++i) {
            for (size_t j = 0; j < right_data.size(); ++j) {
                if (_polynomial[i].degree == right_data[j].degree) {
                    _polynomial[i].coeff += right_data[j].coeff;
                    break;
                }
            }
        }
        _normalize();
        return *this;
	}


	Polynomial& operator-=(const Polynomial& right) {
        if (right.Degree() > Degree()) {
            for (size_t i = Degree() + 1; i <= right.Degree(); ++i) {
                _polynomial.push_back(Elem(i, 0));
            }
        }

        const auto& right_data = right._polynomial;

        if (_polynomial[0].degree != 0 && right_data[0].degree == 0) {
            _polynomial.insert(_polynomial.begin(), Elem(0, 0));
        }

        for (size_t i = 0; i < _polynomial.size(); ++i) {
            for (size_t j = 0; j < right_data.size(); ++j) {
                if (_polynomial[i].degree == right_data[j].degree) {
                    _polynomial[i].coeff -= right_data[j].coeff;
                    break;
                }
            }
        }
        _normalize();
        return *this;
	}

	Polynomial& operator*=(const Polynomial& right) {
		if (Degree() == -1 || right.Degree() == -1) {
			_polynomial.resize(0);
			return *this;
		}
		const auto& right_data = right._polynomial;
		std::vector<coeff_type> tmp(Degree() + right.Degree() + 1);
		for (std::int64_t i = 0; i <= Degree(); ++i) {
			for (std::int64_t j = 0; j <= right.Degree(); ++j) {
			    size_t degree = _polynomial[i].degree + right_data[j].degree;
				tmp[degree] += _polynomial[i].coeff * right_data[j].coeff;
			}
		}
		Polynomial t(tmp);
		*this = t;
        _normalize();
		return *this;
    }

    friend Polynomial<T> operator/(Polynomial<T> lhs, const Polynomial<T>& rhs) {
        if (lhs.Degree() < rhs.Degree() || lhs.Degree() == -1) {
            return {};
        }
        const auto quotient = lhs[lhs.Degree()] / rhs[rhs.Degree()];
        if (lhs.Degree() == rhs.Degree()) {
            return {quotient};
        }

        std::vector<T> answer(lhs.Degree() - rhs.Degree() + 1);
        answer.back() = quotient;
        for (auto i = lhs.Degree(); i >= answer.size() -1 ; --i) {
            auto& lhs_pol = lhs._polynomial;
            auto it = find_if(lhs_pol.begin(), lhs_pol.end(), [i](const Elem& elem) {
                return elem.degree == i;
            });
            it->coeff = lhs[i] - rhs[rhs.Degree() - lhs.Degree() + i] * quotient;
        }
        lhs._normalize();
        Polynomial<T> tmp(answer);
        return tmp + lhs / rhs;
    }

    friend Polynomial<T> operator,(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
        if (lhs == lhs.coeffTypeZero) {
            if (rhs.Degree() != -1) {
                return rhs / rhs[rhs.Degree()];
            }
            return rhs;
        }
        return (rhs % lhs, lhs);
    }

private:
    // метод, удаляющий элементы с нулевым коэффициентом
	void _normalize() {
        auto found = find_if(_polynomial.begin(), _polynomial.end(), [](const Elem& elem) {
            return elem.coeff == coeffTypeZero;
        });
        if (found != _polynomial.end()) {
            _polynomial.erase(found);
            _normalize();
        }
	}

	std::deque<Elem> _polynomial;
	static inline const coeff_type coeffTypeZero{ static_cast<coeff_type>(0) };
};

template<class T>
NODISCARD_ bool operator==(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
    return lhs.GetPolynomial() == rhs.GetPolynomial();
}

template<class T>
NODISCARD_ bool operator!=(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
    return !( lhs.GetPolynomial() == rhs.GetPolynomial());
}

template<class T, class U>
NODISCARD_ bool operator==(const Polynomial<T>& polynomial, const U coeff) {
    if (polynomial.Degree() > 0) {
        return false;
    }
    if (polynomial.Degree() < 0) {
        return true;
    }
    return polynomial.GetPolynomial().back().coeff == coeff;
}

template<class T>
NODISCARD_ Polynomial<T> operator+(const Polynomial<T> lhs, const Polynomial<T> rhs) {
	Polynomial<T> tmp = lhs;
	return tmp += rhs;
}

template<class T>
NODISCARD_ Polynomial<T> operator-(const Polynomial<T> lhs, const Polynomial<T> rhs) {
	Polynomial<T> tmp = lhs;
	return tmp -= rhs;
}
 
template<class T>
NODISCARD_ Polynomial<T> operator*(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
	Polynomial<T> tmp = lhs;
	return tmp *= rhs;
}

template<class T>
NODISCARD_ Polynomial<T> operator&(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
    Polynomial<T> result;
    for (auto i = lhs.Degree(); i >= 0; --i) {
        result *= rhs;
        result += lhs[i];
    }
    return result;
}

template<class T>
Polynomial<T> operator%(const Polynomial<T>& lhs, const Polynomial<T>& rhs) {
    return lhs - (lhs / rhs) * rhs;
}

template<class T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& polynomial) {
    const T zero = static_cast<T>(0), one = static_cast<T>(1),
            minusOne = static_cast<T>(-1);
    if (polynomial.Degree() == -1) {
        return out << zero;
    }

    bool firstWriten = false;
    for (auto degree = polynomial.Degree(); degree >= 0; --degree) {
        const auto& coefficient = polynomial[degree];
        if (coefficient == zero) {
            continue;
        }

        if (coefficient > zero && firstWriten) {
            out << '+';
        }
        else if (degree > 0 && coefficient == minusOne) {
            out << '-';
        }

        if (!degree || (coefficient != one && coefficient != minusOne)) {
            out << coefficient;
            if (degree > 0) {
                out << '*';
            }
        }

        if (degree > 0) {
            out << 'x';
            if (degree > 1) {
                out << '^' << degree;
            }
        }
        firstWriten = true;
    }
    return out;
}

#endif // _POLYNOMIAL_H_
