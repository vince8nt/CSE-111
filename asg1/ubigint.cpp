// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cmath>  // used for constructor #1
using namespace std;

#include "ubigint.h"
#include "debug.h"

// constructor #1 makes unsigned big int from a long input
ubigint::ubigint (unsigned long that): ubig_value (0) {
   long decPlace;
   long digit;
   for (decPlace = log(that); decPlace >= 0; decPlace--) {
      digit = (that / pow(10, decPlace));
      digit = digit % 10;
      ubig_value.push_back(static_cast<char>(digit));
   }
}

// constructor #2 makes unsigned big int from a string input
ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(static_cast<char>(digit - '0'));
      // so ascii "0" -> int 0
   }

   // remove excess 0's
   while(ubig_value.size() > 0 && *ubig_value.cbegin() == 0)
      ubig_value.erase(ubig_value.cbegin());
}

// add operation
ubigint ubigint::operator+ (const ubigint& that) const {

   // main operation
   ubigint result = ubigint();
   auto numA = ubig_value.cend();
   auto numB = that.ubig_value.cend();
   char carryover = 0;
   while(numA != ubig_value.cbegin()
      && numB != that.ubig_value.cbegin())
   {
      --numA;
      --numB;
      carryover += (*numA + *numB);
      result.ubig_value.insert(
         result.ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }
   while(numA != ubig_value.cbegin())
   {
      --numA;
      carryover += *numA;
      result.ubig_value.insert(
         result.ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }
   while(numB != that.ubig_value.cbegin())
   {
      --numB;
      carryover += *numB;
      result.ubig_value.insert(
         result.ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }
   while(carryover != 0)
   {
      result.ubig_value.insert(
         result.ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }
   return result;
}

// subtract operation
ubigint ubigint::operator- (const ubigint& that) const {

   ubigint result = ubigint();
   auto numA = ubig_value.cend();
   auto numB = that.ubig_value.cend();
   char carryover = 0;

   // main operation
   while(numB != that.ubig_value.cbegin())
   {
      --numA;
      --numB;
      carryover += (*numA - *numB);
      result.ubig_value.insert(
         result.ubig_value.cbegin(),
         (carryover + 10) % 10);
      if (carryover < 0) carryover = -1;
      else carryover = 0;
   }
   while(numA != ubig_value.cbegin())
   {
      --numA;
      carryover += *numA;
      result.ubig_value.insert(
         result.ubig_value.cbegin(),
         (carryover + 10) % 10);
      if (carryover < 0) carryover = -1;
      else carryover = 0;
   }
   while(carryover != 0)
   {
      result.ubig_value.insert(
         result.ubig_value.cbegin(),
         (carryover + 10) % 10);
      if (carryover < 0) carryover = -1;
      else carryover = 0;
   }
   // remove excess 0's
   while(result.ubig_value.size() > 0
      && *result.ubig_value.cbegin() == 0)
      result.ubig_value.erase(result.ubig_value.cbegin());

   return result;
}

// multiply operation
ubigint ubigint::operator* (const ubigint& that) const
{
   ubigint result = ubigint();
   int digA, digB;
   auto numA = ubig_value.cend();
   auto numB = that.ubig_value.cend();
   int length = ubig_value.size() + that.ubig_value.size() - 1;
   std::vector<int> temp (length);  // single node may exceed 255

   // set each digit (can be very big)
   digA = ubig_value.size();
   numA = ubig_value.cend();
   while(digA > 0)
   {
      digA--;
      numA--;

      digB = that.ubig_value.size();
      numB = that.ubig_value.cend();
      while(digB > 0)
      {
         digB--;
         numB--;

         temp.at(digA + digB) += (*numA) * (*numB);
      }
   }

   // carryover to make each digit {0, 9}
   int carryover = 0;
   for (int digTemp = length - 1; digTemp >=0; --digTemp)
   {
      carryover += temp.at(digTemp);
      temp.at(digTemp) = carryover % 10;
      carryover /= 10;
   }
   while(carryover != 0)
   {
      result.ubig_value.insert(
         result.ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }

   // set result to char representation of temp
   for (auto dig = temp.cbegin(); dig < temp.cend(); ++dig)
      result.ubig_value.push_back(static_cast<unsigned char>(*dig));

   // remove excess 0's
   while(result.ubig_value.size() > 0
      && *result.ubig_value.cbegin() == 0)
      result.ubig_value.erase(result.ubig_value.cbegin());

   return result;
}

// multiply self by 2
void ubigint::multiply_by_2() {

   int carryover = 0;
   auto digit = ubig_value.cend();
   while(digit != ubig_value.cbegin())
   {
      --digit;
      carryover += *digit * 2;
      ubig_value.at(digit - ubig_value.cbegin()) = carryover % 10;
      carryover /= 10;
   }
   while(carryover != 0)
   {
      ubig_value.insert(
         ubig_value.cbegin(), carryover % 10);
      carryover /= 10;
   }

}

// divide self by 2 (and round down) - doesn't work
void ubigint::divide_by_2() {

   int carryover = 0;
   auto digit = ubig_value.cbegin();
   while(digit != ubig_value.cend())
   {
      carryover += *digit;
      ubig_value.at(digit - ubig_value.cbegin()) = carryover / 2;
      carryover *= 5;
      carryover %= 10;
      carryover *= 2;
      ++digit;
   }

   // remove excess 0's
   while(ubig_value.size() > 0 && *ubig_value.cbegin() == 0)
      ubig_value.erase(ubig_value.cbegin());

}

// divide operation
ubigint ubigint::operator/ (const ubigint& that) const {
   ubigint zero = ubigint("0");
   if (that == zero) throw domain_error ("operator/ by zero");
   ubigint powerof2 = ubigint("1");
   ubigint divisor_ = ubigint("0");
   for (auto dig = that.ubig_value.cbegin();
      dig < that.ubig_value.cend(); ++dig)
      divisor_.ubig_value.push_back(*dig);

   while (divisor_.operator<(*this))
   {
      divisor_.multiply_by_2();
      powerof2.multiply_by_2();
   }

   ubigint quotient = ubigint("0");
   ubigint remainder = ubigint("0");
   for (auto dig = ubig_value.cbegin(); dig < ubig_value.cend(); ++dig)
      remainder.ubig_value.push_back(*dig);

   while (zero.operator<(powerof2))
   {
      if (!remainder.operator<(divisor_))
      {
         remainder = remainder.operator-(divisor_);
         quotient = quotient.operator+(powerof2);
      }
      divisor_.divide_by_2();
      powerof2.divide_by_2();
   }

   return quotient;
}

// mod operation
ubigint ubigint::operator% (const ubigint& that) const {
   ubigint zero = ubigint("0");
   if (that == zero) throw domain_error ("operator/ by zero");
   ubigint powerof2 = ubigint("1");
   ubigint divisor_ = ubigint("0");
   for (auto dig = that.ubig_value.cbegin();
      dig < that.ubig_value.cend(); ++dig)
      divisor_.ubig_value.push_back(*dig);

   while (divisor_.operator<(*this))
   {
      divisor_.multiply_by_2();
      powerof2.multiply_by_2();
   }

   ubigint quotient = ubigint("0");
   ubigint remainder = ubigint("0");
   for (auto dig = ubig_value.cbegin(); dig < ubig_value.cend(); ++dig)
      remainder.ubig_value.push_back(*dig);

   while (zero.operator<(powerof2))
   {
      if (!remainder.operator<(divisor_))
      {
         // mem leaks for sure
         remainder = remainder.operator-(divisor_);
         quotient = quotient.operator+(powerof2);
      }
      divisor_.divide_by_2();
      powerof2.divide_by_2();
   }

   return remainder;
}

// equals operation
bool ubigint::operator== (const ubigint& that) const {

   if (ubig_value.size() > that.ubig_value.size())
      return false;
   if (ubig_value.size() < that.ubig_value.size())
      return false;
   auto numA = ubig_value.cbegin();
   auto numB = that.ubig_value.cbegin();
   while(ubig_value.cend() != numA)
   {
      if(*numA > *numB)
         return false;
      if(*numA < *numB)
         return false;
      ++numA;
      ++numB;
   }
   return true;
}

// less than operation
bool ubigint::operator< (const ubigint& that) const {

   if (ubig_value.size() > that.ubig_value.size())
      return false;
   if (ubig_value.size() < that.ubig_value.size())
      return true;
   auto numA = ubig_value.cbegin();
   auto numB = that.ubig_value.cbegin();
   while(ubig_value.cend() != numA)
   {
      if(*numA > *numB)
         return false;
      if(*numA < *numB)
         return true;
      ++numA;
      ++numB;
   }
   return false;
}

// thing that's getting printed
ostream& operator<< (ostream& out, const ubigint& that)
{
   if (that.ubig_value.size() == 0)
      return out << "0";
   int enter = 0;
   for (auto it = that.ubig_value.cbegin();
      it != that.ubig_value.cend(); ++it)
   {
      if ((enter) % 69 == 0 && enter != 0)
         out << "\\\n";
      out << static_cast<int>(*it);
      ++enter;
   }
   return out;
}








