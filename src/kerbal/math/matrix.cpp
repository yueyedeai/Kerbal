/**
 * @file matrix.cpp
 * @date 2017-3-30
 * @author 倪文卿
 */

#include "matrix.hpp"

#include <algorithm>
#include "../string_serve.hpp"

#if __cplusplus < 201103L //C++0x
#	pragma message("\n\
        * Matrix 使用了 C++ 11 新特性, 请打开 C++ 11 选项以便使用这些新特性\n\
                * Matrix 为 C++ 11 准备的新特性: 转移构造函数\n\
                * Matrix 为 C++ 11 准备的新特性: 利用一维初始化列表进行构造\n\
                * Matrix 为 C++ 11 准备的新特性: 利用二维初始化列表进行构造\n\
                * Matrix 为 C++ 11 准备的新特性: 转移赋值运算符"\
)
#endif //C++0x

#ifdef _OPENMP
# include <omp.h>
# pragma message("\n\
        * " __FILE__ " 已启用 openMP. 版本代码为: " STR2(_OPENMP)\
)
#endif

namespace std
{
	void swap(kerbal::math::matrix::Matrix &a, kerbal::math::matrix::Matrix &b)
	{
		swap(a.row, b.row);
		swap(a.column, b.column);
		swap(a.p, b.p);
	}
}

namespace kerbal
{
	namespace math
	{
		namespace matrix
		{
			namespace
			{
				using namespace string_serve;
			}

			using namespace kerbal::data_struct::array_2d;

			Matrix::Matrix(size_t row, size_t column, Uninit) :
					Array_2d<double>(row, column, Array_2d<double>::uninit_tag)
			{
			}

			Matrix::Matrix() throw () :
					Array_2d<double>()
			{
			}

			Matrix::Matrix(size_t row, size_t column) :
					Array_2d<double>(row, column)
			{
			}

			Matrix::Matrix(size_t row, size_t column, const double &val) :
					Array_2d<double>(row, column, val)
			{
			}

#if __cplusplus < 201103L
			Matrix::Matrix(double (*function)(), size_t row, size_t column, bool para) :
					Array_2d<double>(function, row, column, para)
			{
			}

			Matrix::Matrix(double (*function)(size_t, size_t), size_t row, size_t column, bool para) :
					Array_2d<double>(function, row, column, para)
			{
			}

#else

			Matrix::Matrix(std::function<double()> fun, size_t row, size_t column, bool para) :
			Array_2d<double>(fun, row, column, para)
			{
			}

			Matrix::Matrix(std::function<double(size_t, size_t)> fun, size_t row, size_t column, bool para) :
			Array_2d<double>(fun, row, column, para)
			{
			}
#endif

#if __cplusplus >= 201103L //C++0x
			Matrix::Matrix(std::initializer_list<std::initializer_list<double>> src) :
			Array_2d<double>(src)
			{ //利用二维初始化列表进行构造
			}

			Matrix::Matrix(std::initializer_list<double> src) :
			Array_2d<double>(src)
			{ //利用一维初始化列表进行构造
			}

			Matrix::Matrix(Matrix &&src) :
			Array_2d<double>(src)
			{ //转移构造函数
			}

#endif //C++0x

			Matrix::Matrix(const Matrix &src) :
					Array_2d<double>(src)
			{
			}

			Matrix::~Matrix() // virtual
			{
			}

