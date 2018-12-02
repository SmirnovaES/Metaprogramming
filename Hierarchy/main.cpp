#include <iostream>

//Typelist
////////////////////////////////////////////////////

class NullType {};

template<typename T=NullType, typename ...U>
struct TypeList {
  using head = T;
  using tail = TypeList<U ...>;
};
template<typename T>
struct TypeList<T> {
  using head = T;
  using tail = NullType;
};

using EmptyList = TypeList<>;

//SkipFirstN
////////////////////////////////////////////////////

template<int num, typename T, typename ...U>
struct SkipFirstN;

template<int num, typename T, typename U>
struct SkipFirstN <num, TypeList<T, U>> {
  using result = typename SkipFirstN<num - 1, TypeList<U>>::result;
};

template <typename T, typename U>
struct SkipFirstN <0, TypeList<T, U> > {
  using result = TypeList<T, U>;
};

//Append
////////////////////////////////////////////////////

template <typename TL1, typename TL2>
struct Append;

template <typename TL>
struct Append<EmptyList, TypeList<TL>> {
  using result = TypeList<TL>;
};

template <typename TL1, typename TL2>
struct Append<TypeList<TL1>, TypeList<TL2>> {
  using result = TypeList<TL1, TL2>;
};


//GetFirstN
////////////////////////////////////////////////////

template<int num, typename T, typename ...U >
struct GetFirstN;

template <typename T, typename U>
struct GetFirstN<0, TypeList<T, U> > {
  using result = EmptyList;
};

template<int num, typename T, typename U>
struct GetFirstN<num, TypeList<T, U>> {
  using result = typename Append<TypeList<T>, typename GetFirstN<num - 1, TypeList<U>>::value>::result;
};


//GenLinearHierarchy
////////////////////////////////////////////////////

template <class TList, template <class, class> class Unit, class Root = NullType>
struct GenLinearHierarchy;

template <class T1, class T2, template <class, class> class Unit, class Root>
struct GenLinearHierarchy<TypeList<T1, T2>, Unit, Root> :
public Unit<T1, GenLinearHierarchy<TypeList<T2>, Unit, Root>> {};

template <class AtomicType, template <class, class> class Unit, class Root>
struct GenLinearHierarchy<TypeList<AtomicType>, Unit, Root> :
public Unit<AtomicType, Root> {};


//FibNumber
////////////////////////////////////////////////////

template <unsigned int num>
struct FibNumber {
  enum { value = FibNumber<num - 1>::value + FibNumber<num - 2>::value };
};

template <>
struct FibNumber<0> {
  enum { value = 1 };
};

template <>
struct FibNumber<1> {
  enum { value = 1 };
};

//GenFibHierarchy
////////////////////////////////////////////////////

template <class T, template <class , class> class Unit, unsigned int num = 0>
struct GenFibHierarchy;

template <class T, template <class , class > class Unit, unsigned int num>
struct GenFibHierarchy: public GenLinearHierarchy<typename GetFirstN<FibNumber<num>::value, T>::result , Unit>,
    public GenFibHierarchy<typename SkipFirstN<num, T>::result, Unit, num + 1> {};

template <class T, template <class, class> class Unit>
struct GenFibHierarchy<T, Unit, 0> {};

//test
////////////////////////////////////////////////////

template <class T, class E>
struct Unit: public E{
  T value;
};

using TL1 = TypeList<int, char, bool, long>;

int main() {
    TL1 t;
    GenFibHierarchy<TL1, Unit> e;
    std::cout << FibNumber<4>::value << std::endl;

    return 0;
}