#ifndef Functors_h_
#define Functors_h_

//
// A variety of functors for use with histogram and ntuple wrappers.
// No matter what their actual type is, the values returned by operator()
// will be typically converted into either a double (histogram coordinates,
// histogram weights, ntuple values) or a boolean (ntuple selectors).
//
// Functors which implement "operator()(unsigned) const" but not
// "operator()() const" are intended for use with "Cycled" histograms
// and ntuples only. Functors which implement both of these methods
// can also be used with "Auto" histograms and ntuples.
//
// Functor templates are often split into a helper class and a convenience
// function which returns an instance of that class, similar to what
// the standard library does with std::pair and std::make_pair. The
// intent is that the user code should use the helper functions only.
//
// The following table summarizes usable functions and classes implemented
// in this file.
//
// Functor Name   Auto  Cycled  Purpose
// ------------   ----  ------  -------------------------------------------
//
// Double         yes    yes    Represents a double constant. Particularly
//                              useful for histogram weights. Example:
//                              Double(1).
//
// Boolean        yes    yes    Represents a boolean constant. Example:
//                              Boolean(1).
//
// Constant       yes    yes    Represents a constant of an arbitrary type,
//                              according to a given argument. For example,
//                              Constant(1) will represent an integer.
//
// UIntRatio      no     yes    Unsigned ratio of the cycle number and
//                              the divisor provided as the argument.
//                              Example: UIntRatio(2). The functor will
//                              return 0 for cycle numbers 0 and 1, 1 for
//                              cycle numbers 2 and 3, and so on.
//
// UIntRemainder  no     yes    Unsigned remainder of the cycle number.
//                              Example: UIntRatio(2).  The functor will
//                              return 0 for cycle numbers 0, 2, 4, ...
//                              and 1 for cycle numbers 1, 3, 5, ...
//
// ValueOf        yes    yes    Returns a value of some variable which
//                              must be declared elsewhere. Example:
//                              ValueOf(x_) will return the value of
//                              variable x_. Use of this functor in the
//                              "cycled" mode is not normally expected.
//
// ValueEQ        yes    yes    Check whether a value of some variable
//                              is equal to a predefined constant.
//                              Example: ValueEQ(x_, 10.0). Use of this
//                              functor in the "cycled" mode is not normally
//                              expected.
//
// ValueLT        yes    yes    Check whether a value of some variable
//                              is less than a predefined constant.
//                              Example: ValueLT(x_, 10.0).
//
// ValueGT        yes    yes    Check whether a value of some variable
//                              is greater than a predefined constant.
//                              Example: ValueGT(x_, 10.0).
//
// ValueLE        yes    yes    Just at ValueLT, but this functor also
//                              returns "true" when the variable is equal
//                              to the predefined constant.
// 
// ValueGE        yes    yes    Just at ValueGT, but this functor also
//                              returns "true" when the variable is equal
//                              to the predefined constant.
// 
// CycleNumber    no     yes    Returns the cycle number.
//                              Example: CycleNumber().
//
// ElementOf      no     yes    Returns the element of the given array or
//                              std::vector with the argument index, possibly
//                              multiplied by a stride. Example: ElementOf[x_]
//                              will return x_[i] when the functor is called
//                              with argument i. ElementOf[x_, 3] will return
//                              x_[3*i].
//
// ElementEQ      no     yes    Checks if an element of the given array
//                              equals to a predefined constant, possibly
//                              multiplying the index by a stride.
//                              Example: ElementEQ(x_, 10.0) will return
//                              x_[i] == 10.0 when called with argument i.
//                              Example: ElementEQ(x_, 10.0, 3) will
//                              instead return x_[3*i] == 10.0.
//
// ElementLT      no     yes    All these functors are similar to ElementEQ.
// ElementLE                    Instead of operator == they use operators
// ElementGT                    <, <=, >, and >=, respectively.
// ElementGE
//
// Method         yes    yes    Will call a member function of the given
//                              object. That member function must have either
//                              no arguments or one unsigned argument (for use
//                              in the "cycled" mode).
//                              Example: Method(&A::get, a_).
//
// ElementMethod  no     yes    Will call a member function of the object with
//                              the given index in an array. This member
//                              function must be callable without any arguments.
//                              For example, if objects of class A are stored
//                              in the array a_ then ElementMethod(&A::get, a_)
//                              will return a_[i].get() when called with
//                              argument i. It is also possible to specify
//                              a stride, as in ElementMethod(&A::get, a_, 3).
//
// ElementMember  no     yes    Will return a public member of the object with
//                              the given index in an array. If, for example,
//                              we have an array p_ of std::pair objects then
//                              ElementMember(p_, &p_->first) will return
//                              p_[i].first when called with argument i. It is
//                              also possible to specify a stride.
//
// Apply          yes    no     Applies a c-style function to the given
//                              variable and returns the value of that function.
//                              Example: Apply(cos, x_) will return cos(x_)
//                              when called. Apply(atan2, y_, x_) will return
//                              atan2(y_, x_);
//
// ApplyToElement no     yes    Applies a given c-style function to an array
//                              element, possibly calculated with a stride.
//                              Example: ApplyToElement(sin, x_) will calculate
//                              sin(x_[i]) when called with argument i.
//                              ApplyToElement(sin, x_, 3) will instead
//                              calculate sin(x_[3*i]).
//
// And, Or, Not   yes    yes    Logical operations on other functors (usually,
//                              for use as selectors).
//
// Naturally, other functors can be developed in a similar manner whenever
// additional functionality is needed.
//
// I. Volobouev
// March 2013
//