			void Matrix::print(Matrix::Frame_style frame, bool print_corner, std::ostream &output) const
			{
				//Frame frame 默认= Fr_RtMatrix, bool print_corner 默认= true, ostream &output = output
				//static const std::string Corner_RtMatrix[] = { "┌", "┐", "└", "┘", "│" };

				static const char Corner_RdMatrix[5][5] = { "╭", "╮", "╰", "╯", "│" };
				static const char Corner_Det[5][5] = { " ", " ", " ", " ", "│" };
				static const char Corner_double[5][5] = { " ", " ", " ", " ", "││" };

				const char (*corner)[5] = NULL;
				switch (frame) {
					case Matrix::rt_corner:
						if (print_corner) {
							Array_2d<double>::print(Array_2d<double>::frame_with_corner, output);
						} else {
							Array_2d<double>::print(Array_2d<double>::frame_only, output);
						}
						return;
					case Matrix::none:
						Array_2d<double>::print(Array_2d<double>::none, output);
						return;
					case Matrix::rd_corner:
						corner = Corner_RdMatrix;
						break;
					case Matrix::single_det:
						corner = Corner_Det;
						break;
					case Matrix::double_det:
						corner = Corner_double;
						break;
				}

				output << std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left)
						<< std::setfill(' '); //清除右对齐, 设置左对齐, 设置不足补空格

				size_t i, j;
				output << corner[0];
				for (j = 0; j < column; j++) {
					output << "  " << std::setw(12) << " ";
				}
				output << " " << corner[1] << std::endl;

				for (i = 0; i < row; i++) {
					output << corner[4];

					for (j = 0; j < column; j++) {
						output << "  " << std::setw(12) << p[i][j];
					}
					output << " " << corner[4] << std::endl;
				}

				output << corner[2];
				for (j = 0; j < column; j++) {
					output << "  " << std::setw(12) << " ";
				}
				output << " " << corner[3];
				if (print_corner) {
					output << " " << row << " × " << column;
				}
				output << std::endl;
			}

			void Matrix::save(std::ofstream & fout) const
					throw (std::runtime_error, __cxxabiv1:: __forced_unwind)
			{
				const size_t sizeof_row = sizeof(row);
				const size_t sizeof_column = sizeof(column);
				const size_t sizeof_element = sizeof(p[0][0]);

				/* 输出位宽信息 */
				try {
					switch (sizeof(size_t)) {
						case 4: //size_t 32位版本
							fout.write((const char*) &sizeof_row, 4); //输出变量row在内存中所占的长度
							fout.write("\0\0\0\0", 4); //不足8位补4个零
							fout.write((const char*) &sizeof_column, 4);
							fout.write("\0\0\0\0", 4);
							fout.write((const char*) &sizeof_element, 4);
							fout.write("\0\0\0\0", 4);
							break;
						case 8: //size_t 64位版本
							fout.write((const char*) &sizeof_row, 8); //输出变量row在内存中所占的长度
							fout.write((const char*) &sizeof_column, 8);
							fout.write((const char*) &sizeof_element, 8);
							break;
						default:
							throw std::runtime_error("不支持的位宽");
					}

					fout.write((const char*) &row, sizeof_row); //输出矩阵行数
					fout.write((const char*) &column, sizeof_column); //输出矩阵列数
					for (size_t i = 0; i < row; ++i) {
						for (size_t j = 0; j < column; ++j) {
							fout.write((const char*) (p[i] + j), sizeof_element); //输出矩阵元素
						}
					}
				} catch (...) {
					fout.close();
					throw;
				}
			}

			void Matrix::save(const char * file_name) const
					throw (std::runtime_error, __cxxabiv1:: __forced_unwind)
			{
				std::ofstream fout(file_name, std::ios::out | std::ios::binary);
				if (!fout) {
					throw std::runtime_error("文件打开失败");
				}
				this->save(fout);
				fout.close();
			}

			const Matrix load_from(std::ifstream & fin) throw (std::runtime_error)
			{
				size_t size_of_row_in_file;
				size_t size_of_column_in_file;
				size_t size_of_ele_in_file;

				fin.read((char*) &size_of_row_in_file, 4);
				fin.ignore(4);
				fin.read((char*) &size_of_column_in_file, 4);
				fin.ignore(4);
				fin.read((char*) &size_of_ele_in_file, 4);
				fin.ignore(4);

				static const size_t size_of_row = sizeof(Matrix::row);
				static const size_t size_of_column = sizeof(Matrix::column);
				static const size_t size_of_ele = sizeof(double);

				if (size_of_row != size_of_row_in_file

				|| size_of_column != size_of_column_in_file

				|| size_of_ele != size_of_ele_in_file) {

					fin.close();
					throw std::runtime_error("位宽不兼容");
				}

				size_t row;
				size_t column;
				fin.read((char*) &row, size_of_row);
				fin.read((char*) &column, size_of_column);

				Matrix tmp(row, column);
				for (size_t i = 0; i < row; i++) {
					for (size_t j = 0; j < column; j++) {
						fin.read((char*) (tmp.p[i] + j), size_of_ele);
					}
				}
				return tmp;
			}

