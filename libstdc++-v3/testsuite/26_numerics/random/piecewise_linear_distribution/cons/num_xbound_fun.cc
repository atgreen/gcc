// { dg-options "-std=c++0x" }
//
// 2008-12-03  Edward M. Smith-Rowland <3dw4rd@verizon.net>
//
// Copyright (C) 2008 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// 26.4.8.5.3 Class template piecewise_linear_distribution [rand.dist.samp.plinear]
// 26.4.2.4 Concept RandomNumberDistribution [rand.concept.dist]

#include <random>
#include <cmath>
#include <testsuite_hooks.h>

struct cosine_distribution
{
    cosine_distribution(double x0, double lambda)
    : _M_x0(x0), _M_lambda(lambda)
    { }

    double
    operator()(double x)
    {
      if (x - _M_x0 < -_M_lambda / 4)
        return 0.0;
      else if (x - _M_x0 > _M_lambda / 4)
        return 0.0;
      else
        return std::cos(2 * M_PI * (x - _M_x0) / _M_lambda);
    }

private:
    double _M_x0;
    double _M_lambda;
};

void
test01()
{
  bool test __attribute__((unused)) = true;

  cosine_distribution cd(1.5, 3.0);
  std::piecewise_linear_distribution<> u(21, -10.0, 10.0, cd);
  std::vector<double> interval = u.intervals();
  std::vector<double> density = u.densities();
  VERIFY( interval.size() == 22 );
  VERIFY( density.size() == 22 );
}

int main()
{
  test01();
  return 0;
}
