/*
** Geometric setup
*/
#ifndef __EXT_GEOM_SETUP_HPP__
#define __EXT_GEOM_SETUP_HPP__

#include <vector>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_query.hpp>
#include <glm/ext/scalar_relational.hpp>
#include <glm/ext/vector_relational.hpp>

#include "allocator.hpp"
#include "ext/vector_extensions.hpp"
#include "ext/quat_extensions.hpp"
#include "ext/matrix_extensions.hpp"

#define GLM_GEOM_QUALIFIER static GLM_INLINE

/*
** For MSVC, GLM_NEVER_INLINE cannot be applied as most binding functions
** have no declarations and __declspec(noinline) cannot be used.
*/
#define GLM_GEOM_QUALIFIER_NOINLINE static

/* Check runtime preconditions on geom structures (often related to ensuring normalized vectors) */
#if defined(LUA_GLM_SAFE)
  #define GLM_GEOM_ASSUME(x, onError) \
    do {                              \
      if (!(x))                       \
        return (onError);             \
    } while (0)
#else
  #define GLM_GEOM_ASSUME(x, onError)
#endif

/* Forward declare all structures. */
namespace glm {

  /// <summary>
  /// A STL vector that uses the Lua allocator
  /// </summary>
  /// <typeparam name="T"></typeparam>
  template<typename T> using List = std::vector<T, LuaCrtAllocator<T>>;

  template<length_t L, typename T, qualifier Q = defaultp> struct AABB;
  template<length_t L, typename T, qualifier Q = defaultp> struct Line;
  template<length_t L, typename T, qualifier Q = defaultp> struct Ray;
  template<length_t L, typename T, qualifier Q = defaultp> struct LineSegment;
  template<length_t L, typename T, qualifier Q = defaultp> struct Sphere;
  template<length_t L, typename T, qualifier Q = defaultp> struct Plane;
  template<length_t L, typename T, qualifier Q = defaultp> struct Polygon;
};

/******************************************************************************
* GLM Binding:
* See Copyright Notice in lua.h
\******************************************************************************
* MathGeoLib
* Copyright Jukka Jylänki
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#endif