			const Matrix load_from(const char * file_name) throw (std::runtime_error)
			{
				std::ifstream fin(file_name, std::ios::in | std::ios::binary);
				if (!fin) {
					throw std::runtime_error("文件打开失败");
				}
				const Matrix tmp = load_from(fin);
				fin.close();
				return tmp;
			}

			void Matrix::switch_rows(size_t row1, size_t row2)
			{
				this->test_row(row1);
				this->test_row(row2);
				std::swap(p[row1], p[row2]);
			}

			void Matrix::switch_rows(size_t row1, size_t row2, size_t column_index_start)
			{
				this->switch_rows(row1, row2, column_index_start, this->column);
			}

			void Matrix::switch_rows(size_t row1, size_t row2, size_t column_index_start, size_t column_index_end)
			{
				this->test_row(row1);
				this->test_row(row2);
				this->test_column(column_index_start);
				this->test_column(column_index_end - 1);

				if ((column_index_end - column_index_start) < this->column / 2) {
					for (size_t j = column_index_start; j != column_index_end; ++j) {
						std::swap(this->p[row1][j], this->p[row2][j]);
					}
				} else {
					std::swap(this->p[row1], this->p[row2]);
					for (size_t j = 0; j != column_index_start; ++j) {
						std::swap(this->p[row1][j], this->p[row2][j]);
					}
					for (size_t j = column_index_end; j != this->column; ++j) {
						std::swap(this->p[row1][j], this->p[row2][j]);
					}
				}
			}

			void Matrix::switch_columns(size_t column1, size_t column2)
			{
				this->test_column(column1);
				this->test_column(column2);

				for (size_t i = 0; i != row; ++i) {
					std::swap(p[i][column1], p[i][column2]);
				}
			}

			void Matrix::kmr(double k, size_t row_dest)
			{
				test_row(row_dest);

				for (size_t i = 0; i != column; ++i) {
					p[row_dest][i] *= k;
				}
			}

			void Matrix::kmr_plus_to_another(double k, size_t row_from, size_t row_dest)
			{
				test_row(row_from);
				test_row(row_dest);

				for (size_t i = 0; i != column; ++i) {
					p[row_dest][i] += k * p[row_from][i];
				}
			}

			void Matrix::kmc(double k, size_t column_dest)
			{
				test_column(column_dest);

				for (size_t i = 0; i != row; ++i) {
					p[i][column_dest] *= k;
				}
			}

			void Matrix::kmc_plus_to_another(double k, size_t column_from, size_t column_dest)
			{
				test_column(column_from);
				test_column(column_dest);

				for (size_t i = 0; i != row; ++i) {
					p[i][column_dest] += k * p[i][column_from];
				}
			}

			void Matrix::do_triu()
			{
				bool k = false; //是否取相反数

				for (size_t M = 0; M < this->row - 1 && M < this->column - 1; ++M) {
					for (size_t i = 1; i < this->row - M; ++i) {
						for (size_t j = M; i + j < this->row; ++j) {
							if (p[j + 1][M] != 0) {
								if (p[j][M] == 0 || fabs(p[j][M]) > fabs(p[j + 1][M])) {
									this->switch_rows(j, j + 1, M);
									k = !k;
								}
							}
						}
					}

					if (p[M][M] != 0) {
						for (size_t i = M + 1; p[i][M] != 0.0 && i < this->row; ++i) {
							double ra = -p[i][M] / p[M][M];
							for (size_t j = M; j < this->column; ++j) { //列循环
								p[i][j] += ra * p[M][j];
							}
						}
					}
				}

				if (k) { //k == true
					for (size_t i = 0; p[i][0] != 0.0 && i < this->row; ++i) {
						p[i][0] = -p[i][0];
					}
				}
			}

