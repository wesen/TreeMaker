/*******************************************************************************
File:         tmMatrix.h
Project:      TreeMaker 5.x
Purpose:      Header file for TreeMaker matrix classe
Author:       Robert J. Lang
Modified by:  
Created:      2003-11-21
Copyright:    ©2003 Robert J. Lang. All Rights Reserved.
*******************************************************************************/

#ifndef _TMMATRIX_H_
#define _TMMATRIX_H_

#include <vector>

/*
Class tmMatrix<T> implements a row-and-column matrix. It's used in the Newton-
Raphson equation solver, which is based on the LU decompostion of the gradient
matrix. This class has been implemented in a minimal way as a
std::vector<std::vector<T> >.
*/

/**********
class tmMatrix<T>
**********/
template <class T>
class tmMatrix : public std::vector<std::vector<T> > {
public:
  tmMatrix(std::size_t aRows = 0, std::size_t aCols = 0, const T& t = 0);
  std::size_t GetRows(void) {
    // Return the number of rows in the matrix (first index)
    return this->size();};
  std::size_t GetCols(void) {
    // Return the number of columns in the matrix (second index)
    if (GetRows()) return (*this)[0].size(); else return 0;};
  void resize(std::size_t aRows, std::size_t aCols);
private:
  void resize(std::size_t sz);    // hide this ancestor method
};


/**********
Template definitions
**********/

/*****
Constructor sets size and optionally assigns a value to the element
*****/
template <class T>
tmMatrix<T>::tmMatrix(std::size_t aRows, std::size_t aCols, const T& t)
  : std::vector<std::vector<T> >(aRows)
{
  for (std::size_t i = 0; i < this->size(); ++i) {
    (*this)[i].resize(aCols);
    (*this)[i].assign(aCols, t);
  }
}


/*****
Change row and column dimensions of a tmMatrix
*****/
template <class T>
void tmMatrix<T>::resize(std::size_t aRows, std::size_t aCols)
{
  std::vector<std::vector<T> >::resize(aRows);
  for (std::size_t i = 0; i < this->size(); ++i) (*this)[i].resize(aCols);
}

#endif // _MATRIX_H_
