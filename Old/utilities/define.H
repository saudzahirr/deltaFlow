#ifndef DEFINE_H
#define DEFINE_H

#define fortran extern "C"
#define subroutine void

#endif

#define forAll(index, matrix) for (size_t index = 0; i < (matrix).rows() * (matrix).cols(); index++)
#define forEach(element, container) for (auto& element : container)