			double Matrix::det() const
			{
				this->test_square();

				Matrix copy = *this;
				copy.do_triu();

				double sum = 1.0;
				for (size_t i = 0; i != copy.row; ++i) {
					sum *= copy.p[i][i];
				}

				return sum;
			}

			Matrix Matrix::Adjugate_matrix() const
			{ //返回本方阵的伴随矩阵

				this->test_square();
				Matrix result(row, column);

				size_t i, j;
				for (i = 0; i != row; ++i) {
					for (j = 0; j != column; ++j) {
						//i+j为偶数
						result.p[j][i] = cofactor_of(i, j).det();
						if ((i + j) % 2) {
							//i+j为奇数
							result.p[j][i] = -result.p[j][i];
						}
					}
				}
				return result;
			}

			Matrix Matrix::Inverse_matrix() const
			{ //返回本方阵的逆矩阵

				double D = this->det();
				if (D == 0.0) {
					throw std::invalid_argument("A does not have an inverse matrix");
				}
				const double k = 1.0 / D;
				return (k * this->Adjugate_matrix());
			}

			Matrix Matrix::mul_with_trans() const
			{
				Matrix result(this->row, this->row);

				size_t i, j, k;
				double sum;
				for (i = 0; i != this->row; ++i) {
					for (j = 0; j != this->row; ++j) {
						sum = 0.0;
						for (k = 0; k != this->column; ++k) {
							sum += this->p[i][k] * this->p[j][k];
						}
						result.p[i][j] = sum;
					}
				}
				return result;
			}

			//运算符重载
			const Matrix operator+(const Matrix &A, const Matrix &B)
			{ //检查A,B是否同样大小
				if (A.row != B.row) {
					throw std::invalid_argument("error: row(A) ≠ row(B)");
				}
				if (A.column != B.column) {
					throw std::invalid_argument("error: column(A) ≠ column(B)");
				}
				const size_t &row = A.row;
				const size_t &column = A.column;

				Matrix result(row, column);

				size_t i, j;
				for (i = 0; i != row; ++i) {
					for (j = 0; j != column; ++j) {
						result.p[i][j] = A.p[i][j] + B.p[i][j];
					}
				}
				return result;
			}

			const Matrix operator-(const Matrix &A, const Matrix &B)
			{ //检查A,B是否同样大小
				if (A.row != B.row) {
					throw std::invalid_argument("error: row(A) ≠ row(B)");
				}
				if (A.column != B.column) {
					throw std::invalid_argument("error: column(A) ≠ column(B)");
				}
				const size_t &row = A.row;
				const size_t &column = A.column;

				Matrix result(row, column);

				size_t i, j;
				for (i = 0; i != row; ++i) {
					for (j = 0; j != column; ++j) {
						result.p[i][j] = A.p[i][j] - B.p[i][j];
					}
				}
				return result;
			}

			const Matrix operator*(const double k, const Matrix &A)
			{ //数k乘矩阵
				if (k == 1) {
					return A;
				}
				Matrix result(A);
				size_t i, j;
				for (i = 0; i != A.row; ++i) {
					for (j = 0; j != A.column; ++j) {
						result.p[i][j] *= k;
					}
				}
				return result;
			}

			const Matrix operator*(const Matrix &A, const double k)
			{ //矩阵乘数k
				if (k == 1) {
					return A;
				}
				Matrix result(A);
				size_t i, j;
				for (i = 0; i != A.row; ++i) {
					for (j = 0; j != A.column; ++j) {
						result.p[i][j] *= k;
					}
				}
				return result;
			}

