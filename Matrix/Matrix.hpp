// matrix header

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <string>

static const double E = 1e-16;

template<class T>
class Matrix {
private:
	size_t num_rows_;
	size_t num_columns_;
	std::vector<std::vector<T>> matrix;

public:
	Matrix() : num_rows_(0), num_columns_(0) {}

	Matrix(size_t rows, size_t cols) {
		Reset(rows, cols);
	}

	Matrix(const Matrix& other) {
		matrix       = other.matrix;
		num_rows_    = other.num_rows_;
		num_columns_ = other.num_columns_;
	}

	~Matrix() {}

	void Reset(size_t rows, size_t cols) {
		num_rows_    = rows;
		num_columns_ = cols;
		matrix.assign(rows, std::vector<T>(cols));
	}

	size_t GetNumRows() const {
		return num_rows_;
	}

	size_t GetNumColumns() const {
		return num_columns_;
	}

	T& At(size_t row, size_t column) {
		return matrix.at(row).at(column);
	}

	const T& At(size_t row, size_t column) const {
		return matrix.at(row).at(column);
	}

	// метод транспонирования матрицы
	void transpose() {
		Matrix<T> tmpM(num_columns_, num_rows_);
		for (size_t row = 0; row < num_columns_; row++) {
			for (size_t col = 0; col < num_rows_; col++) {
				tmpM.At(row, col) = At(col, row);
			}
		}
		*this = tmpM;
	}

	// метод вычисления определителя матрицы
	T det() const {
		if (num_columns_ != num_rows_) {
			throw std::invalid_argument("Cannot calculate the determinant of a non-square matrix");
		}
		if (num_rows_ == 2) {
			return At(0, 0)*At(1, 1) - At(0, 1)*At(1, 0);
		}
		Matrix<T> tmpM(*this);
		for (size_t i = 0; i < num_rows_; i++) {
			for (size_t j = i + 1; j < num_rows_; j++) {
				if (At(i, i) != 0) {
					double mu = tmpM.At(j, i) / tmpM.At(i, i);
					for (size_t k = 0; k < num_rows_; k++) {
						tmpM.At(j, k) -= tmpM.At(i, k) * mu;
						if (std::abs(tmpM.At(j, k)) < E) {
							tmpM.At(j, k) = 0;
						}
					}
				}
			}
		}
		T det = 1;
		for (size_t i = 0; i < num_rows_; i++) {
			det *= tmpM.At(i, i);
		}
		return det;
	}

	size_t col_pos(const std::vector<T>& vec) {
		size_t pos = vec.size();
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec[i] != 0) {
				pos = i;
				break;
			}
		}
		return pos;
	}

	// метод приведения матрицы с ступенчатому виду
	void RowEchelonForm() {
		for (size_t i = 0; i < num_rows_ - 1 ; i++) {
			for (size_t j = i + 1; j < num_rows_; j++) {
				if (At(i, i) != 0) {
					double mu = At(j, i) / At(i, i);
					for (size_t k = i; k < num_columns_; k++) {
						At(j, k) -= At(i, k) * mu;
					}
				}
			}
		}
		for (size_t i = 0; i < num_rows_ - 1; i++) {
			if (col_pos(matrix[i]) > col_pos(matrix[i+1])) {
				std::swap(matrix[i], matrix[i + 1]);
			}
		}
		for (size_t i = 0; i < num_rows_; i++) {
			size_t j = col_pos(matrix[i]);
			if (j != num_columns_) {
				double tmp =  At(i, j);
				for (size_t k = 0; k < num_columns_; k ++) {
					At(i, k) /= tmp;
					if (std::abs(At(i, k)) < E) {
						At(i, k) = 0;
					}
				}
				
			}
		}
	}

	// метод приведения матрицы к главному ступенчатому виду
	void ReducedRowEchelonForm() {
		RowEchelonForm();
		for (size_t i = 1; i < num_rows_; i++) {
			size_t j = col_pos(matrix[i]);
			if (j != num_columns_) {
				for (size_t z = 1; z <= i; z++) {
					T el1 = At(i, j);
					T el2 = At(i - z , j);
					double mu = el2 / el1;
					for (size_t k = 0; k < num_columns_; k++) {
						At(i - z, k) -= At(i, k) * mu;
					}
				}
			}
		}

	}

	// метод вычисления обратной матрицы
	Matrix<T> inverse() {
		Matrix<T> t(*this);
		if (num_rows_ != num_columns_) {
			throw std::invalid_argument("Cannot inverse a non-square matrix");
		}
		if (t.det() == 0) {
			throw std::invalid_argument("Cannot inverse the matrix: det = 0");
		}
		Matrix<T> tmpM(num_rows_, 2*num_columns_);
		for (size_t i = 0; i < num_rows_; i++) {
			for (size_t j = 0; j < num_columns_; j++) {
				tmpM.At(i, j) = At(i, j);
			}
		}
		for (size_t i = 0; i < num_columns_; i++) {
			tmpM.At(i, i + num_columns_) = 1;
		}
		tmpM.ReducedRowEchelonForm();
		Matrix<T> inverse(num_rows_, num_columns_);
		for (size_t i = 0; i < num_rows_; i++) {
			for (size_t j = 0; j < num_columns_; j++) {
				inverse.At(i, j) = tmpM.At(i, j + num_columns_);
			}
		}
		return inverse;
	}

};