#include <cassert>
#include <vector>


class Double
{
public:
    inline Double(const double v) : value_(v) {}
    inline double operator()() const {return value_;}
    inline double operator()(unsigned) const {return value_;}

private:
    Double();
    double value_;
};

//======================================================================

class Boolean
{
public:
    inline Boolean(const bool v) : value_(v) {}
    inline bool operator()() const {return value_;}
    inline bool operator()(unsigned) const {return value_;}

private:
    Boolean();
    bool value_;
};

//======================================================================

template<class T>
class ConstantHlp
{
public:
    inline ConstantHlp(const T& v) : value_(v) {}
    inline const T& operator()() const {return value_;}
    inline const T& operator()(unsigned) const {return value_;}

private:
    ConstantHlp();
    T value_;
};

template<typename T>
inline ConstantHlp<T> Constant(const T& t)
{
    return ConstantHlp<T>(t);
}

//======================================================================

class UIntRatio
{
public:
    inline UIntRatio(const unsigned divisor) : divisor_(divisor) {}

    inline unsigned operator()(const unsigned n) const
        {return n/divisor_;}

private:
    unsigned divisor_;
};

//======================================================================

class UIntRemainder
{
public:
    inline UIntRemainder(const unsigned divisor) : divisor_(divisor) {}

    inline unsigned operator()(const unsigned n) const
        {return n%divisor_;}

private:
    unsigned divisor_;
};

//======================================================================

template<typename T>
class ValueOfHlp
{
public:
    inline ValueOfHlp(T& t) : ptr_(&t) {}
    inline const T& operator()() const {return *ptr_;}
    inline const T& operator()(unsigned) const {return *ptr_;}

private:
    T* ptr_;
};

template<typename T>
inline ValueOfHlp<T> ValueOf(T& t)
{
    return ValueOfHlp<T>(t);
}

//======================================================================