			const Matrix operator*(const Matrix &A, const Matrix &B) throw (std::invalid_argument)
			{ //矩阵乘矩阵

				//检查A的列数是否等于B的行数
				if (A.column != B.row) {
					throw std::invalid_argument("error: column(A) ≠ row(B)");
				}

				Matrix result(A.row, B.column);

				double sum;

#ifndef _OPENMP
				size_t i, j, k;
				for (i = 0; i != A.row; ++i) {
					for (j = 0; j != B.column; ++j) {
						sum = 0.0;
						for (k = 0; k != A.column; ++k) {
							sum += A.p[i][k] * B.p[k][j];
						}
						result.p[i][j] = sum;
					}
				}
#else

#pragma message("\n\
        * parallel Matrix operator* enable"\
)
#				pragma omp parallel
				{
#					pragma omp for nowait
					for (size_t i = 0; i < A.row; ++i) {
						for (size_t j = 0; j < B.column; ++j) {
							sum = 0.0;
							for (size_t k = 0; k != A.column; ++k) {
								sum += A.p[i][k] * B.p[k][j];
							}
							result.p[i][j] = sum;
						}
					}
				}
#endif
				return result;
			}

			const Matrix fma(const Matrix &A, const Matrix &B, const Matrix &C)
			{ // A * B + C

				//检查A的列数是否等于B的行数
				if (A.column != B.row) {
					throw std::invalid_argument("error: column(A) ≠ row(B)");
				}
				const size_t &row = A.row;
				const size_t &column = B.column;
				if (row != C.row || column != C.column) {
					throw std::invalid_argument("error: size(A*B) ≠ size(C)");
				}

				Matrix result(C);

				size_t i, j, k;
				double sum;
				for (i = 0; i != row; ++i) {
					for (j = 0; j != column; ++j) {
						sum = 0.0;
						for (k = 0; k != A.column; ++k) {
							sum += A.p[i][k] * B.p[k][j];
						}
						result.p[i][j] += sum;
					}
				}
				return result;
			}

			const Matrix dot_product(const Matrix &A, const Matrix &B)
			{ //矩阵点乘矩阵

				if (A.row == 1 && A.column == 1) {
					return A.p[0][0] * B;
				} else if (B.row == 1 && B.column == 1) {
					return B.p[0][0] * A;
				} else if (A.row != B.row) { //检查A,B是否同样大小
					throw std::invalid_argument("error: row(A) ≠ row(B)");
				} else if (A.column != B.column) { //检查A,B是否同样大小
					throw std::invalid_argument("error: column(A) ≠ column(B)");
				}
				const size_t &row = A.row;
				const size_t &column = A.column;

				Matrix result(row, column);
				size_t i, j;
				for (i = 0; i != row; ++i) {
					for (j = 0; j != column; ++j) {
						result.p[i][j] = A.p[i][j] * B.p[i][j];
					}
				}

				return result;
			}

			const Matrix eye(size_t n)
			{ //构造一个单位矩阵
				Matrix result(n, n, 0);
				for (size_t i = 0; i != n; ++i) {
					result.p[i][i] = 1;
				}
				return result;
			}

			const Matrix operator^(const Matrix &A, const int n)
			{ //计算矩阵A的n次方

				if (n >= 4) {
					Matrix tmp = A ^ (n / 2);
					tmp = tmp * tmp;
					if (n % 2) {
						//指数为奇数
						tmp = tmp * A;
					}
					return tmp;
				} else if (n == 3) {
					return A * A * A;
				} else if (n < 0) {
					return A.Inverse_matrix() ^ -n;
				} else if (n == 2) {
					return A * A;
				} else if (n == 1) {
					return A;
				} else { //n == 0
					A.test_square();
					return eye(A.row);
				}
			}

