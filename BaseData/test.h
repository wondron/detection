#pragma once 

typedef unsigned long Item;

//#include <iostream>
//#include <cstdlib>
//
//
////指定第二个参数的类型是int，不会随着第一个参数的变化而变化。
//template <class T, int n>
//class ArrayTP
//{
//private:
//	T ar[n];
//public:
//	ArrayTP() {}
//	explicit ArrayTP(const T& v);
//	//对值进行修改，a[2] = 2;
//	virtual T& operator[](int i);
//
//	//值读取内容，不修改值。 b = a[2];
//	virtual T operator[](int i) const;
//};
//
//template<class T, int n>
//ArrayTP<T, n>::ArrayTP(const T& v) {
//	for (int i = 0; i < n; i++)
//		ar[i] = v;
//}
//
//template<class T, int n>
//T& ArrayTP<T, n>::operator[](int i) {
//	if (i < 0 || i >= n) {
//		std::cerr << "Error in array limits: " << i << "is out of range\n";
//		std::exit(EXIT_FAILURE);
//	}
//
//	return ar[i];
//}
//
//template<class T, int n>
//T ArrayTP<T, n>::operator[](int i) const
//{
//	if (i < 0 || i >= n) {
//		std::cerr << "Error in array limits: " << i << "is out of range\n";
//		std::exit(EXIT_FAILURE);
//	}
//
//	return ar[i];
//}
//
//template <typename T>
//class Array
//{
//private:
//	T entry;
//};
//
//template <typename Type>
//class GrowArray : public Array<Type> {};
//
//template <typename Tp>
//class Stack
//{
//	Array<tp> ar;
//};