/*
 * Copyright (c) 2024 Saud Zahir
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with deltaFlow.  If not, see
 * <https://www.gnu.org/licenses/>.
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
