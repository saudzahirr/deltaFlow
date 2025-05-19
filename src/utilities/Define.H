/*
 * Copyright (c) 2024 saudzahirr
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with deltaFlow; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * @brief Common macro definitions for iteration in deltaFlow.
 *
 * Provides utility macros for iterating over containers in a clear and concise manner.
 */

#ifndef DEFINE_H
#define DEFINE_H

/**
  * @def forAll(index, container)
  * @brief Macro for indexed iteration over a container.
  */
#define forAll(index, container) for (size_t index = 0; index < container.size(); index++)

/**
  * @def forEach(element, container)
  * @brief Macro for range-based iteration over a container.
  */
#define forEach(element, container) for (auto& element : container)

#endif