			const Matrix operator&&(const Matrix &A, const Matrix &B)
			{ //将两个矩阵按竖直方向连接
				/*   A
				 *  ---
				 *   B  */

				if (A.column != B.column) {
					throw std::invalid_argument("串联的矩阵的列数不一致");
				}

				const size_t row_total = A.row + B.row;
				const size_t &column_total = A.column;

				Matrix result(row_total, column_total, Array_2d<double>::uninit_tag);

				for (size_t i = 0; i != A.row; ++i) { //行循环
					std::uninitialized_copy(A.p[i], A.p[i] + column_total, result.p[i]);
				}

				for (size_t i = 0; i != B.row; ++i) { //行循环
					std::uninitialized_copy(B.p[i], B.p[i] + column_total, result.p[A.row + i]);
				}

				return result;
			}

			const Matrix operator||(const Matrix &A, const Matrix &B)
			{ //将两个矩阵按水平方向连接
				/*   A | B   */

				if (A.row != B.row) {
					throw std::invalid_argument("串联的矩阵的行数不一致");
				}
				const size_t &row_total = A.row;
				const size_t column_total = A.column + B.column;

				Matrix result(row_total, column_total, Array_2d<double>::uninit_tag);

				for (size_t i = 0; i != row_total; ++i) { //行循环
					std::uninitialized_copy(A.p[i], A.p[i] + A.column, result.p[i]);
					std::uninitialized_copy(B.p[i], B.p[i] + B.column, result.p[i] + A.column);
				}

				return result;
			}

			const Matrix Matrix::operator+() const
			{
				return *this;
			}

			const Matrix Matrix::operator-() const
			{
				Matrix result(row, column, Array_2d<double>::uninit_tag);
				for (size_t i = 0; i != row; ++i) {
					for (size_t j = 0; j != column; ++j) {
						new (result.p[i] + j) double(-p[i][j]);
					}
				}
				return result;
			}

			Matrix& Matrix::operator+=(const Matrix &with)
			{
				if (row != with.row) {
					throw std::invalid_argument("error: row(A) ≠ row(B)");
				}
				if (column != with.column) {
					throw std::invalid_argument("error: column(A) ≠ column(B)");
				}

				for (size_t i = 0; i != row; ++i) {
					for (size_t j = 0; j != column; ++j) {
						p[i][j] += with.p[i][j];
					}
				}

				return *this;
			}

			Matrix& Matrix::operator-=(const Matrix &with)
			{
				if (row != with.row) {
					throw std::invalid_argument("error: row(A) ≠ row(B)");
				}
				if (column != with.column) {
					throw std::invalid_argument("error: column(A) ≠ column(B)");
				}

				for (size_t i = 0; i != row; ++i) {
					for (size_t j = 0; j != column; ++j) {
						p[i][j] -= with.p[i][j];
					}
				}

				return *this;
			}

			Matrix& Matrix::operator*=(double k) throw ()
			{
				for (size_t i = 0; i != row; ++i) {
					for (size_t j = 0; j != column; ++j) {
						p[i][j] *= k;
					}
				}

				return *this;
			}

			Matrix& Matrix::operator=(const Matrix &src)
			{
				//cout << this << " = " << &src << endl;
				if (row == src.row) {
					if (column == src.column) { //行,列数都与原来相等
						if (this == &src) { //自己给自己赋值
							return *this;
						}
						const size_t size_of_a_row = src.column * sizeof(double);
						for (size_t i = 0; i != src.row; ++i) {
							memcpy(p[i], src.p[i], size_of_a_row);
						}
					} else { //行数与原来相等, 列数不等

						this->column = src.column;
						const size_t size_of_a_row = column * sizeof(double);
						for (size_t i = 0; i != row; ++i) {
							delete[] p[i];
							memcpy(p[i] = new double[column], src.p[i], size_of_a_row);
						}
					}
				} else {
					for (size_t i = 0; i != row; ++i)
						delete[] p[i];
					delete[] p;

					this->row = src.row;
					this->column = src.column;

					p = new double*[row]; //开辟行
					const size_t size_of_a_row = column * sizeof(double);
					for (size_t i = 0; i != row; ++i) {
						memcpy(p[i] = new double[column], src.p[i], size_of_a_row);
					}
				}

				return *this;
			}

#if __cplusplus >= 201103L //C++0x
			const Matrix& Matrix::operator=(Matrix &&src)
			{ //转移赋值运算符

				this->clear();
				row = src.row;
				column = src.column;
				p = src.p;

				src.p = NULL;
				src.row = 0;
				src.column = 0;
				return *this;
			}
#endif

