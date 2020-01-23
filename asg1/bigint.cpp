// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

// constructors -----------------------------------------

// constructor #1 makes a big int from a long
// calls the ubigint long constructor
bigint::bigint (long that): uvalue (ubigint (that)),
is_negative (that < 0) {

}

// constructor #2 makes a big int from an unsigned big int and a sign
// called by the operators to make the result
bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {

}

// constructor #3 makes a big int from a string input
bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
   // calls the ubigint long constructor
}

// operators --------------------------------------------

// add it to nothing?
bigint bigint::operator+ () const {
   return *this;
}

// subtract it from nothing?
bigint bigint::operator- () const {
   return {uvalue, not is_negative};  // flip the sign
}

// regular add operation
bigint bigint::operator+ (const bigint& that) const {
   if (is_negative != that.is_negative) {
      if (uvalue < that.uvalue)
         return {that.uvalue - uvalue, that.is_negative};
      if (uvalue > that.uvalue)
         return {uvalue - that.uvalue, is_negative};
      return {uvalue - that.uvalue, false};
   }
   return {uvalue + that.uvalue, is_negative};
}

// regular subtract operation
bigint bigint::operator- (const bigint& that) const {
   if (is_negative == that.is_negative) {
      if (uvalue < that.uvalue)
         return {that.uvalue - uvalue, !that.is_negative};
      if (uvalue > that.uvalue)
         return {uvalue - that.uvalue, is_negative};
      return {uvalue - that.uvalue, false};
   }
   return {uvalue + that.uvalue, is_negative};
}

  // should work
bigint bigint::operator* (const bigint& that) const {
   ubigint result = uvalue * that.uvalue;
   if (result == ubigint(0))
      return {result, false};
   return {result, is_negative != that.is_negative};
}

// divides and rounds down
bigint bigint::operator/ (const bigint& that) const {
   ubigint result = uvalue / that.uvalue;
   if (result == ubigint(0))
      return {result, false};
   return {result, is_negative != that.is_negative};
}

bigint bigint::operator% (const bigint& that) const {
   ubigint result = uvalue % that.uvalue;
   return {result, false};
}

// working
bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

// working
bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

// prints out the number (adds the sign to the ubigint)
ostream& operator<< (ostream& out, const bigint& that)
{

   return out << (that.is_negative ? "-" : "") << that.uvalue;
}