template<class T>
std::istream& operator>>(std::istream& in, Matrix<T>& m) {
	if (m.GetNumRows() == 0 || m.GetNumColumns() == 0) {
		size_t num_cols;
		size_t num_rows;
		in >> num_rows >> num_cols;
		m.Reset(num_rows, num_cols);
	}
	for (size_t row = 0; row < m.GetNumRows(); row++) {
		for (size_t col = 0; col < m.GetNumColumns(); col++) {
			in >> m.At(row, col);
		}
	}
	return in;
}

template<class T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& m) {
	for (size_t row = 0; row < m.GetNumRows(); row++) {
		for (size_t col = 0; col < m.GetNumColumns(); col++) {
			out << m.At(row, col) << ' ';
		}
		out << std::endl;
	}
	return out;
}

template<class T>
Matrix<T> operator+(const Matrix<T>& m1, const Matrix<T>& m2) {
	if (m1.GetNumRows() != m2.GetNumRows()) {
		throw std::invalid_argument("Cannot sum matrices of different sizes");
	}
	if (m1.GetNumColumns() != m2.GetNumColumns()) {
		throw std::invalid_argument("Cannot sum matrices of different sizes");
	}
	Matrix<T> result(m1.GetNumRows(), m1.GetNumColumns());
	for (size_t row = 0; row < m1.GetNumRows(); row++) {
		for (size_t col = 0; col < m1.GetNumColumns(); col++) {
			result.At(row, col) = m1.At(row, col) + m2.At(row, col);
		}
	}
	return result;
}


template<class T>
Matrix<T> operator*(const Matrix<T>& m1, const Matrix<T>& m2) {
	if (m1.GetNumColumns() != m2.GetNumRows()) {
		throw std::invalid_argument("Cannot multiply these matrices: matrix #1 num cols does not equal to matrix #2 num rows");
	}
	Matrix<T> result(m1.GetNumRows(), m2.GetNumColumns());
	for (size_t i = 0; i < m1.GetNumRows(); i++) {
		for (size_t j = 0; j < m2.GetNumColumns(); j++) {
			for (size_t k = 0; k < m1.GetNumColumns(); k++) {
				result.At(i, j) += m1.At(i, k) * m2.At(k, j);
			}
		}
	}
	return result;
}

template<class T, class T2>
Matrix<T> operator*(Matrix<T>& m, const T2& number) {
	Matrix<T> result(m.GetNumRows(), m.GetNumColumns());
	for (size_t row = 0; row < m.GetNumRows(); row++) {
		for (size_t col = 0; col < m.GetNumColumns(); col++) {
			result.At(row, col) = m.At(row, col) * number;
		}
	}
}

template<class T, class T2>
void operator*=(Matrix<T>& m, const T2& number) {
	for (size_t row = 0; row < m.GetNumRows(); row++) {
		for (size_t col = 0; col < m.GetNumColumns(); col++) {
			m.At(row, col) *= number;
		}
	}
}

template<class T>
void operator*=(Matrix <T>& m1, const Matrix<T>& m2) {
	if (m1.GetNumColumns() != m2.GetNumRows()) {
		throw std::invalid_argument("Cannot multiply these matrices: matrix #1 num cols does not equal to matrix #2 num rows");
	}
	Matrix<T> tmp(m1.GetNumRows(), m2.GetNumColumns());
	for (size_t i = 0; i < m1.GetNumRows(); i++) {
		for (size_t j = 0; j < m2.GetNumColumns(); j++) {
			for (size_t k = 0; k < m1.GetNumColumns(); k++) {
				tmp.At(i, j) += m1.At(i, k) * m2.At(k, j);
			}
		}
	}
	m1 = tmp;
}

template<class T>
void operator+=(Matrix<T>& m1, const Matrix<T>& m2) {
    if (m1.GetNumRows() != m2.GetNumRows()) {
        throw std::invalid_argument("Cannot add matrices of different sizes");
    }
    if (m1.GetNumColumns() != m2.GetNumColumns()) {
        throw std::invalid_argument("Cannot add matrices of different sizes");
    }
    Matrix<T> result(m1.GetNumRows(), m1.GetNumColumns());
    for (size_t row = 0; row < m1.GetNumRows(); row++) {
        for (size_t col = 0; col < m1.GetNumColumns(); col++) {
            m1.At(row, col) += m2.At(row, col);
        }
    }
}

#endif // _MATRIX_H_