			const Matrix Matrix::sub_of(size_t up, size_t down, size_t left, size_t right) const
					throw (std::invalid_argument, std::out_of_range)
			{
				if (up >= down || left >= right) {
					throw std::invalid_argument("up >= down or left >= right");
				}

				this->test_row(up);
				this->test_row(down - 1);
				this->test_column(left);
				this->test_column(right - 1);

				Matrix result(down - up, right - left, Array_2d<double>::uninit_tag);
				for (size_t i = up; i < down; ++i) {
					std::uninitialized_copy(p[i] + left, p[i] + right, result.p[i - up]);
				}
				return result;
			}

			const Matrix pow(const Matrix &A, const int n)
			{
				return A ^ n;
			}

			double Matrix::tr() const
			{ //返回方阵的迹
				this->test_square();
				double result = 0;
				for (size_t i = 0; i < this->row; ++i) {
					result += this->p[i][i];
				}
				return result;
			}

			void Matrix::test_row(size_t row_test) const throw (std::out_of_range)
			{
#			if __cplusplus >= 201103L
				using std::to_string;
#			else
				using kerbal::string_serve::to_string;
#			endif

				if (row_test >= this->row) {
					throw std::out_of_range(
							"The " + to_string(this->row) + " × " + to_string(this->column)
									+ " Matrix doesn't have the no." + to_string(row_test)
									+ " row!");
				}
			}

			void Matrix::test_column(size_t column_test) const throw (std::out_of_range)
			{
#			if __cplusplus >= 201103L
				using std::to_string;
#			else
				using kerbal::string_serve::to_string;
#			endif

				if (column_test >= this->column) {
					throw std::out_of_range(
							"The " + to_string(this->row) + " × " + to_string(this->column)
									+ " Matrix doesn't have the no." + to_string(column_test)
									+ " column!");
				}
			}

			void Matrix::test_square() const throw (std::invalid_argument)
			{
				if (row != column) {
					throw std::invalid_argument("A is not a square matrix");
				}
			}

			const Matrix conv2(const Matrix &core, const Matrix &A, int size)
			{ //矩阵卷积
				size_t i, j, m, n;
				if (size == 2) {
					Matrix zsmall(A.row - core.row + 1, A.column - core.column + 1, 0);
					for (i = 0; i < zsmall.row; ++i) {
						for (m = i; m < i + core.row; ++m) {
							for (j = 0; j < zsmall.column; ++j) {
								for (n = j; n < j + core.column; ++n) {
									zsmall.p[i][j] += A.p[m][n] * core.p[m - i][n - j];
									std::cout << A.p[m][n] << "*" << core.p[m - i][n - j] << "   ";
								}
								std::cout << std::endl;
							}
						}
					}
					zsmall.print();
					return zsmall;
				}

				Matrix z(A.row + core.row - 1, A.column + core.column - 1, 0);
				for (m = 0; m < A.row; ++m) {
					for (i = m; i < m + core.row && i < z.row; ++i) {
						for (n = 0; n < A.column; ++n) {
							for (j = n; j < n + core.column && j < z.column; ++j) {
								z.p[i][j] += A.p[m][n] * core.p[i - m][j - n];
							}
						}
					}
				}

				if (size == 1) {

					Matrix z2 = z.sub_of((core.row - 1) / 2, A.row + (core.row - 1) / 2,
							(core.column - 1) / 2, A.column + (core.column - 1) / 2);

					return z2;
				} else {
					return z;
				}
			}