template<typename T>
class ValueEQHlp
{
public:
    inline ValueEQHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ == value_;}
    inline bool operator()(unsigned) const {return *ptr_ == value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueEQHlp<T> ValueEQ(T& t, const T& v)
{
    return ValueEQHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueLTHlp
{
public:
    inline ValueLTHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ < value_;}
    inline bool operator()(unsigned) const {return *ptr_ < value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueLTHlp<T> ValueLT(T& t, const T& v)
{
    return ValueLTHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueLEHlp
{
public:
    inline ValueLEHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ <= value_;}
    inline bool operator()(unsigned) const {return *ptr_ <= value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueLEHlp<T> ValueLE(T& t, const T& v)
{
    return ValueLEHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueGTHlp
{
public:
    inline ValueGTHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ > value_;}
    inline bool operator()(unsigned) const {return *ptr_ > value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueGTHlp<T> ValueGT(T& t, const T& v)
{
    return ValueGTHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueGEHlp
{
public:
    inline ValueGEHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ >= value_;}
    inline bool operator()(unsigned) const {return *ptr_ >= value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueGEHlp<T> ValueGE(T& t, const T& v)
{
    return ValueGEHlp<T>(t, v);
}

//======================================================================

struct CycleNumber
{
    inline unsigned operator()(const unsigned i) const {return i;}
};

//======================================================================

template<typename T>
class ElementOfHlp
{
public:
    inline ElementOfHlp(T* t, const unsigned stride)
        : ptr_(t), stride_(stride) {assert(ptr_);}
    inline const T& operator()(const unsigned i) const
        {return ptr_[i*stride_];}

private:
    T* ptr_;
    unsigned stride_;
};

template<typename T>
inline ElementOfHlp<T> ElementOf(T* t, unsigned stride=1)
{
    return ElementOfHlp<T>(t, stride);
}

//======================================================================

template<typename T>
class ElementOfHlpVec
{
public:
    inline ElementOfHlpVec(std::vector<T>& t, const unsigned stride)
        : ptr_(t), stride_(stride) {}
    inline const T& operator()(const unsigned i) const
        {return ptr_[i*stride_];}

private:
    std::vector<T>& ptr_;
    unsigned stride_;
};

template<typename T>
inline ElementOfHlpVec<T> ElementOf(std::vector<T>& t, unsigned stride=1)
{
    return ElementOfHlpVec<T>(t, stride);
}

//======================================================================

template<typename T>
class ElementEQHlp
{
public:
    inline ElementEQHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] == value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementEQHlp<T> ElementEQ(T* t, const T& value, unsigned stride=1)
{
    return ElementEQHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementLTHlp
{
public:
    inline ElementLTHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] < value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementLTHlp<T> ElementLT(T* t, const T& value, unsigned stride=1)
{
    return ElementLTHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementLEHlp
{
public:
    inline ElementLEHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] <= value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementLEHlp<T> ElementLE(T* t, const T& value, unsigned stride=1)
{
    return ElementLEHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementGTHlp
{
public:
    inline ElementGTHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] > value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementGTHlp<T> ElementGT(T* t, const T& value, unsigned stride=1)
{
    return ElementGTHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementGEHlp
{
public:
    inline ElementGEHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] >= value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementGEHlp<T> ElementGE(T* t, const T& value, unsigned stride=1)
{
    return ElementGEHlp<T>(t, value, stride);
}

//======================================================================

template<typename Result, typename T>
class ApplyToElementHlp
{
public:
    typedef Result (*CFunc)(const T&);

    inline ApplyToElementHlp(CFunc f, T* t, const unsigned stride)
        : fcn_(f), ptr_(t), stride_(stride) {assert(ptr_);}

    inline Result operator()(const unsigned i) const
        {return fcn_(ptr_[i*stride_]);}

private:
    CFunc fcn_;
    T* ptr_;
    unsigned stride_;
};

template<typename Result, typename T>
inline ApplyToElementHlp<Result, T> ApplyToElement(
    Result (*f)(const T&), T* ptr, unsigned stride=1)
{
    return ApplyToElementHlp<Result, T>(f, ptr, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberFcnHlp0
{
public:
    typedef Result (T::*MemFunc)();

    inline ElementMemberFcnHlp0(MemFunc f, T* obj, const unsigned stride)
        : f_(f), ptr_(obj), stride_(stride) {assert(ptr_);}

    inline Result operator()(const unsigned i) const
        {return ((ptr_ + i*stride_)->*f_)();}

private:
    MemFunc f_;
    T* ptr_;
    unsigned stride_;
};

template<typename Result, typename T>
inline ElementMemberFcnHlp0<Result,T> ElementMethod(
    Result (T::*f)(), T* ptr, unsigned stride=1)
{
    return ElementMemberFcnHlp0<Result,T>(f, ptr, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberFcnHlp0Const
{
public:
    typedef Result (T::*MemFunc)() const;

    inline ElementMemberFcnHlp0Const(MemFunc f, const T* obj,
                                     const unsigned stride)
        : f_(f), ptr_(obj), stride_(stride) {assert(ptr_);}

    inline Result operator()(const unsigned i) const
        {return ((ptr_ + i*stride_)->*f_)();}

private:
    MemFunc f_;
    const T* ptr_;
    unsigned stride_;
};

template<typename Result, typename T>
inline ElementMemberFcnHlp0Const<Result,T> ElementMethod(
    Result (T::*f)() const, const T* ptr, unsigned stride=1)
{
    return ElementMemberFcnHlp0Const<Result,T>(f, ptr, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberHlp
{
public:
    inline ElementMemberHlp(const T* arr, const unsigned long offset,
                            const unsigned stride)
        : ptr_(arr), offset_(offset), stride_(stride) {}

    inline Result operator()(const unsigned i) const
    {
        return *(reinterpret_cast<const Result*>(
                 reinterpret_cast<const char*>(ptr_+i*stride_)+offset_));
    }

private:
    const T* ptr_;
    unsigned long offset_;
    unsigned stride_;
};


template<typename Result, typename T>
inline ElementMemberHlp<Result,T> ElementMember(
    const T* base, const Result* element, unsigned stride=1)
{
    assert(base);
    assert(element);
    long off = reinterpret_cast<const char*>(element) - 
               reinterpret_cast<const char*>(base);
    assert(off >= 0L);
    return ElementMemberHlp<Result,T>(base, off, stride);
}

//======================================================================

template<typename T>
class CMathFcnHlp
{
public:
    typedef double (*CFunc)(double);

    inline CMathFcnHlp(CFunc f, T& t1) : fcn_(f), ptr1_(&t1) {}
    inline double operator()() const {return fcn_(*ptr1_);}

private:
    CFunc fcn_;
    T* ptr1_;
};

template<typename T>
inline CMathFcnHlp<T> Apply(double (*f)(double), T& t)
{
    return CMathFcnHlp<T>(f, t);
}

//======================================================================

template<typename T>
class RefFcnHlp
{
public:
    typedef double (*CFunc)(const T&);

    inline RefFcnHlp(CFunc f, const T& t1) : fcn_(f), ptr1_(&t1) {}
    inline double operator()() const {return fcn_(*ptr1_);}

private:
    CFunc fcn_;
    const T* ptr1_;
};

template<typename T>
inline RefFcnHlp<T> Apply(double (*f)(const T&), const T& t)
{
    return RefFcnHlp<T>(f, t);
}

//======================================================================

template<typename T>
class BinaryFcnHlp
{
public:
    typedef double (*CFunc)(double, double);

    inline BinaryFcnHlp(CFunc f, T& t1, T& t2)
        : fcn_(f), ptr1_(&t1), ptr2_(&t2) {}
    inline double operator()() const {return fcn_(*ptr1_, *ptr2_);}

private:
    CFunc fcn_;
    T* ptr1_;
    T* ptr2_;
};


template<typename T>
inline BinaryFcnHlp<T> Apply(double (*f)(double, double), T& t1, T& t2)
{
    return BinaryFcnHlp<T>(f, t1, t2);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp0
{
public:
    typedef Result (T::*MemFunc)();

    inline MemberFcnHlp0(MemFunc f, T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()() const {return (ptr_->*f_)();}
    inline Result operator()(unsigned) const {return (ptr_->*f_)();}

private:
    MemFunc f_;
    T* ptr_;
};

template<typename Result, typename T, typename T2>
inline MemberFcnHlp0<Result,T> Method(Result (T::*f)(), T2* ptr)
{
    return MemberFcnHlp0<Result,T>(f, dynamic_cast<T*>(ptr));
}

template<typename Result, typename T>
inline MemberFcnHlp0<Result,T> Method(Result (T::*f)(), T& ref)
{
    return MemberFcnHlp0<Result,T>(f, &ref);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp0Const
{
public:
    typedef Result (T::*MemFunc)() const;

    inline MemberFcnHlp0Const(MemFunc f, const T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()() const {return (ptr_->*f_)();}
    inline Result operator()(unsigned) const {return (ptr_->*f_)();}

private:
    MemFunc f_;
    const T* ptr_;
};

template<typename Result, typename T, typename T2>
inline MemberFcnHlp0Const<Result,T> Method(Result (T::*f)() const,
                                           const T2* ptr)
{
    return MemberFcnHlp0Const<Result,T>(f, dynamic_cast<const T*>(ptr));
}

template<typename Result, typename T>
inline MemberFcnHlp0Const<Result,T> Method(Result (T::*f)() const,
                                           const T& ref)
{
    return MemberFcnHlp0Const<Result,T>(f, &ref);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp1
{
public:
    typedef Result (T::*MemFunc)(unsigned);

    inline MemberFcnHlp1(MemFunc f, T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()(unsigned n) const {return (ptr_->*f_)(n);}

private:
    MemFunc f_;
    T* ptr_;
};

template<typename Result, typename T, typename T2>
inline MemberFcnHlp1<Result,T> Method(Result (T::*f)(unsigned), T2* ptr)
{
    return MemberFcnHlp1<Result,T>(f, dynamic_cast<T*>(ptr));
}

template<typename Result, typename T>
inline MemberFcnHlp1<Result,T> Method(Result (T::*f)(unsigned), T& ref)
{
    return MemberFcnHlp1<Result,T>(f, &ref);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp1Const
{
public:
    typedef Result (T::*MemFunc)(unsigned) const;

    inline MemberFcnHlp1Const(MemFunc f, const T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()(unsigned n) const {return (ptr_->*f_)(n);}

private:
    MemFunc f_;
    const T* ptr_;
};

template<typename Result, typename T, typename T2>
inline MemberFcnHlp1Const<Result,T> Method(Result (T::*f)(unsigned) const,
                                           const T2* ptr)
{
    return MemberFcnHlp1Const<Result,T>(f, dynamic_cast<const T*>(ptr));
}

template<typename Result, typename T>
inline MemberFcnHlp1Const<Result,T> Method(Result (T::*f)(unsigned) const,
                                           const T& ref)
{
    return MemberFcnHlp1Const<Result,T>(f, &ref);
}

//======================================================================

template<class Functor>
class NotHlp
{
public:
    inline explicit NotHlp(const Functor& f) : f_(f) {}
    inline bool operator()() const {return !(f_());}
    inline bool operator()(const unsigned i) const {return !(f_(i));}

private:
    Functor f_;
};

template<typename T>
inline NotHlp<T> Not(const T& t)
{
    return NotHlp<T>(t);
}

//======================================================================

template<class Functor1, class Functor2>
class AndHlp
{
public:
    inline AndHlp(const Functor1& f1, const Functor2& f2) : f1_(f1), f2_(f2) {}
    inline bool operator()() const {return f1_() && f2_();}
    inline bool operator()(const unsigned i) const {return f1_(i) && f2_(i);}

private:
    Functor1 f1_;
    Functor2 f2_;
};

template<class Functor1, class Functor2>
inline AndHlp<Functor1,Functor2> And(const Functor1& f1, const Functor2& f2)
{
    return AndHlp<Functor1,Functor2>(f1, f2);
}

//======================================================================

template<class Functor1, class Functor2>
class OrHlp
{
public:
    inline OrHlp(const Functor1& f1, const Functor2& f2) : f1_(f1), f2_(f2) {}
    inline bool operator()() const {return f1_() && f2_();}
    inline bool operator()(const unsigned i) const {return f1_(i) && f2_(i);}

private:
    Functor1 f1_;
    Functor2 f2_;
};

template<class Functor1, class Functor2>
inline OrHlp<Functor1,Functor2> Or(const Functor1& f1, const Functor2& f2)
{
    return OrHlp<Functor1,Functor2>(f1, f2);
}

#endif // Functors_h_
