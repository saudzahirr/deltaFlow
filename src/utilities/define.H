#ifndef DEFINE_H
#define DEFINE_H

#define fortran extern "C"
#define subroutine void

#define forAll(index, container) for (size_t index = 0; i < container.size(); index++)
#define forEach(element, container) for (auto& element : container)

#endif