			template <>
			const Matrix conv_2d<max>(const Matrix &core, const Matrix &A)
			{
				//矩阵卷积
				std::cout << "max" << std::endl;
				return Matrix(1ull, 1ull);
			}

			template <>
			const Matrix conv_2d<mid>(const Matrix &core, const Matrix &A)
			{
				//矩阵卷积
				std::cout << "mid" << std::endl;
				return Matrix(1ull, 1ull);
			}

			template <>
			const Matrix conv_2d<small>(const Matrix &core, const Matrix &A)
			{
				//矩阵卷积
				std::cout << "small" << std::endl;
				return Matrix(1ull, 1ull);
			}

//计算
			bool Matcmp(const Matrix &A, const Matrix &B, double eps)
			{
				for (size_t i = 0; i != A.row; ++i) {
					for (size_t j = 0; j != A.column; ++j) {
						if (fabs(A.p[i][j] - B.p[i][j]) > eps) {
							return false;
						}
					}
				}
				return true;
			}

			const Matrix transpose_of(const Matrix &A)
			{ //返回矩阵A的转置矩阵

				Matrix result(A.column, A.row);
				for (size_t i = 0; i != result.row; ++i) {
					for (size_t j = 0; j != result.column; ++j) {
						result.p[i][j] = A.p[j][i];
					}
				}
				return result;
			}

			const Matrix Matrix::cofactor_of(size_t row_tar, size_t column_tar) const
					throw (std::out_of_range)
			{ //返回一个矩阵划去row_tar 行和 column_tar 列后的矩阵

				test_row(row_tar);
				test_column(column_tar);

				Matrix result(row - 1, column - 1);

				for (size_t i = 0; i != row_tar; ++i) {
					std::copy(this->p[i], this->p[i] + column_tar, result.p[i]);
					std::copy(this->p[i] + column_tar + 1, this->p[i] + this->column,
							result.p[i] + column_tar);
				}

				for (size_t i = row_tar + 1; i != row; ++i) {
					std::copy(this->p[i], this->p[i] + column_tar, result.p[i - 1]);
					std::copy(this->p[i] + column_tar + 1, this->p[i] + this->column,
							result.p[i - 1] + column_tar);
				}

				return result;
			}

			void Matrix::do_cofactor(size_t row_tar, size_t column_tar) throw (std::out_of_range)
			{ //返回一个矩阵划去row_tar 行和 column_tar 列后的矩阵
				*this = cofactor_of(row_tar, column_tar);
			}

//应用部分

			const Matrix rotate_X(double sigma)
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double a[3][3] = { 1, 0, 0, 0, cosine, -sine, 0, sine, cosine };
				return Matrix(a);
			}

			const Matrix rotate_Y(double sigma)
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double a[3][3] = { cosine, 0, sine, 0, 1, 0, -sine, 0, cosine };
				return Matrix(a);
			}

			const Matrix rotate_Z(double sigma)
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double a[3][3] = { cosine, -sine, 0, sine, cosine, 0, 0, 0, 1 };
				return Matrix(a);
			}

			void rotate_X(double sigma, const double& x0, double& y0, double& z0) throw ()
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double y1 = y0 * cosine - z0 * sine;
				double z1 = y0 * sine + z0 * cosine;

				y0 = y1;
				z0 = z1;
			}

			void rotate_Y(double sigma, double& x0, const double& y0, double& z0) throw ()
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double x1 = x0 * cosine + z0 * sine;
				double z1 = -x0 * sine + z0 * cosine;

				x0 = x1;
				z0 = z1;
			}

			void rotate_Z(double sigma, double& x0, double& y0, const double& z0) throw ()
			{
				double cosine = cos(sigma);
				double sine = sin(sigma);
				double x1 = x0 * cosine - y0 * sine;
				double y1 = x0 * sine + y0 * cosine;

				x0 = x1;
				y0 = y1;
			}

		}/* namespace matrix */

	}/* namespace math */

} /* namespace kerbal */

