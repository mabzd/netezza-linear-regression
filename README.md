# IBM Netezza Multivariate Linear Regression

## What is this?

This piece of software calculates regression coefficients vector for multivariate linear regression. 
It was written for IBM Netezza appliance system and tested on IBM Netezza Software Emulator 7.1.

## How does it work?

Given SQL table `T` with n numerical columns `X1, X2, ..., Xn` containing independent variables and one 
numerical column `Y` containing dependent variable we can calculate n+1 linear regression coefficients vector 
represented as an output table `B` with two columns: coefficient number (int) and coefficient value (double).

Function signature:
```
linear_regression('
  model=linear_model,
  input_table=<T>,
  input_col=<X1>; <X2>; ...; <Xn>,
  target=<Y>');
```

Usage:
```
=> select * from tab;
 A | B | VAL 
---+---+------
 0 | 0 |  0.5
 0 | 1 |  2.5
 1 | 1 |  3.5
 2 | 5 | 12.5
(4 rows)

=> select linear_regression('model=linear_model, input_table=tab, input_col=a;b, target=val') order by b;
 B | VALUE
---+-------
 0 |     1
 1 |     2
 2 |   0.5
(3 rows)
```
