/*
** $Id: api.hpp $
** External Lua/GLM binding API.
**
** Missing Headers:
**  glm/gtx/associated_min_max.hpp
**
** See Copyright Notice in lua.h
*/
#ifndef BINDING_API_HPP
#define BINDING_API_HPP
#if !defined(LUA_LIB)
  #define LUA_LIB
#endif

/* Ensure experimental headers are included with glm/ext.hpp */
#if !defined(GLM_ENABLE_EXPERIMENTAL)
  #define GLM_ENABLE_EXPERIMENTAL
#endif

/*
** Flags used by the binding API.
**
** Functions:
@@ LUAGLM_INCLUDE_GTC Include glm/gtc/+
@@ LUAGLM_INCLUDE_GTX Include glm/gtx/+
@@ LUAGLM_INCLUDE_EXT Include glm/ext/+
@@ LUAGLM_INCLUDE_ALL Include all GLM headers.
@@ LUAGLM_ALIASES Include aliases (e.g., length vs. magnitude) to functions.
**
** Features:
@@ LUAGLM_INSTALL_METATABLES Update the global metatables for vector and matrix,
**  types, if ones are not already defined, with lglmlib on load.
@@ LUAGLM_TYPE_COERCION Enable string-to-number type coercion when parsing
**  arguments from the Lua stack. For this binding library to be a superset of
**  lmathlib, e.g., math = require('glm'), this flag must be enabled.
@@ LUAGLM_REPLACE_MATH Force replace _G.math with the binding library on open.
@@ LUAGLM_RECYCLE Enable object recycling: trailing/unused parameters in a
**  function call, e.g., matrix objects, are used as a result store.
@@ LUAGLM_FORCED_RECYCLE Disable this library from allocating memory, i.e.,
**  force usage of LUAGLM_RECYCLE. Note, polygons other geom/ structures ignore
**  this flag (this may change).
@@ LUAGLM_SAFELIB Enable a general try/catch wrapper for API/binding functions.
**  This should not be enabled.
**
** Experimental:
@@ LUAGLM_NUMBER_ARGS Control where the lua_Number-to-float typecasting occurs
**  when binding to *float-only* functions:
**    1. parsing parameters (default);
**    2. pushing result, e.g., glm::eulerAngleXYZ.
@@ LUAGLM_DRIFT Implicitly normalize parameters that expect direction vectors
**  and quaternions.
@@ LUAGLM_INLINED_TEMPLATES Enable inlined-template resolution. Function names
**  include object types to be parsed, e.g., F_P1P2, up to template resolution.
**  @TODO: Include inlined binding examples using C++14-isms.
*/
#include "bindings.hpp"
#include "iterators.hpp"

#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <glm/gtx/easing.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtx/texture.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_factorisation.hpp>
#include <glm/detail/_vectorize.hpp>
/* @COMPAT: introduced in 0.9.9.4 and missing from ext.hpp until 0.9.9.8 */
#if GLM_VERSION >= 994 && GLM_VERSION <= 997
  #include <glm/ext/matrix_common.hpp>
#endif
#if GLM_HAS_CXX11_STL
  #include <glm/gtx/hash.hpp>
#endif

#include "ext/scalar_extensions.hpp"
#include "ext/vector_extensions.hpp"
#include "ext/matrix_extensions.hpp"
#include "ext/quat_extensions.hpp"

#include <lua.hpp>

/*
** {==================================================================
** Header Selection
** ===================================================================
*/

#define COMMON_HPP
#define CONSTANTS_HPP
#define EXPONENTIAL_HPP
#define GEOMETRIC_HPP
#define INTEGER_HPP
#define MATRIX_HPP
#define TRIGONOMETRIC_HPP
#define VECTOR_RELATIONAL_HPP
#if defined(LUAGLM_INCLUDE_ALL)
  #define PACKING_HPP
  //#define STD_RANDOM
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_EXT)
  #define EXT_MATRIX_CLIP_SPACE_HPP
  #define EXT_MATRIX_COMMON_HPP
  #define EXT_MATRIX_PROJECTION_HPP
  #define EXT_MATRIX_RELATIONAL_HPP
  #define EXT_MATRIX_TRANSFORM_HPP
  #define EXT_QUATERNION_COMMON_HPP
  #define EXT_QUATERNION_EXPONENTIAL_HPP
  #define EXT_QUATERNION_GEOMETRIC_HPP
  #define EXT_QUATERNION_RELATIONAL_HPP
  #define EXT_QUATERNION_TRIGONOMETRIC_HPP
  #define EXT_SCALAR_COMMON_HPP
  #define EXT_SCALAR_CONSTANTS_HPP
  #define EXT_SCALAR_INTEGER_HPP
  #define EXT_SCALAR_RELATIONAL_HPP
  #define EXT_SCALAR_ULP_HPP
  #define EXT_VECTOR_COMMON_HPP
  #define EXT_VECTOR_INTEGER_HPP
  #define EXT_VECTOR_RELATIONAL_HPP
  #define EXT_VECTOR_ULP_HPP
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_GTC)
  #define GTC_BITFIELD_HPP
  #define GTC_COLOR_SPACE_HPP
  #define GTC_EPSILON_HPP
  #define GTC_INTEGER_HPP
  #define GTC_MATRIX_ACCESS_HPP
  #define GTC_MATRIX_INVERSE_HPP
  #define GTC_NOISE_HPP
  #define GTC_QUATERNION_HPP
  #define GTC_RANDOM_HPP
  #define GTC_RECIPROCAL_HPP
  #define GTC_ROUND_HPP
  #define GTC_TYPE_PRECISION_HPP
  #define GTC_ULP_HPP
#endif

#if defined(LUAGLM_INCLUDE_ALL) || defined(LUAGLM_INCLUDE_GTX)
  #if !defined(GLM_ENABLE_EXPERIMENTAL)
    #error "GLM_ENABLE_EXPERIMENTAL not enabled!"
  #endif

  #define GTX_ASSOCIATED_MIN_MAX_HPP
  #define GTX_BIT_HPP
  #define GTX_CLOSEST_POINT_HPP
  #define GTX_COLOR_ENCODING_HPP
  #define GTX_COLOR_SPACE_HPP
  #define GTX_COLOR_SPACE_YCOCG_HPP
  #define GTX_COMMON_HPP
  #define GTX_COMPATIBILITY_HPP
  #define GTX_COMPONENT_WISE_HPP
  #define GTX_EASING_HPP
  #define GTX_EULER_ANGLES_HPP
  #define GTX_EXTEND_HPP
  #define GTX_EXTERIOR_PRODUCT_HPP
  #define GTX_FAST_EXPONENTIAL_HPP
  #define GTX_FAST_SQUARE_ROOT_HPP
  #define GTX_FAST_TRIGONOMETRY_HPP
  #define GTX_FUNCTIONS_HPP
  #define GTX_GRADIENT_PAINT_HPP
  #define GTX_HANDED_COORDINATE_SPACE_HPP
  #define GTX_INTEGER_HPP
  #define GTX_INTERSECT_HPP
  #define GTX_LOG_BASE_HPP
  #define GTX_LOG_BASE_HPP
  #define GTX_MATRIX_CROSS_PRODUCT_HPP
  #define GTX_MATRIX_DECOMPOSE_HPP
  #define GTX_MATRIX_FACTORISATION_HPP
  #define GTX_MATRIX_INTERPOLATION_HPP
  #define GTX_MATRIX_MAJOR_STORAGE_HPP
  #define GTX_MATRIX_OPERATION_HPP
  #define GTX_MATRIX_QUERY_HPP
  #define GTX_MATRIX_TRANSFORM_2D_HPP
  #define GTX_MIXED_PRODUCT_HPP
  #define GTX_NORMALIZE_DOT_HPP
  #define GTX_NORMAL_HPP
  #define GTX_NORM_HPP
  #define GTX_OPTIMUM_POW_HPP
  #define GTX_ORTHONORMALIZE_HPP
  #define GTX_PCA_HPP
  #define GTX_PERPENDICULAR_HPP
  #define GTX_POLAR_COORDINATES_HPP
  #define GTX_PROJECTION_HPP
  #define GTX_QUATERNION_HPP
  #define GTX_QUATERNION_TRANSFORM_HPP
  #define GTX_RANGE_HPP
  #define GTX_ROTATE_NORMALIZED_AXIS_HPP
  #define GTX_ROTATE_VECTOR_HPP
  #define GTX_SPLINE_HPP
  #define GTX_TEXTURE_HPP
  #define GTX_TRANSFORM2_HPP
  #define GTX_TRANSFORM_HPP
  #define GTX_VECTOR_ANGLE_HPP
  #define GTX_VECTOR_QUERY_HPP
  #define GTX_WRAP_HPP
#endif

/*
** @WIP Macros that ensure backwards compatibility.
**
** Temporary change until an improved feature-flag system is developed to reduce
** the size of the binding library (if ever?).
*/
#if GLM_VERSION < 999
  #undef GTX_PCA_HPP  // @COMPAT: pca.hpp introduced in 0.9.9.9
  #if GLM_VERSION < 994
    #undef EXT_MATRIX_COMMON_HPP  // @COMPAT: ext/matrix_common.hpp introduced in 0.9.9.4
  #endif
#endif

/* }================================================================== */

/*
** {==================================================================
** Object Properties
** ===================================================================
*/

/* Template for matrix equals/not-equals */
#define LAYOUT_MATRIX_EQUAL(LB, F, Tr, ...) \
  LAYOUT_GENERIC_EQUAL(LB, F, Tr, Tr::row_type)

/*
** Template for generalized equals/not-equals function.
**
** Missing: "vec<L, int, Q> const& ULPs". The current design makes it impossible
** to differentiate between a vector of eps values and ULP values.
*/
#define LAYOUT_EQUAL(LB, F, ...)                                                                        \
  LUA_MLM_BEGIN                                                                                         \
  const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                                        \
  switch (ttypetag(_tv)) {                                                                              \
    case LUA_VNUMINT: {                                                                                 \
      if (gLuaInteger::Is((LB).L, (LB).idx + 1))                                                        \
        VA_CALL(BIND_FUNC, LB, F, gLuaInteger::fast, gLuaInteger);                                      \
    }  LUAGLM_FALLTHROUGH;                                                                              \
    case LUA_VFALSE: case LUA_VTRUE: /* @BoolCoercion */                                                \
    case LUA_VSHRSTR: case LUA_VLNGSTR: /* @StringCoercion */                                           \
    case LUA_VNUMFLT: LAYOUT_GENERIC_EQUAL(LB, F, gLuaTrait<glm_Number>, gLuaTrait<glm_Number>); break; \
    case LUA_VVECTOR2: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec2<>::fast, gLuaVec2<>::fast); break;          \
    case LUA_VVECTOR3: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec3<>::fast, gLuaVec3<>::fast); break;          \
    case LUA_VVECTOR4: LAYOUT_GENERIC_EQUAL(LB, F, gLuaVec4<>::fast, gLuaVec4<>::fast); break;          \
    case LUA_VQUAT: LAYOUT_GENERIC_EQUAL(LB, F, gLuaQuat<>::fast, gLuaVec4<>::fast); break;             \
    case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(_tv), F, LAYOUT_MATRIX_EQUAL); break;                \
    default:                                                                                            \
      break;                                                                                            \
  }                                                                                                     \
  return GLM_TYPE_ERROR((LB).L, (LB).idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN);                 \
  LUA_MLM_END

/* glm/gtx/string_cast.hpp */
GLM_BINDING_QUALIFIER(to_string) {
  GLM_BINDING_BEGIN
  const int n = LB.top();
  for (int i = LB.idx; i <= n; ++i)
    lua_tostring(LB.L, i);
  return n;
  GLM_BINDING_END
}

/* glm/ext/scalar_relational.hpp, glm/ext/vector_common.hpp, glm/ext/vector_relational.hpp, glm/ext/quaternion_relational.hpp, glm/ext/matrix_relational.hpp */
LAYOUT_DEFN(equal, glm::equal, LAYOUT_EQUAL, void)
LAYOUT_DEFN(notEqual, glm::notEqual, LAYOUT_EQUAL, void)
LAYOUT_DEFN(all_equal, glm::all_equal, LAYOUT_EQUAL, void)  // LUA_VECTOR_EXTENSIONS
LAYOUT_DEFN(any_notequal, glm::any_notequal, LAYOUT_EQUAL, void)  // LUA_VECTOR_EXTENSIONS
#if GLM_HAS_CXX11_STL
#define LAYOUT_HASH(LB, F, Traits, ...)                     \
  LUA_MLM_BEGIN                                             \
  F<Traits::type> hash;                                     \
  gLuaBase::Push(LB, hash(Traits::Next((LB).L, (LB).idx))); \
  LUA_MLM_END

GLM_BINDING_QUALIFIER(hash) { /* glm/gtx/hash.hpp */
  GLM_BINDING_BEGIN
  const int n = LB.top();
  while (LB.idx <= n) {
    const TValue *_tv = glm_i2v(LB.L, LB.idx);
    switch (ttypetag(_tv)) {
      case LUA_VTRUE:
      case LUA_VFALSE: LAYOUT_HASH(LB, std::hash, gLuaTrait<bool>::fast); break;
      case LUA_VSHRSTR:
      case LUA_VLNGSTR: LAYOUT_HASH(LB, std::hash, gLuaTrait<const char *>::fast); break;
      case LUA_VNUMINT: LAYOUT_HASH(LB, std::hash, gLuaInteger::fast); break;
      case LUA_VNUMFLT: LAYOUT_HASH(LB, std::hash, gLuaTrait<glm_Number>::fast); break;
      case LUA_VVECTOR2: LAYOUT_HASH(LB, std::hash, gLuaVec2<>::fast); break;
      case LUA_VVECTOR3: LAYOUT_HASH(LB, std::hash, gLuaVec3<>::fast); break;
      case LUA_VVECTOR4: LAYOUT_HASH(LB, std::hash, gLuaVec4<>::fast); break;
      case LUA_VQUAT: LAYOUT_HASH(LB, std::hash, gLuaQuat<>); break;
      case LUA_VMATRIX: PARSE_MATRIX(LB, mvalue_dims(_tv), std::hash, LAYOUT_HASH); break;
      default: {
        return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN " or " GLM_STRING_MATRIX);
      }
    }
  }
  return LB.top() - n;
  GLM_BINDING_END
}
#endif

TRAITS_DEFN(up, glm::unit::up<glm_Float>)  // LUA_VECTOR_EXTENSIONS
TRAITS_DEFN(right, glm::unit::right<glm_Float>)
TRAITS_DEFN(forward, glm::unit::forward<glm_Float>)
TRAITS_DEFN(forwardLH, glm::unit::forwardLH<glm_Float>)
TRAITS_DEFN(forwardRH, glm::unit::forwardRH<glm_Float>)
GLM_BINDING_QUALIFIER(unpack) {
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle();
  for (; LB.idx <= n; ++LB.idx) {
    const TValue *_tv = glm_i2v(LB.L, LB.idx);
    switch (ttype(_tv)) {
      case LUA_TVECTOR: glm_unpack_vector(LB.L, LB.idx); break;
      case LUA_TMATRIX: glm_unpack_matrix(LB.L, LB.idx); break;
      default: {
        lua_pushvalue(LB.L, LB.idx);
        break;
      }
    }
  }
  return LB.top() - n;
  GLM_BINDING_END
}

/* }================================================================== */

/*
** {==================================================================
** Functional Operators
** ===================================================================
*/

/* Layout for object addition: object + object || object + number */
#define LAYOUT_ADDITION_OP(LB, F, Tr, type, ...)                   \
  LUA_MLM_BEGIN                                                    \
  if (type == LUA_TNUMBER)                                         \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); \
  else                                                             \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, ##__VA_ARGS__);        \
  LUA_MLM_END

/* Layout for generic matrix multiplication. */
#define LAYOUT_MULTIPLICATION_OP(LB, F, Tr, type, ...)                                      \
  LUA_MLM_BEGIN                                                                             \
  switch (type) {                                                                           \
    case LUA_TNUMBER: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, ##__VA_ARGS__); break; \
    case LUA_TVECTOR: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::row_type, ##__VA_ARGS__); break;    \
    case LUA_TMATRIX: {                                                                     \
      switch (LUAGLM_MATRIX_ROWS(mvalue_dims(_tv))) {                                       \
        case 2: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<2>, ##__VA_ARGS__); break;   \
        case 3: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<3>, ##__VA_ARGS__); break;   \
        case 4: VA_CALL(BIND_FUNC, LB, F, Tr, Tr::rhs_mat_type<4>, ##__VA_ARGS__); break;   \
        default:                                                                            \
          break;                                                                            \
      }                                                                                     \
      break;                                                                                \
    }                                                                                       \
    default:                                                                                \
      break;                                                                                \
  }                                                                                         \
  break;                                                                                    \
  LUA_MLM_END

/// <summary>
/// A matrix addition function intended to allow the recycling of preallocated
/// matrix structures (for the function result). Any operations that result in
/// vector/numeric types should use the arithmetic operator.
/// </summary>
GLM_BINDING_QUALIFIER(mat_add) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);
  if (l_likely(ttismatrix(_tv))) {
    switch (mvalue_dims(_tv)) {
      case LUAGLM_MATRIX_2x2: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat2x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_2x3: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat2x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_2x4: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat2x4<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x2: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat3x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x3: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat3x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x4: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat3x4<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x2: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat4x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x3: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat4x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x4: LAYOUT_ADDITION_OP(LB, operator+, gLuaMat4x4<>::fast, ttype(_tv2)); break;
      default: {
        break;
      }
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(mat_sub) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);
  if (l_likely(ttismatrix(_tv))) {
    switch (mvalue_dims(_tv)) {
      case LUAGLM_MATRIX_2x2: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat2x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_2x3: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat2x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_2x4: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat2x4<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x2: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat3x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x3: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat3x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_3x4: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat3x4<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x2: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat4x2<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x3: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat4x3<>::fast, ttype(_tv2)); break;
      case LUAGLM_MATRIX_4x4: LAYOUT_ADDITION_OP(LB, operator-, gLuaMat4x4<>::fast, ttype(_tv2)); break;
      default: {
        break;
      }
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

/// <summary>
/// A matrix multiplication function intended to allow the recycling of
/// preallocated matrix structures (for the function result). Any operations
/// that result in vector/numeric types should use the arithmetic operator.
/// </summary>
GLM_BINDING_QUALIFIER(mat_mul) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);
  switch (ttypetag(_tv)) {
    case LUA_VFALSE: case LUA_VTRUE:  // @BoolCoercion
    case LUA_VNUMINT:
    case LUA_VNUMFLT: {  // number * matrix
      if (l_likely(ttismatrix(_tv2))) {
        switch (mvalue_dims(_tv2)) {
          case LUAGLM_MATRIX_2x2: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat2x2<>::fast); break;
          case LUAGLM_MATRIX_2x3: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat2x3<>::fast); break;
          case LUAGLM_MATRIX_2x4: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat2x4<>::fast); break;
          case LUAGLM_MATRIX_3x2: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat3x2<>::fast); break;
          case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat3x3<>::fast); break;
          case LUAGLM_MATRIX_3x4: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat3x4<>::fast); break;
          case LUAGLM_MATRIX_4x2: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat4x2<>::fast); break;
          case LUAGLM_MATRIX_4x3: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat4x3<>::fast); break;
          case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, operator*, gLuaFloat, gLuaMat4x4<>::fast); break;
          default: {
            break;
          }
        }
      }
      break;
    }
    case LUA_VVECTOR2: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec2<>::fast, ttype(_tv2)); break;
    case LUA_VVECTOR3: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec3<>::fast, ttype(_tv2)); break;
    case LUA_VVECTOR4: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaVec4<>::fast, ttype(_tv2)); break;
    case LUA_VMATRIX: {
      switch (mvalue_dims(_tv)) {
        case LUAGLM_MATRIX_2x2: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat2x2<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_2x3: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat2x3<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_2x4: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat2x4<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_3x2: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat3x2<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_3x3: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat3x3<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_3x4: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat3x4<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_4x2: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat4x2<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_4x3: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat4x3<>::fast, ttype(_tv2)); break;
        case LUAGLM_MATRIX_4x4: LAYOUT_MULTIPLICATION_OP(LB, operator*, gLuaMat4x4<>::fast, ttype(_tv2)); break;
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(mat_negate) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  if (l_likely(ttismatrix(_tv))) {
    switch (mvalue_dims(_tv)) {
      case LUAGLM_MATRIX_2x2: BIND_FUNC(LB, operator-, gLuaMat2x2<>::fast); break;
      case LUAGLM_MATRIX_2x3: BIND_FUNC(LB, operator-, gLuaMat2x3<>::fast); break;
      case LUAGLM_MATRIX_2x4: BIND_FUNC(LB, operator-, gLuaMat2x4<>::fast); break;
      case LUAGLM_MATRIX_3x2: BIND_FUNC(LB, operator-, gLuaMat3x2<>::fast); break;
      case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, operator-, gLuaMat3x3<>::fast); break;
      case LUAGLM_MATRIX_3x4: BIND_FUNC(LB, operator-, gLuaMat3x4<>::fast); break;
      case LUAGLM_MATRIX_4x2: BIND_FUNC(LB, operator-, gLuaMat4x2<>::fast); break;
      case LUAGLM_MATRIX_4x3: BIND_FUNC(LB, operator-, gLuaMat4x3<>::fast); break;
      case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, operator-, gLuaMat4x4<>::fast); break;
      default: {
        break;
      }
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_MATRIX);
  GLM_BINDING_END
}

/* }================================================================== */

/*
** {==================================================================
** Scalar Specific
** ===================================================================
*/

#if defined(INTEGER_HPP)
#define LAYOUT_ADD_CARRY(LB, F, Tr, ...)               \
  LUA_MLM_BEGIN                                        \
  const Tr::type a = Tr::Next((LB).L, (LB).idx);       \
  const Tr::type b = Tr::safe::Next((LB).L, (LB).idx); \
  Tr::type carry = Tr::zero();                         \
  TRAITS_PUSH(LB, F(a, b, carry), carry);              \
  LUA_MLM_END

#define LAYOUT_MUL_EXTENDED(LB, F, Tr, ...)            \
  LUA_MLM_BEGIN                                        \
  const Tr::type a = Tr::Next((LB).L, (LB).idx);       \
  const Tr::type b = Tr::safe::Next((LB).L, (LB).idx); \
  Tr::type lsb = Tr::zero();                           \
  Tr::type msb = Tr::zero();                           \
  F(a, b, lsb, msb);                                   \
  TRAITS_PUSH(LB, lsb, msb);                           \
  LUA_MLM_END

#if LUA_INT_TYPE != LUA_INT_INT || !defined(LUAGLM_ALIGNED)
INTEGER_VECTOR_DEFN(bitCount, glm::bitCount, LAYOUT_UNARY, lua_Unsigned)
#endif
INTEGER_VECTOR_DEFN(bitfieldExtract, glm::bitfieldExtract, LAYOUT_UNARY, lua_Unsigned, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldInsert, glm::bitfieldInsert, LAYOUT_BINARY, lua_Unsigned, gLuaTrait<int>, gLuaTrait<int>)
#if LUA_INT_TYPE != LUA_INT_INT || !defined(LUAGLM_ALIGNED)
INTEGER_VECTOR_DEFN(bitfieldReverse, glm::bitfieldReverse, LAYOUT_UNARY, lua_Unsigned)
#endif
INTEGER_VECTOR_DEFN(findLSB, glm::findLSB, LAYOUT_UNARY, lua_Unsigned)
INTEGER_VECTOR_DEFN(findMSB, glm::findMSB, LAYOUT_UNARY, lua_Unsigned)
INTEGER_VECTOR_DEFN(imulExtended, glm::imulExtended, LAYOUT_MUL_EXTENDED, int)
INTEGER_VECTOR_DEFN(uaddCarry, glm::uaddCarry, LAYOUT_ADD_CARRY, glm::uint)
INTEGER_VECTOR_DEFN(umulExtended, glm::umulExtended, LAYOUT_MUL_EXTENDED, glm::uint)
INTEGER_VECTOR_DEFN(usubBorrow, glm::usubBorrow, LAYOUT_ADD_CARRY, glm::uint)
#endif

#if defined(EXT_SCALAR_INTEGER_HPP) || defined(EXT_VECTOR_INTEGER_HPP)
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
INTEGER_VECTOR_DEFN(findNSB, glm::findNSB, LAYOUT_VECTOR_INT, lua_Unsigned)
#endif
#endif

#if defined(GTC_BITFIELD_HPP)
TRAITS_DEFN(bitfieldDeinterleave, glm::bitfieldDeinterleave, gLuaTrait<glm::uint64>)
INTEGER_VECTOR_DEFN(bitfieldFillOne, glm::bitfieldFillOne, LAYOUT_UNARY, lua_Unsigned, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldFillZero, glm::bitfieldFillZero, LAYOUT_UNARY, lua_Unsigned, gLuaTrait<int>, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldRotateLeft, glm::bitfieldRotateLeft, LAYOUT_UNARY, lua_Unsigned, gLuaTrait<int>)
INTEGER_VECTOR_DEFN(bitfieldRotateRight, glm::bitfieldRotateRight, LAYOUT_UNARY, lua_Unsigned, gLuaTrait<int>)
GLM_BINDING_QUALIFIER(bitfieldInterleave) {
  GLM_BINDING_BEGIN
  switch (LB.top()) {
    case 2: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>); break;
    case 3: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>, gLuaTrait<glm::uint32_t>); break;
    case 4: BIND_FUNC(LB, glm::bitfieldInterleave, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>, gLuaTrait<glm::uint16_t>); break;
    default: {
      break;
    }
  }
  return luaL_error(LB.L, "interleave expects {uint32_t, uint32_t}, {uint32_t, uint32_t, uint32_t}, or {uint16_t, uint16_t, uint16_t, uint16_t}");
  GLM_BINDING_END
}
INTEGER_VECTOR_DEFN(mask, glm::mask, LAYOUT_UNARY, lua_Unsigned)
#endif

#if defined(GTX_BIT_HPP)
INTEGER_VECTOR_DEFN(highestBitValue, glm::highestBitValue, LAYOUT_UNARY, lua_Integer)
INTEGER_VECTOR_DEFN(lowestBitValue, glm::lowestBitValue, LAYOUT_UNARY, lua_Integer)
// GLM_BINDING_DECL(powerOfTwoAbove);  // Deprecated
// GLM_BINDING_DECL(powerOfTwoBelow);  // Deprecated
// GLM_BINDING_DECL(powerOfTwoNearest);  // Deprecated
#endif

#if defined(PACKING_HPP)
TRAITS_DEFN(packUnorm2x16, glm::packUnorm2x16, gLuaVec2<float>)
TRAITS_DEFN(unpackUnorm2x16, glm::unpackUnorm2x16, gLuaTrait<glm::uint>)
TRAITS_DEFN(packSnorm2x16, glm::packSnorm2x16, gLuaVec2<float>)
TRAITS_DEFN(unpackSnorm2x16, glm::unpackSnorm2x16, gLuaTrait<glm::uint>)
TRAITS_DEFN(packUnorm4x8, glm::packUnorm4x8, gLuaVec4<float>)
TRAITS_DEFN(unpackUnorm4x8, glm::unpackUnorm4x8, gLuaTrait<glm::uint>)
TRAITS_DEFN(packSnorm4x8, glm::packSnorm4x8, gLuaVec4<float>)
TRAITS_DEFN(unpackSnorm4x8, glm::unpackSnorm4x8, gLuaTrait<glm::uint>)
TRAITS_DEFN(packDouble2x32, glm::packDouble2x32, gLuaVec2<glm::uint>)
TRAITS_DEFN(unpackDouble2x32, glm::unpackDouble2x32, gLuaTrait<double>)
TRAITS_DEFN(packHalf2x16, glm::packHalf2x16, gLuaVec2<float>)
TRAITS_DEFN(unpackHalf2x16, glm::unpackHalf2x16, gLuaTrait<glm::uint>)
#endif

#if defined(GTC_TYPE_PRECISION_HPP)
TRAITS_DEFN(packUnorm1x8, glm::packUnorm1x8, gLuaTrait<float>)
TRAITS_DEFN(unpackUnorm1x8, glm::unpackUnorm1x8, gLuaTrait<glm::uint8>)
TRAITS_DEFN(packUnorm2x8, glm::packUnorm2x8, gLuaVec2<float>)
TRAITS_DEFN(unpackUnorm2x8, glm::unpackUnorm2x8, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packSnorm1x8, glm::packSnorm1x8, gLuaTrait<float>)
TRAITS_DEFN(unpackSnorm1x8, glm::unpackSnorm1x8, gLuaTrait<glm::uint8>)
TRAITS_DEFN(packSnorm2x8, glm::packSnorm2x8, gLuaVec2<float>)
TRAITS_DEFN(unpackSnorm2x8, glm::unpackSnorm2x8, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packUnorm1x16, glm::packUnorm1x16, gLuaTrait<float>)
TRAITS_DEFN(unpackUnorm1x16, glm::unpackUnorm1x16, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packUnorm4x16, glm::packUnorm4x16, gLuaVec4<float>)
TRAITS_DEFN(unpackUnorm4x16, glm::unpackUnorm4x16, gLuaTrait<glm::uint64>)
TRAITS_DEFN(packSnorm1x16, glm::packSnorm1x16, gLuaTrait<float>)
TRAITS_DEFN(unpackSnorm1x16, glm::unpackSnorm1x16, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packSnorm4x16, glm::packSnorm4x16, gLuaVec4<float>)
TRAITS_DEFN(unpackSnorm4x16, glm::unpackSnorm4x16, gLuaTrait<glm::uint64>)
TRAITS_DEFN(packHalf1x16, glm::packHalf1x16, gLuaTrait<float>)
TRAITS_DEFN(unpackHalf1x16, glm::unpackHalf1x16, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packHalf4x16, glm::packHalf4x16, gLuaVec4<float>)
TRAITS_DEFN(unpackHalf4x16, glm::unpackHalf4x16, gLuaTrait<glm::uint64>)
TRAITS_DEFN(packI3x10_1x2, glm::packI3x10_1x2, gLuaVec4<int>)
TRAITS_DEFN(unpackI3x10_1x2, glm::unpackI3x10_1x2, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packU3x10_1x2, glm::packU3x10_1x2, gLuaVec4<unsigned>)
TRAITS_DEFN(unpackU3x10_1x2, glm::unpackU3x10_1x2, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packSnorm3x10_1x2, glm::packSnorm3x10_1x2, gLuaVec4<float>)
TRAITS_DEFN(unpackSnorm3x10_1x2, glm::unpackSnorm3x10_1x2, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packUnorm3x10_1x2, glm::packUnorm3x10_1x2, gLuaVec4<float>)
TRAITS_DEFN(unpackUnorm3x10_1x2, glm::unpackUnorm3x10_1x2, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packF2x11_1x10, glm::packF2x11_1x10, gLuaVec3<float>)
TRAITS_DEFN(unpackF2x11_1x10, glm::unpackF2x11_1x10, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packF3x9_E1x5, glm::packF3x9_E1x5, gLuaVec3<float>)
TRAITS_DEFN(unpackF3x9_E1x5, glm::unpackF3x9_E1x5, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packRGBM, glm::packRGBM, gLuaVec3<>)
TRAITS_DEFN(unpackRGBM, glm::unpackRGBM, gLuaVec4<>)
INTEGER_VECTOR_DEFN(packHalf, glm::packHalf, LAYOUT_UNARY, float)
INTEGER_VECTOR_DEFN(unpackHalf, glm::unpackHalf, LAYOUT_UNARY, glm::uint16)
INTEGER_VECTOR_DEFN(packUnorm, glm::packUnorm<glm::uint16>, LAYOUT_UNARY, float)
INTEGER_VECTOR_DEFN(unpackUnorm, glm::unpackUnorm<float>, LAYOUT_UNARY, glm::uint16)
INTEGER_VECTOR_DEFN(packSnorm, glm::packSnorm<glm::int16>, LAYOUT_UNARY, float)
INTEGER_VECTOR_DEFN(unpackSnorm, glm::unpackSnorm<float>, LAYOUT_UNARY, glm::int16)
TRAITS_DEFN(packUnorm2x4, glm::packUnorm2x4, gLuaVec2<float>)
TRAITS_DEFN(unpackUnorm2x4, glm::unpackUnorm2x4, gLuaTrait<glm::uint8>)
TRAITS_DEFN(packUnorm4x4, glm::packUnorm4x4, gLuaVec4<float>)
TRAITS_DEFN(unpackUnorm4x4, glm::unpackUnorm4x4, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packUnorm1x5_1x6_1x5, glm::packUnorm1x5_1x6_1x5, gLuaVec3<float>)
TRAITS_DEFN(unpackUnorm1x5_1x6_1x5, glm::unpackUnorm1x5_1x6_1x5, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packUnorm3x5_1x1, glm::packUnorm3x5_1x1, gLuaVec4<float>)
TRAITS_DEFN(unpackUnorm3x5_1x1, glm::unpackUnorm3x5_1x1, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packUnorm2x3_1x2, glm::packUnorm2x3_1x2, gLuaVec3<float>)
TRAITS_DEFN(unpackUnorm2x3_1x2, glm::unpackUnorm2x3_1x2, gLuaTrait<glm::uint8>)
TRAITS_DEFN(packInt2x8, glm::packInt2x8, gLuaVec2<glm::i8>)
TRAITS_DEFN(unpackInt2x8, glm::unpackInt2x8, gLuaTrait<glm::int16>)
TRAITS_DEFN(packUint2x8, glm::packUint2x8, gLuaVec2<glm::u8>)
TRAITS_DEFN(unpackUint2x8, glm::unpackUint2x8, gLuaTrait<glm::uint16>)
TRAITS_DEFN(packInt4x8, glm::packInt4x8, gLuaVec4<glm::i8>)
TRAITS_DEFN(unpackInt4x8, glm::unpackInt4x8, gLuaTrait<glm::int32>)
TRAITS_DEFN(packUint4x8, glm::packUint4x8, gLuaVec4<glm::u8>)
TRAITS_DEFN(unpackUint4x8, glm::unpackUint4x8, gLuaTrait<glm::uint32>)
TRAITS_DEFN(packInt2x16, glm::packInt2x16, gLuaVec2<glm::i16>)
TRAITS_DEFN(unpackInt2x16, glm::unpackInt2x16, gLuaTrait<int>)
TRAITS_DEFN(packInt4x16, glm::packInt4x16, gLuaVec4<glm::i16>)
TRAITS_DEFN(unpackInt4x16, glm::unpackInt4x16, gLuaTrait<glm::int64>)
TRAITS_DEFN(packUint2x16, glm::packUint2x16, gLuaVec2<glm::u16>)
TRAITS_DEFN(unpackUint2x16, glm::unpackUint2x16, gLuaTrait<glm::uint>)
TRAITS_DEFN(packUint4x16, glm::packUint4x16, gLuaVec4<glm::u16>)
TRAITS_DEFN(unpackUint4x16, glm::unpackUint4x16, gLuaTrait<glm::uint64>)
TRAITS_DEFN(packInt2x32, glm::packInt2x32, gLuaVec2<glm::i32>)
TRAITS_DEFN(unpackInt2x32, glm::unpackInt2x32, gLuaTrait<glm::int64>)
TRAITS_DEFN(packUint2x32, glm::packUint2x32, gLuaVec2<glm::u32>)
TRAITS_DEFN(unpackUint2x32, glm::unpackUint2x32, gLuaTrait<glm::uint64>)
#endif

#if defined(GTC_ULP_HPP) || defined(EXT_SCALAR_ULP_HPP)
#if GLM_VERSION >= 993  // @COMPAT: float_distance incorrectly declared until 0.9.9.3
NUMBER_VECTOR_DEFN(float_distance, glm::float_distance, LAYOUT_BINARY)
#endif
#endif

#if defined(GTC_ULP_HPP) || defined(EXT_SCALAR_ULP_HPP) || defined(EXT_VECTOR_ULP_HPP)
#define LAYOUT_NEXT_FLOAT(LB, F, Tr, ...) /* @GLMAssert: assert(ULPs >= 0); */        \
  LUA_MLM_BEGIN                                                                       \
  if (lua_isnoneornil((LB).L, (LB).idx + 1))                                          \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                                     \
  else if (gLuaTrait<int>::Is((LB).L, (LB).idx + 1))                                  \
    VA_CALL(BIND_FUNC, LB, F, Tr, gLuaBoundedBelow<gLuaTrait<int>>, ##__VA_ARGS__);   \
  else                                                                                \
    VA_CALL(BIND_FUNC, LB, F, Tr, gLuaBoundedBelow<Tr::as_type<int>>, ##__VA_ARGS__); \
  LUA_MLM_END

#if GLM_VERSION >= 993  // @COMPAT: vector support added in 0.9.9.3
NUMBER_VECTOR_DEFN(next_float, glm::next_float, LAYOUT_NEXT_FLOAT)
NUMBER_VECTOR_DEFN(prev_float, glm::prev_float, LAYOUT_NEXT_FLOAT)
#else
LAYOUT_DEFN(next_float, glm::next_float, LAYOUT_NEXT_FLOAT, gLuaNumber)
LAYOUT_DEFN(prev_float, glm::prev_float, LAYOUT_NEXT_FLOAT, gLuaNumber)
#endif
#endif

/* }================================================================== */

/*
** {==================================================================
** Quaternion Specific
** ===================================================================
*/

#if defined(EXT_QUATERNION_COMMON_HPP)
TRAITS_DEFN(conjugate, glm::conjugate, gLuaQuat<>)
#endif

#if defined(EXT_QUATERNION_COMMON_HPP) || defined(MATRIX_HPP)
GLM_BINDING_QUALIFIER(inverse) {
  GLM_BINDING_BEGIN
  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttismatrix(o))
    PARSE_SYMMETRIC_MATRIX(LB, glm::inverse, LAYOUT_UNARY);
  else
    PARSE_NUMBER_VECTOR_QUAT(LB, glm::inverse, LAYOUT_UNARY, LAYOUT_UNARY, LAYOUT_UNARY);
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}
SYMMETRIC_MATRIX_DEFN(invertible, glm::invertible, LAYOUT_UNARY)  // LUA_MATRIX_EXTENSIONS
ROTATION_MATRIX_DEFN(inverse_transform, glm::inverse_transform, LAYOUT_UNARY)
#endif

/* glm/ext/quaternion_trigonometric.hpp */
#if defined(EXT_QUATERNION_TRIGONOMETRIC_HPP)
TRAITS_DEFN(axis, glm::axis, gLuaQuat<>)
TRAITS_DEFN(angleAxis, glm::angleAxis, gLuaVec3<>::value_trait, gLuaDir3<>)
#endif

/* glm/gtc/quaternion.hpp */
#if defined(GTC_QUATERNION_HPP)
TRAITS_DEFN(eulerAngles, glm::eulerAngles, gLuaQuat<>)
TRAITS_DEFN(mat3_cast, glm::mat3_cast, gLuaQuat<>)
TRAITS_DEFN(mat4_cast, glm::mat4_cast, gLuaQuat<>)
TRAITS_DEFN(pitch, glm::pitch, gLuaQuat<>)
TRAITS_DEFN(roll, glm::roll, gLuaQuat<>)
TRAITS_DEFN(yaw, glm::yaw, gLuaQuat<>)
TRAITS_DEFN(quatLookAt, glm::quatLookAt, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(quatLookAtLH, glm::quatLookAtLH, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(quatLookAtRH, glm::quatLookAtRH, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(quatbillboard, glm::quatbillboard, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)  // LUA_QUATERNION_EXTENSIONS
TRAITS_DEFN(quatbillboardRH, glm::quatbillboardRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(quatbillboardLH, glm::quatbillboardLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(quatFromBasis, glm::fromBasis, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
#endif

/* glm/gtx/quaternion.hpp */
#if defined(GTX_QUATERNION_HPP)
TRAITS_DEFN(extractRealComponent, glm::extractRealComponent, gLuaQuat<>)
TRAITS_DEFN(fastMix, glm::fastMix, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
TRAITS_DEFN(intermediate, glm::intermediate, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>)
TRAITS_DEFN(shortMix, glm::shortMix, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
TRAITS_DEFN(toMat3, glm::toMat3, gLuaQuat<>)
TRAITS_DEFN(toMat4, glm::toMat4, gLuaQuat<>)
TRAITS_DEFN(squad, glm::squad, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>, gLuaQuat<>::value_trait)
TRAITS_DEFN(rotation, glm::rotation, gLuaVec3<>, gLuaVec3<>)
ROTATION_MATRIX_DEFN(quat_cast, glm::quat_cast, LAYOUT_UNARY)
TRAITS_DEFN(quat_identity, glm::identity<gLuaQuat<>::type>)
#endif

/* glm/gtx/rotate_normalized_axis.hpp */
#if defined(GTX_ROTATE_NORMALIZED_AXIS_HPP)
ROTATION_MATRIX_DEFN(rotateNormalizedAxis, glm::rotateNormalizedAxis, LAYOUT_UNARY, gLuaFloat, gLuaDir3<>)
#endif

/* }================================================================== */

/*
** {==================================================================
** Matrix Specific
** ===================================================================
*/

#if defined(MATRIX_HPP)
#define LAYOUT_OUTERPRODUCT(LB, F, Tr, ...)                                           \
  LUA_MLM_BEGIN                                                                       \
  const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);                                     \
  switch (ttypetag(_tv2)) {                                                           \
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec2<>::fast); break; \
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec3<>::fast); break; \
    case LUA_VVECTOR4: BIND_FUNC(LB, glm::outerProduct, Tr, gLuaVec4<>::fast); break; \
    default: {                                                                        \
      break;                                                                          \
    }                                                                                 \
  }                                                                                   \
  LUA_MLM_END

SYMMETRIC_MATRIX_DEFN(determinant, glm::determinant, LAYOUT_UNARY)
#if defined(LUAGLM_ALIGNED)
MATRIX_DEFN(matrixCompMult, glm::__matrixCompMult, LAYOUT_BINARY)
#else
MATRIX_DEFN(matrixCompMult, glm::matrixCompMult, LAYOUT_BINARY)
#endif
MATRIX_DEFN(transpose, glm::transpose, LAYOUT_UNARY)
GLM_BINDING_QUALIFIER(outerProduct) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VVECTOR2: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec2<>::fast); break;
    case LUA_VVECTOR3: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec3<>::fast); break;
    case LUA_VVECTOR4: LAYOUT_OUTERPRODUCT(LB, glm::outerProduct, gLuaVec4<>::fast); break;
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR);
  GLM_BINDING_END
}
#endif

#if defined(EXT_MATRIX_CLIP_SPACE_HPP)
using gAspect = gLuaBoundedBelow<gLuaCNum, false, true>;  // @GLMAssert: assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
using gFov = gLuaBoundedBelow<gLuaCNum, false, false>;  // @GLMAssert: assert(fov > static_cast<T>(0));
using gHeight = gFov;  // @GLMAssert: assert(height > static_cast<T>(0));
using gWidth = gFov;  // @GLMAssert: assert(width > static_cast<T>(0));
TRAITS_DEFN(frustum, glm::frustum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumLH, glm::frustumLH, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumLH_NO, glm::frustumLH_NO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumLH_ZO, glm::frustumLH_ZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumNO, glm::frustumNO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumRH, glm::frustumRH, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumRH_NO, glm::frustumRH_NO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumRH_ZO, glm::frustumRH_ZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(frustumZO, glm::frustumZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(infinitePerspective, glm::infinitePerspective, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(infinitePerspectiveLH, glm::infinitePerspectiveLH, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(infinitePerspectiveRH, glm::infinitePerspectiveRH, gLuaCNum, gLuaCNum, gLuaCNum)
GLM_BINDING_QUALIFIER(ortho) {
  GLM_BINDING_BEGIN
  if (gLuaCNum::Is(LB.L, LB.idx + 4))
    BIND_FUNC(LB, glm::ortho, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum);
  BIND_FUNC(LB, glm::ortho, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum);
  GLM_BINDING_END
}
TRAITS_DEFN(orthoLH, glm::orthoLH, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoLH_NO, glm::orthoLH_NO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoLH_ZO, glm::orthoLH_ZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoNO, glm::orthoNO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoRH, glm::orthoRH, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoRH_NO, glm::orthoRH_NO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoRH_ZO, glm::orthoRH_ZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orthoZO, glm::orthoZO, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspective, glm::perspective, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveLH, glm::perspectiveLH, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveLH_NO, glm::perspectiveLH_NO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveLH_ZO, glm::perspectiveLH_ZO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveNO, glm::perspectiveNO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveRH, glm::perspectiveRH, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveRH_NO, glm::perspectiveRH_NO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveRH_ZO, glm::perspectiveRH_ZO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveZO, glm::perspectiveZO, gLuaCNum, gAspect, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFov, glm::perspectiveFov, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovLH, glm::perspectiveFovLH, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovLH_NO, glm::perspectiveFovLH_NO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovLH_ZO, glm::perspectiveFovLH_ZO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovNO, glm::perspectiveFovNO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovRH, glm::perspectiveFovRH, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovRH_NO, glm::perspectiveFovRH_NO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovRH_ZO, glm::perspectiveFovRH_ZO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
TRAITS_DEFN(perspectiveFovZO, glm::perspectiveFovZO, gFov, gWidth, gHeight, gLuaCNum, gLuaCNum)
GLM_BINDING_QUALIFIER(tweakedInfinitePerspective) {
  GLM_BINDING_BEGIN
  if (gLuaCNum::Is(LB.L, LB.idx + 3))
    BIND_FUNC(LB, glm::tweakedInfinitePerspective, gLuaCNum, gLuaCNum, gLuaCNum, gLuaCNum);
  else
    BIND_FUNC(LB, glm::tweakedInfinitePerspective, gLuaCNum, gLuaCNum, gLuaCNum);
  GLM_BINDING_END
}
#endif

#if defined(EXT_MATRIX_TRANSFORM_HPP) || defined(GTX_MATRIX_TRANSFORM_2D_HPP)
GLM_BINDING_QUALIFIER(identity) {
  GLM_BINDING_BEGIN
  const lua_Integer size = gLuaInteger::Next(LB.L, LB.idx);
  const lua_Integer secondary = gLuaInteger::Next(LB.L, LB.idx);
  switch (LUAGLM_MATRIX_TYPE(size, secondary)) {
    case LUAGLM_MATRIX_2x2: return gLuaBase::Push(LB, glm::identity<gLuaMat2x2<>::type>());
    case LUAGLM_MATRIX_2x3: return gLuaBase::Push(LB, glm::identity<gLuaMat2x3<>::type>());
    case LUAGLM_MATRIX_2x4: return gLuaBase::Push(LB, glm::identity<gLuaMat2x4<>::type>());
    case LUAGLM_MATRIX_3x2: return gLuaBase::Push(LB, glm::identity<gLuaMat3x2<>::type>());
    case LUAGLM_MATRIX_3x3: return gLuaBase::Push(LB, glm::identity<gLuaMat3x3<>::type>());
    case LUAGLM_MATRIX_3x4: return gLuaBase::Push(LB, glm::identity<gLuaMat3x4<>::type>());
    case LUAGLM_MATRIX_4x2: return gLuaBase::Push(LB, glm::identity<gLuaMat4x2<>::type>());
    case LUAGLM_MATRIX_4x3: return gLuaBase::Push(LB, glm::identity<gLuaMat4x3<>::type>());
    case LUAGLM_MATRIX_4x4: return gLuaBase::Push(LB, glm::identity<gLuaMat4x4<>::type>());
    default: {
      break;
    }
  }
  return luaL_error(LB.L, GLM_INVALID_MAT_DIMENSIONS);
  GLM_BINDING_END
}

TRAITS_DEFN(lookAt, glm::lookAt, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
TRAITS_DEFN(lookAtLH, glm::lookAtLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
TRAITS_DEFN(lookAtRH, glm::lookAtRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>)
TRAITS_DEFN(lookRotation, glm::lookRotation, gLuaDir3<>, gLuaDir3<>)  // LUA_MATRIX_EXTENSIONS
TRAITS_DEFN(lookRotationRH, glm::lookRotationRH, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(lookRotationLH, glm::lookRotationLH, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(billboard, glm::billboard, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(billboardRH, glm::billboardRH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(billboardLH, glm::billboardLH, gLuaVec3<>, gLuaVec3<>, gLuaDir3<>, gLuaDir3<>)
#endif

#if defined(EXT_MATRIX_PROJECTION_HPP)
// @GLMAssert: assert(delta.x > static_cast<T>(0) && delta.y > static_cast<T>(0));
// glm::pickMatrix also sanitizes the parameters without asserts; a bit redundant.
using gPickDeltaValue = gLuaBoundedBelow<gLuaVec2<>, false>;
TRAITS_DEFN(pickMatrix, glm::pickMatrix, gLuaVec2<>, gPickDeltaValue, gLuaVec4<>)
TRAITS_DEFN(project, glm::project, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(projectNO, glm::projectNO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(projectZO, glm::projectZO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(unProject, glm::unProject, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(unProjectNO, glm::unProjectNO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(unProjectZO, glm::unProjectZO, gLuaVec3<>, gLuaMat4x4<>, gLuaMat4x4<>, gLuaVec4<>)
TRAITS_DEFN(rayPicking, glm::rayPicking, gLuaVec3<>, gLuaVec3<>, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat, gLuaFloat)  // LUA_VECTOR_EXTENSIONS
TRAITS_DEFN(containsProjection, glm::containsProjection, gLuaMat4x4<>, gLuaMat4x4<>::eps_trait)  // LUA_MATRIX_EXTENSIONS
#endif

#if defined(GTC_MATRIX_ACCESS_HPP)  // @NOTE: These GLM functions are zero-based
#define LAYOUT_MATRIX_ACCESS_COLUMN(LB, F, Tr, ...) \
  LAYOUT_MATRIX_ACCESS(LB, F, Tr, Tr::col_type, Tr::row_type, gLuaTrait<glm::length_t>, ##__VA_ARGS__)
#define LAYOUT_MATRIX_ACCESS_ROW(LB, F, Tr, ...) \
  LAYOUT_MATRIX_ACCESS(LB, F, Tr, Tr::row_type, Tr::col_type, gLuaTrait<glm::length_t>, ##__VA_ARGS__)
#define LAYOUT_MATRIX_ACCESS(LB, F, Tr, TrComp, TrDims, TrIdx, ...) \
  LUA_MLM_BEGIN                                                     \
  const lua_Integer _idx = luaL_checkinteger((LB).L, (LB).idx + 1); \
  if (_idx < 0 || _idx >= cast(lua_Integer, TrDims::length()))      \
    return GLM_ARG_ERROR((LB).L, (LB).idx + 1, "matrix index");     \
  else if (TrComp::Is((LB).L, (LB).idx + 2)) /* Set */              \
    VA_CALL(BIND_FUNC, LB, F, Tr, TrIdx, TrComp, ##__VA_ARGS__);    \
  else                                                              \
    VA_CALL(BIND_FUNC, LB, F, Tr, TrIdx, ##__VA_ARGS__); /* Get */  \
  LUA_MLM_END

MATRIX_DEFN(column, glm::column, LAYOUT_MATRIX_ACCESS_COLUMN)
MATRIX_DEFN(row, glm::row, LAYOUT_MATRIX_ACCESS_ROW)
#endif

#if defined(GTC_MATRIX_INVERSE_HPP)
BINARY_LAYOUT_DEFN(affineInverse, glm::affineInverse, LAYOUT_UNARY, gLuaMat3x3<>, gLuaMat4x4<>)
SYMMETRIC_MATRIX_DEFN(inverseTranspose, glm::inverseTranspose, LAYOUT_UNARY)
#endif

// The GLM implementation of extractEuler is not particularly complete.
#if defined(GTX_EULER_ANGLES_HPP)
#define LAYOUT_EULER_DECOMPOSE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                                \
  Tr::value_type a, b, c;                      \
  F(Tr::Next((LB).L, (LB).idx), a, b, c);      \
  TRAITS_PUSH(LB, a, b, c);                    \
  LUA_MLM_END

TRAITS_DEFN(derivedEulerAngleX, glm::derivedEulerAngleX, gLuaCNum, gLuaCNum)
TRAITS_DEFN(derivedEulerAngleY, glm::derivedEulerAngleY, gLuaCNum, gLuaCNum)
TRAITS_DEFN(derivedEulerAngleZ, glm::derivedEulerAngleZ, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleX, glm::eulerAngleX, gLuaCNum)
TRAITS_DEFN(eulerAngleXY, glm::eulerAngleXY, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleXYX, glm::eulerAngleXYX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleXYZ, glm::eulerAngleXYZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleXZ, glm::eulerAngleXZ, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleXZX, glm::eulerAngleXZX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleXZY, glm::eulerAngleXZY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleY, glm::eulerAngleY, gLuaCNum)
TRAITS_DEFN(eulerAngleYX, glm::eulerAngleYX, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleYXY, glm::eulerAngleYXY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleYXZ, glm::eulerAngleYXZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleYZ, glm::eulerAngleYZ, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleYZX, glm::eulerAngleYZX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleYZY, glm::eulerAngleYZY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZ, glm::eulerAngleZ, gLuaCNum)
TRAITS_DEFN(eulerAngleZX, glm::eulerAngleZX, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZXY, glm::eulerAngleZXY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZXZ, glm::eulerAngleZXZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZY, glm::eulerAngleZY, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZYX, glm::eulerAngleZYX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(eulerAngleZYZ, glm::eulerAngleZYZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(orientate2, glm::orientate2, gLuaCNum)
BINARY_LAYOUT_DEFN(orientate3, glm::orientate3, LAYOUT_UNARY, gLuaFloat, gLuaVec3<>::fast)
TRAITS_DEFN(orientate4, glm::orientate4, gLuaVec3<>)
TRAITS_DEFN(yawPitchRoll, glm::yawPitchRoll, gLuaCNum, gLuaCNum, gLuaCNum)
ROTATION_MATRIX_DEFN(extractEulerAngleXYX, glm::extractEulerAngleXYX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXYZ, glm::extractEulerAngleXYZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXZX, glm::extractEulerAngleXZX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleXZY, glm::extractEulerAngleXZY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYXY, glm::extractEulerAngleYXY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYXZ, glm::extractEulerAngleYXZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYZX, glm::extractEulerAngleYZX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleYZY, glm::extractEulerAngleYZY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZXY, glm::extractEulerAngleZXY, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZXZ, glm::extractEulerAngleZXZ, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZYX, glm::extractEulerAngleZYX, LAYOUT_EULER_DECOMPOSE)
ROTATION_MATRIX_DEFN(extractEulerAngleZYZ, glm::extractEulerAngleZYZ, LAYOUT_EULER_DECOMPOSE)
TRAITS_DEFN(quatEulerAngleX, glm::quatEulerAngleX, gLuaCNum)  // LUA_QUATERNION_EXTENSIONS
TRAITS_DEFN(quatEulerAngleXY, glm::quatEulerAngleXY, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleXYX, glm::quatEulerAngleXYX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleXYZ, glm::quatEulerAngleXYZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleXZ, glm::quatEulerAngleXZ, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleXZX, glm::quatEulerAngleXZX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleXZY, glm::quatEulerAngleXZY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleY, glm::quatEulerAngleY, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYX, glm::quatEulerAngleYX, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYXY, glm::quatEulerAngleYXY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYXZ, glm::quatEulerAngleYXZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYZ, glm::quatEulerAngleYZ, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYZX, glm::quatEulerAngleYZX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleYZY, glm::quatEulerAngleYZY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZ, glm::quatEulerAngleZ, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZX, glm::quatEulerAngleZX, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZXY, glm::quatEulerAngleZXY, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZXZ, glm::quatEulerAngleZXZ, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZY, glm::quatEulerAngleZY, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZYX, glm::quatEulerAngleZYX, gLuaCNum, gLuaCNum, gLuaCNum)
TRAITS_DEFN(quatEulerAngleZYZ, glm::quatEulerAngleZYZ, gLuaCNum, gLuaCNum, gLuaCNum)
#endif

#if defined(GTX_MATRIX_CROSS_PRODUCT_HPP)
TRAITS_DEFN(matrixCross3, glm::matrixCross3, gLuaVec3<>)
TRAITS_DEFN(matrixCross4, glm::matrixCross4, gLuaVec3<>)
#endif

#if defined(GTX_MATRIX_DECOMPOSE_HPP)
GLM_BINDING_QUALIFIER(decompose) {
  GLM_BINDING_BEGIN
  gLuaQuat<>::type orientation;
  gLuaVec4<>::type perspective;
  gLuaVec3<>::type scale, translation, skew;
  if (glm::decompose(gLuaMat4x4<>::Next(LB.L, LB.idx), scale, orientation, translation, skew, perspective))
    TRAITS_PUSH(LB, scale, orientation, translation, skew, perspective);
  return gLuaBase::Push(LB);
  GLM_BINDING_END
}
#endif

#if defined(GTX_MATRIX_FACTORISATION_HPP)
#define LAYOUT_QRDECOMPOSE(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                            \
  Tr::q_type::type q = Tr::q_type::zero(); \
  Tr::r_type::type r = Tr::r_type::zero(); \
  F(Tr::Next((LB).L, (LB).idx), q, r);     \
  TRAITS_PUSH(LB, q, r);                   \
  LUA_MLM_END

MATRIX_DEFN(fliplr, glm::fliplr, LAYOUT_UNARY)
MATRIX_DEFN(flipud, glm::flipud, LAYOUT_UNARY)
MATRIX_DEFN(qr_decompose, glm::qr_decompose, LAYOUT_QRDECOMPOSE)
MATRIX_DEFN(rq_decompose, glm::rq_decompose, LAYOUT_QRDECOMPOSE)
#endif

/* glm/gtx/matrix_interpolation.hpp */
#if defined(GTX_MATRIX_INTERPOLATION_HPP)
#define LAYOUT_AXIS_ANGLE(LB, F, Tr, ...)     \
  LUA_MLM_BEGIN                               \
  gLuaVec3<>::type axis;                      \
  gLuaVec3<>::value_type angle;               \
  F(Tr::Next((LB).L, (LB).idx), axis, angle); \
  TRAITS_PUSH(LB, axis, angle);               \
  LUA_MLM_END

TRAITS_DEFN(axisAngleMatrix, glm::axisAngleMatrix, gLuaDir3<>, gLuaFloat)
TRAITS_DEFN(extractMatrixRotation, glm::extractMatrixRotation, gLuaMat4x4<>)
TRAITS_DEFN(interpolate, glm::interpolate, gLuaMat4x4<>, gLuaMat4x4<>, gLuaFloat)
ROTATION_MATRIX_DEFN(axisAngle, glm::__axisAngle, LAYOUT_AXIS_ANGLE)
#endif

/* glm/gtx/matrix_major_storage.hpp */
#if defined(GTX_MATRIX_MAJOR_STORAGE_HPP)
#define MATRIX_MAJOR_DEFN(Name, F, ArgLayout, Tr)             \
  GLM_BINDING_QUALIFIER(Name) {                               \
    GLM_BINDING_BEGIN                                         \
    if (Tr::col_type::Is((LB).L, (LB).idx))                   \
      ArgLayout(LB, F, Tr::col_type);                         \
    return gLuaBase::Push(LB, F(Tr::Next((LB).L, (LB).idx))); \
    GLM_BINDING_END                                           \
  }

#define MAJOR(A, B) A##B
#define MATRIX_GENERAL_MAJOR_DEFN(Name, F)                                                                            \
  GLM_BINDING_QUALIFIER(Name) {                                                                                       \
    GLM_BINDING_BEGIN                                                                                                 \
    const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                                                    \
    switch (ttypetag(_tv)) {                                                                                          \
      case LUA_VVECTOR2: LAYOUT_BINARY(LB, MAJOR(F, 2), gLuaVec2<>::fast); break;                                     \
      case LUA_VVECTOR3: LAYOUT_TERNARY(LB, MAJOR(F, 3), gLuaVec3<>::fast); break;                                    \
      case LUA_VVECTOR4: LAYOUT_QUATERNARY(LB, MAJOR(F, 4), gLuaVec4<>::fast); break;                                 \
      case LUA_VMATRIX: {                                                                                             \
        switch (mvalue_dims(_tv)) {                                                                                   \
          case LUAGLM_MATRIX_2x2: return gLuaBase::Push(LB, MAJOR(F, 2)(gLuaMat2x2<>::fast::Next((LB).L, (LB).idx))); \
          case LUAGLM_MATRIX_3x3: return gLuaBase::Push(LB, MAJOR(F, 3)(gLuaMat3x3<>::fast::Next((LB).L, (LB).idx))); \
          case LUAGLM_MATRIX_4x4: return gLuaBase::Push(LB, MAJOR(F, 4)(gLuaMat4x4<>::fast::Next((LB).L, (LB).idx))); \
          default:                                                                                                    \
            break;                                                                                                    \
        }                                                                                                             \
        break;                                                                                                        \
      }                                                                                                               \
      default:                                                                                                        \
        break;                                                                                                        \
    }                                                                                                                 \
    return GLM_TYPE_ERROR((LB).L, (LB).idx, GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);                              \
    GLM_BINDING_END                                                                                                   \
  }

MATRIX_MAJOR_DEFN(colMajor2, glm::colMajor2, LAYOUT_BINARY, gLuaMat2x2<>)
MATRIX_MAJOR_DEFN(colMajor3, glm::colMajor3, LAYOUT_TERNARY, gLuaMat3x3<>)
MATRIX_MAJOR_DEFN(colMajor4, glm::colMajor4, LAYOUT_QUATERNARY, gLuaMat4x4<>)
MATRIX_MAJOR_DEFN(rowMajor2, glm::rowMajor2, LAYOUT_BINARY, gLuaMat2x2<>)
MATRIX_MAJOR_DEFN(rowMajor3, glm::rowMajor3, LAYOUT_TERNARY, gLuaMat3x3<>)
MATRIX_MAJOR_DEFN(rowMajor4, glm::rowMajor4, LAYOUT_QUATERNARY, gLuaMat4x4<>)
MATRIX_GENERAL_MAJOR_DEFN(colMajor, glm::colMajor)  // LUA_MATRIX_EXTENSIONS
MATRIX_GENERAL_MAJOR_DEFN(rowMajor, glm::rowMajor)
#endif

#if defined(GTX_MATRIX_OPERATION_HPP)
#if GLM_VERSION >= 993  // @COMPAT: Added in 0.9.9.3
SYMMETRIC_MATRIX_DEFN(adjugate, glm::adjugate, LAYOUT_UNARY)
#endif
TRAITS_DEFN(diagonal2x2, glm::diagonal2x3, gLuaVec2<>)
TRAITS_DEFN(diagonal2x3, glm::diagonal2x3, gLuaVec2<>)
TRAITS_DEFN(diagonal2x4, glm::diagonal2x4, gLuaVec2<>)
TRAITS_DEFN(diagonal3x2, glm::diagonal3x2, gLuaVec2<>)
TRAITS_DEFN(diagonal3x3, glm::diagonal3x3, gLuaVec3<>)
TRAITS_DEFN(diagonal3x4, glm::diagonal3x4, gLuaVec3<>)
TRAITS_DEFN(diagonal4x2, glm::diagonal4x2, gLuaVec2<>)
TRAITS_DEFN(diagonal4x3, glm::diagonal4x3, gLuaVec3<>)
TRAITS_DEFN(diagonal4x4, glm::diagonal4x4, gLuaVec4<>)
MATRIX_DEFN(diagonal, glm::diagonal, LAYOUT_UNARY)  // LUA_MATRIX_EXTENSIONS
#endif

#if defined(GTX_MATRIX_QUERY_HPP)
MATRIX_DEFN(isIdentity, glm::_isIdentity, LAYOUT_BINARY_EPS)
MATRIX_DEFN(isOrthogonal, glm::isOrthogonal, LAYOUT_BINARY_EPS)
ROTATION_MATRIX_DEFN(extractScale, glm::extractScale, LAYOUT_UNARY)  // LUA_MATRIX_EXTENSIONS
ROTATION_MATRIX_DEFN(hasUniformScale, glm::hasUniformScale, LAYOUT_BINARY_EPS)
#endif

#if defined(GTX_TRANSFORM2_HPP)
TRAITS_DEFN(proj2D, glm::proj2D, gLuaMat3x3<>, gLuaVec3<>)
TRAITS_DEFN(proj3D, glm::proj3D, gLuaMat4x4<>, gLuaVec3<>)
TRAITS_DEFN(shearX2D, glm::shearX2D, gLuaMat3x3<>, gLuaFloat)
TRAITS_DEFN(shearX3D, glm::shearX3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
TRAITS_DEFN(shearY2D, glm::shearY2D, gLuaMat3x3<>, gLuaFloat)
TRAITS_DEFN(shearY3D, glm::shearY3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
TRAITS_DEFN(shearZ3D, glm::shearZ3D, gLuaMat4x4<>, gLuaFloat, gLuaFloat)
GLM_BINDING_QUALIFIER(scaleBias) {
  GLM_BINDING_BEGIN
  if (gLuaMat4x4<>::fast::Is(LB.L, LB.idx))
    BIND_FUNC(LB, glm::__scaleBias, gLuaMat4x4<>::fast, gLuaFloat, gLuaFloat);

  const gLuaCNum::type a = gLuaCNum::Next(LB.L, LB.idx);
  const gLuaCNum::type b = gLuaCNum::Next(LB.L, LB.idx);
  return gLuaBase::Push(LB, glm::__scaleBias<typename gLuaCNum::type, glm::qualifier::defaultp>(a, b));
  GLM_BINDING_END
}
#endif

#if defined(GTX_MATRIX_TRANSFORM_2D_HPP)
TRAITS_DEFN(shearX, glm::shearX, gLuaMat3x3<>, gLuaMat3x3<>::value_trait)
TRAITS_DEFN(shearY, glm::shearY, gLuaMat3x3<>, gLuaMat3x3<>::value_trait)
#endif

#if defined(GTX_PCA_HPP)  // @COMPAT: pca.hpp introduced in 0.9.9.9
#include <glm/gtx/pca.hpp>
#define LAYOUT_FIND_EIGEN(LB, F, Tr, ...)                                                    \
  LUA_MLM_BEGIN                                                                              \
  Tr::type outVectors = Tr::zero();                                                          \
  Tr::col_type::type outValues = Tr::col_type::zero();                                       \
  glm::length_t count = glm::length_t(F(Tr::Next((LB).L, (LB).idx), outValues, outVectors)); \
  if (outValues.length() == count)                                                           \
    glm::sortEigenvalues(outValues, outVectors);                                             \
  TRAITS_PUSH(LB, count, outValues, outVectors);                                             \
  LUA_MLM_END

#define LAYOUT_COMPUTE_COVARIANCE(LB, F, Mat, Cols, ...)               \
  LUA_MLM_BEGIN                                                        \
  using Vec = Mat::col_type;                                           \
  glmLuaArray<Vec> lArray((LB).L, (LB).idx++);                         \
  if (Vec::fast::Is((LB).L, (LB).idx)) {                               \
    return gLuaBase::Push(LB, F<Cols, Mat::value_type, glm::defaultp>( \
      lArray.begin(), lArray.end(), Vec::fast::Next((LB).L, (LB).idx)  \
    ));                                                                \
  }                                                                    \
  else {                                                               \
    return gLuaBase::Push(LB, F<Cols, Mat::value_type, glm::defaultp>( \
      lArray.begin(), lArray.end()                                     \
    ));                                                                \
  }                                                                    \
  LUA_MLM_END

SYMMETRIC_MATRIX_DEFN(findEigenvaluesSymReal, glm::findEigenvaluesSymReal, LAYOUT_FIND_EIGEN)
GLM_BINDING_QUALIFIER(computeCovarianceMatrix) {
  GLM_BINDING_BEGIN
  luaL_checktype(L, LB.idx, LUA_TTABLE);
  lua_rawgeti(LB.L, LB.idx, 1);  // Determine array dimensions
  const glm::length_t dimensions = glm_vector_length(LB.L, -1);
  lua_pop(L, 1);

  switch (dimensions) {
    case 2: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat2x2<>, 2); break;
    case 3: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat3x3<>, 3); break;
    case 4: LAYOUT_COMPUTE_COVARIANCE(LB, glm::computeCovarianceMatrix, gLuaMat4x4<>, 4); break;
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, "vector array");
  GLM_BINDING_END
}
#endif

/* }================================================================== */

/*
** {==================================================================
** OpenGL Mathematics API (everything else)
** ===================================================================
*/

#if defined(COMMON_HPP)
#define LAYOUT_FREXP(LB, F, Tr, ...)                     \
  LUA_MLM_BEGIN                                          \
  Tr::as_type<int>::type v2;                             \
  const Tr::type v3 = F(Tr::Next((LB).L, (LB).idx), v2); \
  TRAITS_PUSH(LB, v3, v2);                               \
  LUA_MLM_END

#define LAYOUT_MODF(LB, F, Tr, ...)                      \
  LUA_MLM_BEGIN                                          \
  Tr::type v2;                                           \
  const Tr::type v3 = F(Tr::Next((LB).L, (LB).idx), v2); \
  const int _a = gLuaBase::PushNumInt(LB, v2);           \
  const int _b = gLuaBase::Push(LB, v3);                 \
  return _a + _b;                                        \
  LUA_MLM_END

#define LAYOUT_UNARY_NUMINT(LB, F, Tr, ...) \
  return gLuaBase::PushNumInt(LB, F(Tr::Next((LB).L, (LB).idx)));

INTEGER_NUMBER_VECTOR_DEFN(abs, glm::abs, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fract, glm::fract, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
TRAITS_DEFN(fma, glm::fma, gLuaNumber, gLuaNumber, gLuaNumber)
#else
NUMBER_VECTOR_DEFN(fma, glm::fma, LAYOUT_TERNARY)
#endif
INTEGER_VECTOR_DEFN(floatBitsToInt, glm::floatBitsToInt, LAYOUT_UNARY, float)
INTEGER_VECTOR_DEFN(floatBitsToUint, glm::floatBitsToUint, LAYOUT_UNARY, float)
INTEGER_VECTOR_DEFN(intBitsToFloat, glm::intBitsToFloat, LAYOUT_UNARY, int)
INTEGER_VECTOR_DEFN(uintBitsToFloat, glm::uintBitsToFloat, LAYOUT_UNARY, unsigned)
NUMBER_VECTOR_QUAT_DEFN(isinf, glm::isinf, LAYOUT_UNARY) /* glm/ext/quaternion_common.hpp */
NUMBER_VECTOR_QUAT_DEFN(isnan, glm::isnan, LAYOUT_UNARY) /* glm/ext/quaternion_common.hpp */
NUMBER_VECTOR_DEFN(round, glm::round, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(roundEven, glm::roundEven, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(smoothstep, glm::smoothstep, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(step, glm::step, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(trunc, glm::trunc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(ldexp, glm::ldexp, LAYOUT_VECTOR_INT)
NUMBER_VECTOR_DEFN(frexp, glm::frexp, LAYOUT_FREXP, int)
NUMBER_VECTOR_DEFN(reverse, glm::reverse, LAYOUT_UNARY)  // LUA_VECTOR_EXTENSIONS
INTEGER_NUMBER_VECTOR_DEFN(mod, glm::imod, LAYOUT_MODULO)  // lmathlib compat
INTEGER_NUMBER_VECTOR_DEFNS(ceil, glm::iceil, LAYOUT_UNARY, LAYOUT_UNARY_NUMINT, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFNS(floor, glm::ifloor, LAYOUT_UNARY, LAYOUT_UNARY_NUMINT, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(fdim, glm::fdim, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(hypot, glm::hypot, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(isnormal, glm::isnormal, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(isunordered, glm::isunordered, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(nearbyint, glm::nearbyint, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(nextafter, glm::nextafter, LAYOUT_BINARY)
// @TODO: nexttoward
NUMBER_VECTOR_DEFN(remainder, glm::remainder, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(scalbn, glm::scalbn, LAYOUT_VECTOR_INT)
#endif

/* lmathlib compatibility */
GLM_BINDING_QUALIFIER(modf) {
  GLM_BINDING_BEGIN
  if (lua_isinteger(LB.L, LB.idx)) {
    lua_pushvalue(LB.L, LB.idx); /* number is its own integer part */
    lua_pushnumber(L, 0); /* no fractional part */
    return 2;
  }
  PARSE_NUMBER_VECTOR(LB, glm::modf, LAYOUT_MODF, LAYOUT_MODF);
  GLM_BINDING_END
}

/* lmathlib compatibility */
GLM_BINDING_QUALIFIER(toint) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VVECTOR2: return gLuaBase::Push(LB, cast_vec2(glm_v2value(_tv), glm_Integer));
    case LUA_VVECTOR3: return gLuaBase::Push(LB, cast_vec3(glm_v3value(_tv), glm_Integer));
    case LUA_VVECTOR4: return gLuaBase::Push(LB, cast_vec4(glm_v4value(_tv), glm_Integer));
    default: {
      int valid = 0;
      const lua_Integer n = lua_tointegerx(LB.L, LB.idx, &valid);
      if (l_likely(valid))
        lua_pushinteger(LB.L, n);
      else {
        luaL_checkany(LB.L, 1);
        luaL_pushfail(LB.L); /* value is not convertible to integer */
      }
      return 1;
    }
  }
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(EXT_SCALAR_COMMON_HPP) || defined(EXT_VECTOR_COMMON_HPP)
/* Accumulation for min/max functions, where arguments can be Tr or a primitive */
#define LAYOUT_MINMAX(LB, F, Tr, ...)                                                        \
  LUA_MLM_BEGIN                                                                              \
  const int _n = (LB).top();                                                                 \
  Tr::type base = Tr::Next((LB).L, (LB).idx);                                                \
  while ((LB).idx <= _n) {                                                                   \
    if (Tr::safe::Is((LB).L, (LB).idx))                                                      \
      base = F(base, Tr::safe::Next((LB).L, (LB).idx));                                      \
    else if (Tr::value_trait::Is((LB).L, (LB).idx))                                          \
      base = F(base, Tr::value_trait::Next((LB).L, (LB).idx));                               \
    else                                                                                     \
      return luaL_error((LB).L, "%s or %s expected", Tr::Label(), Tr::value_trait::Label()); \
  }                                                                                          \
  return gLuaBase::Push(LB, base);                                                           \
  LUA_MLM_END

/* glm::clamp */
#define LAYOUT_CLAMP(LB, F, Tr, ...)                                                               \
  LUA_MLM_BEGIN /* <Tr, 0, 1>, <Tr, minVal, maxVal>, <Tr, TrMin, TrMax> */                         \
  if (lua_isnoneornil((LB).L, (LB).idx + 1) && lua_isnoneornil((LB).L, (LB).idx + 2))              \
    VA_CALL(BIND_FUNC, LB, F, Tr, ##__VA_ARGS__);                                                  \
  else if (Tr::value_trait::Is((LB).L, (LB).idx + 1) && Tr::value_trait::Is((LB).L, (LB).idx + 2)) \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, Tr::value_trait, ##__VA_ARGS__);                \
  else                                                                                             \
    LAYOUT_TERNARY(LB, F, Tr, ##__VA_ARGS__);                                                      \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(fmin, glm::fmin, LAYOUT_MINMAX)
NUMBER_VECTOR_DEFN(fmax, glm::fmax, LAYOUT_MINMAX)
NUMBER_VECTOR_DEFN(fclamp, glm::fclamp, LAYOUT_CLAMP)
GLM_BINDING_QUALIFIER(clamp) {
  GLM_BINDING_BEGIN
  if (gLuaInteger::Is(LB.L, LB.idx)) { /* support int-only values */
    if (gLuaInteger::Is(LB.L, LB.idx + 1) && gLuaInteger::Is(LB.L, LB.idx + 2))
      BIND_FUNC(LB, glm::clamp, gLuaInteger, gLuaInteger, gLuaInteger);
    else if (lua_isnoneornil(LB.L, LB.idx + 1) && lua_isnoneornil(LB.L, LB.idx + 2))
      BIND_FUNC(LB, glm::clamp, gLuaInteger);
  }
  PARSE_NUMBER_VECTOR(LB, glm::clamp, LAYOUT_CLAMP, LAYOUT_CLAMP);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(min) { /* lmathlib: static int math_min (lua_State *L) */
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle(); /* number of arguments */
  luaL_argcheck(L, n >= 1, 1, "value expected");

  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttisnumber(o) || cvt2num(o)) { /* string -> number coercion; follow lmathlib */
    int imin = 1; /* index of current minimum value */
    for (int i = 2; i <= n; i++) {
      if (lua_compare(L, i, imin, LUA_OPLT))
        imin = i;
    }
    lua_pushvalue(L, imin);
    return 1;
  }
  PARSE_NUMBER_VECTOR(LB, glm::min, LAYOUT_MINMAX, LAYOUT_MINMAX);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(max) { /* lmathlib: static int math_max (lua_State *L) */
  GLM_BINDING_BEGIN
  const int n = LB.top_for_recycle(); /* number of arguments */
  luaL_argcheck(L, n >= 1, 1, "value expected");

  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttisnumber(o) || cvt2num(o)) { /* string -> number coercion; follow lmathlib */
    int imax = 1; /* index of current maximum value */
    for (int i = 2; i <= n; i++) {
      if (lua_compare(L, imax, i, LUA_OPLT))
        imax = i;
    }
    lua_pushvalue(L, imax);
    return 1;
  }
  PARSE_NUMBER_VECTOR(LB, glm::max, LAYOUT_MINMAX, LAYOUT_MINMAX);
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(EXT_MATRIX_COMMON_HPP)
#define LAYOUT_MIX(LB, F, Tr, ...)                                           \
  LUA_MLM_BEGIN                                                              \
  if (gLuaTrait<bool>::Is((LB).L, (LB).idx + 2))                             \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, gLuaTrait<bool>, ##__VA_ARGS__); \
  else if (Tr::value_trait::Is((LB).L, (LB).idx + 2))                        \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__); \
  else                                                                       \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);        \
  LUA_MLM_END

GLM_BINDING_QUALIFIER(mix) {
  GLM_BINDING_BEGIN
#if GLM_VERSION >= 994  // @COMPAT: ext/matrix_common.hpp introduced in 0.9.9.4
  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttismatrix(o))
    PARSE_SYMMETRIC_MATRIX(LB, glm::__mix, LAYOUT_TERNARY_OPTIONAL);
  else
#endif
  PARSE_NUMBER_VECTOR_QUAT(LB, glm::mix, LAYOUT_MIX, LAYOUT_MIX, LAYOUT_TERNARY_SCALAR);
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}
#endif

#if defined(COMMON_HPP) || defined(GTX_LOG_BASE_HPP)
NUMBER_VECTOR_DEFN(sign, glm::sign, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(signP, glm::signP, LAYOUT_UNARY)  // LUA_VECTOR_EXTENSIONS
NUMBER_VECTOR_DEFN(signN, glm::signN, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(copysign, glm::copysign, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fpclassify, glm::fpclassify, LAYOUT_UNARY)
#endif
#endif

#if defined(EXPONENTIAL_HPP)
NUMBER_VECTOR_DEFN(exp2, glm::exp2, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(inversesqrt, glm::inversesqrt, LAYOUT_UNARY)
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(expm1, glm::expm1, LAYOUT_UNARY)
#endif
NUMBER_VECTOR_DEFN(logistic, glm::logistic, LAYOUT_UNARY)  // LUA_VECTOR_EXTENSIONS
#endif

#if defined(EXPONENTIAL_HPP) || defined(EXT_QUATERNION_EXPONENTIAL_HPP) || defined(GTX_LOG_BASE_HPP)
NUMBER_VECTOR_DEFN(log2, glm::log2, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(exp, glm::exp, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(sqrt, glm::sqrt, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFNS(log, glm::log, LAYOUT_UNARY_OR_BINARY, LAYOUT_UNARY_OR_BINARY, LAYOUT_UNARY)
GLM_BINDING_QUALIFIER(pow) {
  GLM_BINDING_BEGIN
  if (gLuaInteger::Is(LB.L, LB.idx) && gLuaTrait<unsigned>::Is(LB.L, LB.idx + 1))
    BIND_FUNC(LB, glm::pow, gLuaInteger, gLuaTrait<unsigned>);
  else
    PARSE_NUMBER_VECTOR_QUAT(LB, glm::pow, LAYOUT_BINARY_SCALAR, LAYOUT_BINARY_OPTIONAL, LAYOUT_BINARY_SCALAR);
  GLM_BINDING_END
}

#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(cbrt, glm::cbrt, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(log10, glm::log10, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(log1p, glm::log1p, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(logb, glm::logb, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(ilogb, glm::ilogb, LAYOUT_UNARY)
#endif
#endif

#if defined(EXPONENTIAL_HPP) || defined(GTX_EXTERIOR_PRODUCT_HPP) || defined(EXT_QUATERNION_GEOMETRIC_HPP)
GLM_BINDING_QUALIFIER(cross) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::cross, gLuaVec2<>::fast, gLuaVec2<>); break; /* glm/gtx/exterior_product.hpp */
    case LUA_VVECTOR3: {
      if (gLuaQuat<>::fast::Is(LB.L, LB.idx + 1))
        BIND_FUNC(LB, glm::cross, gLuaVec3<>::fast, gLuaQuat<>::fast); /* glm/gtx/quaternion.hpp */
      else
        BIND_FUNC(LB, glm::cross, gLuaVec3<>::fast, gLuaVec3<>); /* glm/geometric.hpp */
      break;
    }
    case LUA_VQUAT: {
      if (gLuaQuat<>::fast::Is(LB.L, LB.idx + 1)) /* glm/gtx/quaternion.hpp */
        BIND_FUNC(LB, glm::cross, gLuaQuat<>::fast, gLuaQuat<>::fast); /* <quat, quat> */
      else
        BIND_FUNC(LB, glm::cross, gLuaQuat<>::fast, gLuaVec3<>); /* <quat, vector3> */
      break;
    }
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN);
  GLM_BINDING_END
}
TRAITS_DEFN(crossXAxis, glm::crossXAxis, gLuaVec3<>)  // LUA_VECTOR_EXTENSIONS
TRAITS_DEFN(crossYAxis, glm::crossYAxis, gLuaVec3<>)
TRAITS_DEFN(crossZAxis, glm::crossZAxis, gLuaVec3<>)
TRAITS_DEFN(xAxisCross, glm::xAxisCross, gLuaVec3<>)
TRAITS_DEFN(yAxisCross, glm::yAxisCross, gLuaVec3<>)
TRAITS_DEFN(zAxisCross, glm::zAxisCross, gLuaVec3<>)
#endif

#if defined(GEOMETRIC_HPP)
NUMBER_VECTOR_DEFN(distance, glm::distance, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(faceforward, glm::faceforward, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(reflect, glm::reflect, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(refract, glm::refract, LAYOUT_TERNARY_SCALAR)
#endif

#if defined(GEOMETRIC_HPP) || defined(EXT_QUATERNION_GEOMETRIC_HPP)
NUMBER_VECTOR_QUAT_DEFN(dot, glm::dot, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(length, glm::length, LAYOUT_UNARY)
NUMBER_VECTOR_QUAT_DEFN(normalize, glm::normalize, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(clampLength, glm::clampLength, LAYOUT_BINARY_SCALAR)  // LUA_VECTOR_EXTENSIONS
NUMBER_VECTOR_DEFN(scaleLength, glm::scaleLength, LAYOUT_BINARY_SCALAR)
NUMBER_VECTOR_DEFN(direction, glm::direction, LAYOUT_BINARY)
#endif

/* glm/vector_relational.hpp */
#if defined(VECTOR_RELATIONAL_HPP)
INTEGER_VECTOR_DEFN(all, glm::all, LAYOUT_UNARY, bool)
INTEGER_VECTOR_DEFN(any, glm::any, LAYOUT_UNARY, bool)
INTEGER_VECTOR_DEFN(not_, glm::not_, LAYOUT_UNARY, bool)
#endif

#if defined(VECTOR_RELATIONAL_HPP) || defined(GTC_QUATERNION_HPP)
NUMBER_VECTOR_QUAT_DEFN(greaterThan, glm::greaterThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(greaterThanEqual, glm::greaterThanEqual, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(lessThan, glm::lessThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(lessThanEqual, glm::lessThanEqual, LAYOUT_BINARY)
INTEGER_VECTOR_DEFN(ult, glm::lessThan, LAYOUT_BINARY, lua_Unsigned)  // lmathlib
INTEGER_VECTOR_DEFN(ulte, glm::lessThanEqual, LAYOUT_BINARY, lua_Unsigned)
#if defined(LUAGLM_ALIASES_O3DE)
NUMBER_VECTOR_QUAT_DEFN(all_greaterThan, glm::all_greaterThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_greaterThanEqual, glm::all_greaterThanEqual, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_lessThan, glm::all_lessThan, LAYOUT_BINARY)
NUMBER_VECTOR_QUAT_DEFN(all_lessThanEqual, glm::all_lessThanEqual, LAYOUT_BINARY)
#endif
#endif

#if defined(TRIGONOMETRIC_HPP)
#define LAYOUT_SINCOS(LB, F, Tr, ...)  \
  LUA_MLM_BEGIN                        \
  Tr::type s, c;                       \
  F(Tr::Next((LB).L, (LB).idx), s, c); \
  TRAITS_PUSH(LB, s, c);               \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(acos, glm::acos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acosh, glm::acosh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asin, glm::asin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asinh, glm::asinh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(atan, glm::atan, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(atanh, glm::atanh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cos, glm::cos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cosh, glm::cosh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(radians, glm::radians, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(degrees, glm::degrees, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sin, glm::sin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sinh, glm::sinh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tan, glm::tan, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tanh, glm::tanh, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sincos, glm::sincos, LAYOUT_SINCOS)  // LUA_VECTOR_EXTENSIONS
#endif

#if defined(EXT_SCALAR_INTEGER_HPP) || defined(EXT_VECTOR_INTEGER_HPP)
#if GLM_VERSION >= 999  // @COMPAT: isMultiple fixed in 0.9.9.9
INTEGER_VECTOR_DEFN(isMultiple, glm::isMultiple, LAYOUT_BINARY_SCALAR, lua_Unsigned)
#endif
#if GLM_VERSION >= 996  // @COMPAT: Fixed in 0.9.9.6
INTEGER_VECTOR_DEFN(isPowerOfTwo, glm::isPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
#else
TRAITS_DEFN(isPowerOfTwo, glm::isPowerOfTwo, gLuaInteger)
#endif
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
INTEGER_VECTOR_DEFN(nextMultiple, glm::nextMultiple, LAYOUT_BINARY_OPTIONAL, lua_Unsigned)
INTEGER_VECTOR_DEFN(nextPowerOfTwo, glm::nextPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
INTEGER_VECTOR_DEFN(prevMultiple, glm::prevMultiple, LAYOUT_BINARY_OPTIONAL, lua_Unsigned)
INTEGER_VECTOR_DEFN(prevPowerOfTwo, glm::prevPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
#endif
#endif

#if defined(GTC_EPSILON_HPP)
#define LAYOUT_EPSILON_EQUAL(LB, F, Tr, ...)                               \
  LUA_MLM_BEGIN                                                            \
  if (Tr::Is((LB).L, (LB).idx + 2))                                        \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::safe, ##__VA_ARGS__);      \
  else                                                                     \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::eps_trait, ##__VA_ARGS__); \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(epsilonEqual, glm::epsilonEqual, LAYOUT_EPSILON_EQUAL)
NUMBER_VECTOR_DEFN(epsilonNotEqual, glm::epsilonNotEqual, LAYOUT_EPSILON_EQUAL)
#endif

#if defined(GTC_INTEGER_HPP)
// @GLMAssert: assert(static_cast<genType>(0.0) <= x);
// @GLMAssert: assert(all(lessThanEqual(vec<L, T, Q>(0), x)));
#define LAYOUT_ROUND_BOUNDED(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, gLuaBoundedBelow<Tr>, ##__VA_ARGS__)
NUMBER_VECTOR_DEFN(iround, glm::iround, LAYOUT_ROUND_BOUNDED)
NUMBER_VECTOR_DEFN(uround, glm::uround, LAYOUT_ROUND_BOUNDED)
#endif

#if defined(GTC_RANDOM_HPP)
using gRandValue = gLuaBoundedBelow<gLuaNumber, false>;  // @GLMAssert: assert(Radius > static_cast<T>(0));
NUMBER_VECTOR_DEFN(linearRand, glm::linearRand, LAYOUT_BINARY)
TRAITS_DEFN(ballRand, glm::ballRand, gRandValue)
TRAITS_DEFN(circularRand, glm::circularRand, gRandValue)
TRAITS_DEFN(diskRand, glm::diskRand, gRandValue)
TRAITS_DEFN(gaussRand, glm::gaussRand, gRandValue, gRandValue)
TRAITS_DEFN(sphericalRand, glm::sphericalRand, gRandValue)
#endif

#if defined(GTC_RECIPROCAL_HPP)
NUMBER_VECTOR_DEFN(acot, glm::acot, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acoth, glm::acoth, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acsc, glm::acsc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(acsch, glm::acsch, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asec, glm::asec, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(asech, glm::asech, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(cot, glm::cot, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(coth, glm::coth, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(csc, glm::csc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(csch, glm::csch, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sec, glm::sec, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(sech, glm::sech, LAYOUT_UNARY)
#endif

#if defined(GTC_ROUND_HPP)
INTEGER_NUMBER_VECTOR_DEFN(ceilMultiple, glm::ceilMultiple, LAYOUT_MODULO)
INTEGER_NUMBER_VECTOR_DEFN(floorMultiple, glm::floorMultiple, LAYOUT_MODULO)
INTEGER_NUMBER_VECTOR_DEFN(roundMultiple, glm::roundMultiple, LAYOUT_MODULO)
INTEGER_VECTOR_DEFN(ceilPowerOfTwo, glm::ceilPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
INTEGER_VECTOR_DEFN(floorPowerOfTwo, glm::floorPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
INTEGER_VECTOR_DEFN(roundPowerOfTwo, glm::roundPowerOfTwo, LAYOUT_UNARY, lua_Unsigned)
NUMBER_VECTOR_DEFN(snap, glm::snap, LAYOUT_BINARY)
#endif

#if defined(GTC_COLOR_SPACE_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
NUMBER_VECTOR_DEFN(convertLinearToSRGB, glm::convertLinearToSRGB, LAYOUT_UNARY_OPTIONAL)
NUMBER_VECTOR_DEFN(convertSRGBToLinear, glm::convertSRGBToLinear, LAYOUT_UNARY_OPTIONAL)
#endif

#if defined(GTC_NOISE_HPP)
NUMBER_VECTOR_DEFN(perlin, glm::perlin, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(simplex, glm::simplex, LAYOUT_UNARY)
#endif

#if defined(GTX_ASSOCIATED_MIN_MAX_HPP)
#define LAYOUT_ASSOCIATED_OPTIONAL(LB, F, Tr, ...)                                     \
  LUA_MLM_BEGIN                                                                        \
  using S = Tr::safe;                                                                  \
  if (Tr::Is((LB).L, (LB).idx + 6)) /* f(x,a,y,b,z,c,w,d) */                           \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, S, S, S, S, ##__VA_ARGS__);                 \
  else if (Tr::Is((LB).L, (LB).idx + 4)) /* f(x,a,y,b,z,c) */                          \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, S, S, ##__VA_ARGS__);                       \
  else if (gLuaTrait<bool>::Is(((LB)).L, ((LB)).idx + 1)) /* f(x,a,y,b) */             \
    VA_CALL(BIND_FUNC, LB, F, Tr, gLuaTrait<bool>, S, gLuaTrait<bool>, ##__VA_ARGS__); \
  else if (Tr::value_trait::Is(((LB)).L, ((LB)).idx + 1))                              \
    VA_CALL(BIND_FUNC, LB, F, Tr, Tr::value_trait, S, Tr::value_trait, ##__VA_ARGS__); \
  else                                                                                 \
    VA_CALL(BIND_FUNC, LB, F, Tr, S, S, S, ##__VA_ARGS__);                             \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(associatedMin, glm::associatedMin, LAYOUT_ASSOCIATED_OPTIONAL)
NUMBER_VECTOR_DEFN(associatedMax, glm::associatedMax, LAYOUT_ASSOCIATED_OPTIONAL)
#endif

#if defined(GTX_CLOSEST_POINT_HPP)
NUMBER_VECTOR_DEFN(closestPointOnLine, glm::closestPointOnLine, LAYOUT_TERNARY)
#endif

#if defined(GTX_COLOR_ENCODING_HPP)
TRAITS_DEFN(convertD65XYZToD50XYZ, glm::convertD65XYZToD50XYZ, gLuaVec3<>)
TRAITS_DEFN(convertD65XYZToLinearSRGB, glm::convertD65XYZToLinearSRGB, gLuaVec3<>)
TRAITS_DEFN(convertLinearSRGBToD50XYZ, glm::convertLinearSRGBToD50XYZ, gLuaVec3<>)
TRAITS_DEFN(convertLinearSRGBToD65XYZ, glm::convertLinearSRGBToD65XYZ, gLuaVec3<>)
#endif

#if defined(GTX_COLOR_SPACE_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
TRAITS_DEFN(hsvColor, glm::hsvColor, gLuaVec3<float>)
TRAITS_DEFN(luminosity, glm::luminosity, gLuaVec3<>)
TRAITS_DEFN(rgbColor, glm::rgbColor, gLuaVec3<>)
GLM_BINDING_QUALIFIER(saturation) {
  GLM_BINDING_BEGIN
  const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);
  if (!_isvalid(LB.L, _tv2)) BIND_FUNC(LB, glm::saturation, gLuaCNum);
  else if (ttisvector3(_tv2)) BIND_FUNC(LB, glm::saturation, gLuaFloat, gLuaVec3<>::fast);
  else if (ttisvector4(_tv2)) BIND_FUNC(LB, glm::saturation, gLuaFloat, gLuaVec4<>::fast);
  return GLM_TYPE_ERROR(LB.L, LB.idx + 1, GLM_STRING_NUMBER " or " GLM_STRING_VECTOR);
  GLM_BINDING_END
}
#endif

#if defined(GTX_COLOR_SPACE_YCOCG_HPP) && !defined(GLM_FORCE_XYZW_ONLY)
TRAITS_DEFN(rgb2YCoCg, glm::rgb2YCoCg, gLuaVec3<>)
TRAITS_DEFN(rgb2YCoCgR, glm::rgb2YCoCgR, gLuaVec3<>)
TRAITS_DEFN(YCoCg2rgb, glm::YCoCg2rgb, gLuaVec3<>)
TRAITS_DEFN(YCoCgR2rgb, glm::YCoCgR2rgb, gLuaVec3<>)
#endif

#if defined(GTX_COMMON_HPP)
NUMBER_VECTOR_DEFN(closeBounded, glm::closeBounded, LAYOUT_TERNARY)
NUMBER_VECTOR_DEFN(isdenormal, glm::isdenormal, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(openBounded, glm::openBounded, LAYOUT_TERNARY)
INTEGER_NUMBER_VECTOR_DEFN(fmod, glm::fmod, LAYOUT_MODULO)
#endif

#if defined(GTX_COMPATIBILITY_HPP)
NUMBER_VECTOR_DEFN(isfinite, glm::isfinite, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(atan2, glm::atan2, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(saturate, glm::saturate, LAYOUT_UNARY)
#if defined(LUAGLM_ALIASES_O3DE)
NUMBER_VECTOR_DEFN(all_isfinite, glm::all_isfinite, LAYOUT_UNARY)
#endif
#endif

#if defined(GTX_COMPATIBILITY_HPP) || defined(EXT_QUATERNION_COMMON_HPP)
// @GLMAssert: assert(a >= static_cast<T>(0));
// @GLMAssert: assert(a <= static_cast<T>(1));
#define LAYOUT_QUAT_LERP(LB, F, Tr, ...) \
  VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, gLuaBoundedBetween<Tr::value_trait>, ##__VA_ARGS__)
NUMBER_VECTOR_QUAT_DEFNS(lerp, glm::lerp, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_OPTIONAL, LAYOUT_QUAT_LERP)
NUMBER_VECTOR_QUAT_DEFNS(nlerp, glm::nlerp, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_OPTIONAL, LAYOUT_TERNARY_SCALAR)
NUMBER_VECTOR_DEFN(lerpinverse, glm::lerpinverse, LAYOUT_TERNARY_OPTIONAL)
#endif

#if defined(GTX_COMPONENT_WISE_HPP)
INTEGER_NUMBER_VECTOR_DEFN(compAdd, glm::compAdd, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMax, glm::compMax, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMin, glm::compMin, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(compMul, glm::compMul, LAYOUT_UNARY)
INTEGER_VECTOR_DEFN(compNormalize, glm::compNormalize<glm_Float>, LAYOUT_UNARY, glm_Integer)
NUMBER_VECTOR_DEFN(compScale, glm::compScale<glm_Integer>, LAYOUT_UNARY)
/* glm::compNormalize/glm::compScale for different types, e.g., u8, i8, u16, i16 */
// INTEGER_VECTOR_DEFN(compNormalize_i8, glm::compNormalize<glm_Float>, LAYOUT_UNARY, glm::i8)
// INTEGER_VECTOR_DEFN(compNormalize_u8, glm::compNormalize<glm_Float>, LAYOUT_UNARY, glm::u8)
// INTEGER_VECTOR_DEFN(compNormalize_i16, glm::compNormalize<glm_Float>, LAYOUT_UNARY, glm::i16)
// INTEGER_VECTOR_DEFN(compNormalize_u16, glm::compNormalize<glm_Float>, LAYOUT_UNARY, glm::u16)
// NUMBER_VECTOR_DEFN(compScale_i8, glm::compScale<glm::i8>, LAYOUT_UNARY)
// NUMBER_VECTOR_DEFN(compScale_u8, glm::compScale<glm::u8>, LAYOUT_UNARY)
// NUMBER_VECTOR_DEFN(compScale_i16, glm::compScale<glm::i16>, LAYOUT_UNARY)
// NUMBER_VECTOR_DEFN(compScale_u16, glm::compScale<glm::u16>, LAYOUT_UNARY)
#endif

#if defined(GTX_EASING_HPP)
// @GLMAssert: assert(a >= zero<genType>());
// @GLMAssert: assert(a <= one<genType>());
using gEasingValue = gLuaBoundedBetween<gLuaNumber>;
LAYOUT_DEFN(backEaseIn, glm::backEaseIn, LAYOUT_UNARY_OR_BINARY, gEasingValue)
LAYOUT_DEFN(backEaseInOut, glm::backEaseInOut, LAYOUT_UNARY_OR_BINARY, gEasingValue)
LAYOUT_DEFN(backEaseOut, glm::backEaseOut, LAYOUT_UNARY_OR_BINARY, gEasingValue)
TRAITS_DEFN(bounceEaseIn, glm::bounceEaseIn, gEasingValue)
TRAITS_DEFN(bounceEaseInOut, glm::bounceEaseInOut, gEasingValue)
TRAITS_DEFN(bounceEaseOut, glm::bounceEaseOut, gEasingValue)
TRAITS_DEFN(circularEaseIn, glm::circularEaseIn, gEasingValue)
TRAITS_DEFN(circularEaseInOut, glm::circularEaseInOut, gEasingValue)
TRAITS_DEFN(circularEaseOut, glm::circularEaseOut, gEasingValue)
TRAITS_DEFN(cubicEaseIn, glm::cubicEaseIn, gEasingValue)
TRAITS_DEFN(cubicEaseInOut, glm::cubicEaseInOut, gEasingValue)
TRAITS_DEFN(cubicEaseOut, glm::cubicEaseOut, gEasingValue)
TRAITS_DEFN(elasticEaseIn, glm::elasticEaseIn, gEasingValue)
TRAITS_DEFN(elasticEaseInOut, glm::elasticEaseInOut, gEasingValue)
TRAITS_DEFN(elasticEaseOut, glm::elasticEaseOut, gEasingValue)
TRAITS_DEFN(exponentialEaseIn, glm::exponentialEaseIn, gEasingValue)
TRAITS_DEFN(exponentialEaseInOut, glm::exponentialEaseInOut, gEasingValue)
TRAITS_DEFN(exponentialEaseOut, glm::exponentialEaseOut, gEasingValue)
TRAITS_DEFN(linearInterpolation, glm::linearInterpolation, gEasingValue)
TRAITS_DEFN(quadraticEaseIn, glm::quadraticEaseIn, gEasingValue)
TRAITS_DEFN(quadraticEaseInOut, glm::quadraticEaseInOut, gEasingValue)
TRAITS_DEFN(quadraticEaseOut, glm::quadraticEaseOut, gEasingValue)
TRAITS_DEFN(quarticEaseIn, glm::quarticEaseIn, gEasingValue)
TRAITS_DEFN(quarticEaseInOut, glm::quarticEaseInOut, gEasingValue)
TRAITS_DEFN(quarticEaseOut, glm::quarticEaseOut, gEasingValue)
TRAITS_DEFN(quinticEaseIn, glm::quinticEaseIn, gEasingValue)
TRAITS_DEFN(quinticEaseInOut, glm::quinticEaseInOut, gEasingValue)
TRAITS_DEFN(quinticEaseOut, glm::quinticEaseOut, gEasingValue)
TRAITS_DEFN(sineEaseIn, glm::sineEaseIn, gEasingValue)
TRAITS_DEFN(sineEaseInOut, glm::sineEaseInOut, gEasingValue)
TRAITS_DEFN(sineEaseOut, glm::sineEaseOut, gEasingValue)
#endif

#if defined(GTX_EXTEND_HPP)
NUMBER_VECTOR_DEFN(extend, glm::extend, LAYOUT_TERNARY)
#endif

#if defined(GTX_FAST_EXPONENTIAL_HPP)
NUMBER_VECTOR_DEFN(fastExp, glm::fastExp, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLog, glm::fastLog, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastPow, glm::fastPow, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fastExp2, glm::fastExp2, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLog2, glm::fastLog2, LAYOUT_UNARY)
#endif

#if defined(GTX_FAST_SQUARE_ROOT_HPP)
NUMBER_VECTOR_DEFN(fastDistance, glm::fastDistance, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(fastInverseSqrt, glm::fastInverseSqrt, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastLength, glm::fastLength, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastSqrt, glm::fastSqrt, LAYOUT_UNARY)
#if GLM_VERSION >= 999  // @COMPAT: fastNormalize ambiguity fixed in 0.9.9.9
NUMBER_VECTOR_QUAT_DEFN(fastNormalize, glm::fastNormalize, LAYOUT_UNARY)
#endif
#endif

#if defined(GTX_FAST_TRIGONOMETRY_HPP)
NUMBER_VECTOR_DEFN(fastAcos, glm::fastAcos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastAsin, glm::fastAsin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastAtan, glm::fastAtan, LAYOUT_UNARY_OR_BINARY)
NUMBER_VECTOR_DEFN(fastCos, glm::fastCos, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastSin, glm::fastSin, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(fastTan, glm::fastTan, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(wrapAngle, glm::wrapAngle, LAYOUT_UNARY)
#endif

#if defined(GTX_FUNCTIONS_HPP)
#define LAYOUT_SMOOTH_DAMP(LB, F, Tr, ...)                           \
  LUA_MLM_BEGIN                                                      \
  const Tr::type c = Tr::Next((LB).L, (LB).idx);                     \
  const Tr::safe::type t = Tr::safe::Next((LB).L, (LB).idx);         \
  Tr::safe::type cv = Tr::safe::Next((LB).L, (LB).idx);              \
  const Tr::value_type st = Tr::value_trait::Next((LB).L, (LB).idx); \
  const Tr::value_type ms = Tr::value_trait::Next((LB).L, (LB).idx); \
  const Tr::value_type dt = Tr::value_trait::Next((LB).L, (LB).idx); \
  const Tr::type result = F(c, t, cv, st, ms, dt);                   \
  TRAITS_PUSH(LB, result, cv);                                       \
  LUA_MLM_END

BINARY_LAYOUT_DEFN(gauss, glm::gauss, LAYOUT_TERNARY, gLuaNumber, gLuaVec2<>::fast)
NUMBER_VECTOR_DEFN(smoothDamp, glm::smoothDamp, LAYOUT_SMOOTH_DAMP)  // LUA_VECTOR_EXTENSIONS
NUMBER_VECTOR_DEFN(moveTowards, glm::moveTowards, LAYOUT_TERNARY_SCALAR)
GLM_BINDING_QUALIFIER(rotateTowards) {
  GLM_BINDING_BEGIN
  if (gLuaQuat<>::fast::Is(LB.L, LB.idx))
    BIND_FUNC(LB, glm::rotateTowards, gLuaQuat<>::fast, gLuaQuat<>, gLuaFloat);
  else
    BIND_FUNC(LB, glm::rotateTowards, gLuaVec3<>, gLuaVec3<>, gLuaFloat, gLuaFloat);
  GLM_BINDING_END
}
#if GLM_HAS_CXX11_STL
NUMBER_VECTOR_DEFN(erf, glm::erf, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(erfc, glm::erfc, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(lgamma, glm::lgamma, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(tgamma, glm::tgamma, LAYOUT_UNARY)
#endif
#endif

#if defined(GTX_GRADIENT_PAINT_HPP)
TRAITS_DEFN(linearGradient, glm::linearGradient, gLuaVec2<>, gLuaVec2<>, gLuaVec2<>)
TRAITS_DEFN(radialGradient, glm::radialGradient, gLuaVec2<>, gLuaFloat, gLuaVec2<>, gLuaVec2<>)
#endif

#if defined(GTX_HANDED_COORDINATE_SPACE_HPP)
TRAITS_DEFN(leftHanded, glm::leftHanded, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
TRAITS_DEFN(rightHanded, glm::rightHanded, gLuaDir3<>, gLuaDir3<>, gLuaDir3<>)
#endif

#if defined(GTX_INTEGER_HPP)
TRAITS_DEFN(factorial, glm::factorial, gLuaInteger)
TRAITS_DEFN(nlz, glm::nlz, gLuaTrait<unsigned>)
#endif

#if defined(GTX_INTERSECT_HPP)
#define LAYOUT_INTERSECT_LINE_SPHERE(LB, F, Tr, ...)                 \
  LUA_MLM_BEGIN                                                      \
  Tr::type v5, v6, v7, v8;                                           \
  const Tr::type v1 = Tr::Next((LB).L, (LB).idx);                    \
  const Tr::safe::type v2 = Tr::safe::Next((LB).L, (LB).idx);        \
  const Tr::safe::type v3 = Tr::safe::Next((LB).L, (LB).idx);        \
  const Tr::value_type v4 = Tr::value_trait::Next((LB).L, (LB).idx); \
  if (glm::intersectLineSphere(v1, v2, v3, v4, v5, v6, v7, v8))      \
    TRAITS_PUSH(LB, true, v5, v6, v7, v8);                           \
  else                                                               \
    TRAITS_PUSH(LB, false);                                          \
  LUA_MLM_END

#define LAYOUT_INTERSECT_RAY_PLANE(LB, F, Tr, ...)                                 \
  LUA_MLM_BEGIN                                                                    \
  Tr::value_type v5;                                                               \
  const Tr::type v1 = Tr::Next((LB).L, (LB).idx);                                  \
  const Tr::safe::type v2 = glm_trait_normalize(Tr::safe::Next((LB).L, (LB).idx)); \
  const Tr::safe::type v3 = Tr::safe::Next((LB).L, (LB).idx);                      \
  const Tr::safe::type v4 = Tr::safe::Next((LB).L, (LB).idx);                      \
  if (glm::intersectRayPlane(v1, v2, v3, v4, v5))                                  \
    TRAITS_PUSH(LB, true, v5);                                                     \
  else                                                                             \
    TRAITS_PUSH(LB, false);                                                        \
  LUA_MLM_END

#define LAYOUT_INTERSECT_RAY_SPHERE(LB, F, Tr, ...)                                \
  LUA_MLM_BEGIN                                                                    \
  Tr::type v5, v6;                                                                 \
  const Tr::type v1 = Tr::Next((LB).L, (LB).idx);                                  \
  const Tr::safe::type v2 = glm_trait_normalize(Tr::safe::Next((LB).L, (LB).idx)); \
  const Tr::safe::type v3 = Tr::safe::Next((LB).L, (LB).idx);                      \
  const Tr::value_type v4 = Tr::value_trait::Next((LB).L, (LB).idx);               \
  if (glm::intersectRaySphere(v1, v2, v3, v4, v5, v6))                             \
    TRAITS_PUSH(LB, true, v5, v6);                                                 \
  else                                                                             \
    TRAITS_PUSH(LB, false);                                                        \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(intersectLineSphere, glm::intersectLineSphere, LAYOUT_INTERSECT_LINE_SPHERE)
NUMBER_VECTOR_DEFN(intersectRayPlane, glm::intersectRayPlane, LAYOUT_INTERSECT_RAY_PLANE)
NUMBER_VECTOR_DEFN(intersectRaySphere, glm::intersectRaySphere, LAYOUT_INTERSECT_RAY_SPHERE)
GLM_BINDING_QUALIFIER(intersectLineTriangle) {
  GLM_BINDING_BEGIN
  gLuaVec3<float>::type v6;
  const gLuaVec3<float>::type v1 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<float>::type v2 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<float>::type v3 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<float>::type v4 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<float>::type v5 = gLuaVec3<>::Next(LB.L, LB.idx);
  if (glm::intersectLineTriangle(v1, v2, v3, v4, v5, v6))
    TRAITS_PUSH(LB, true, v6);
  else
    TRAITS_PUSH(LB, false);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(intersectRayTriangle) {
  GLM_BINDING_BEGIN
  gLuaVec2<>::type baryPosition;
  gLuaVec3<>::value_type distance;
  const gLuaVec3<>::type orig = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<>::type dir = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<>::type v0 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<>::type v1 = gLuaVec3<>::Next(LB.L, LB.idx);
  const gLuaVec3<>::type v2 = gLuaVec3<>::Next(LB.L, LB.idx);
  if (glm::intersectRayTriangle(orig, dir, v0, v1, v2, baryPosition, distance))
    TRAITS_PUSH(LB, true, baryPosition, distance);
  else
    TRAITS_PUSH(LB, false);
  GLM_BINDING_END
}
#endif

#if defined(GTX_MIXED_PRODUCT_HPP)
TRAITS_DEFN(mixedProduct, glm::mixedProduct, gLuaVec3<>, gLuaVec3<>, gLuaVec3<>)
#endif

#if defined(GTX_NORM_HPP)
NUMBER_VECTOR_DEFN(distance2, glm::distance2, LAYOUT_BINARY)
LAYOUT_DEFN(l1Norm, glm::l1Norm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
LAYOUT_DEFN(l2Norm, glm::l2Norm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
NUMBER_VECTOR_DEFN(length2, glm::length2, LAYOUT_UNARY) /* glm/gtx/quaternion.hpp */
#if GLM_VERSION >= 996  // @COMPAT: Added in 0.9.9.6
LAYOUT_DEFN(lMaxNorm, glm::lMaxNorm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>)
#endif
LAYOUT_DEFN(lxNorm, glm::lxNorm, LAYOUT_UNARY_OR_BINARY, gLuaVec3<>, gLuaTrait<unsigned>)
#endif

#if defined(GTX_NORMAL_HPP)
TRAITS_DEFN(triangleNormal, glm::triangleNormal, gLuaVec3<>, gLuaVec3<>, gLuaVec3<>)
#endif

#if defined(GTX_NORMAL_HPP) || defined(GTX_NORMALIZE_DOT_HPP)
NUMBER_VECTOR_DEFN(fastNormalizeDot, glm::fastNormalizeDot, LAYOUT_BINARY)
#endif

#if defined(GTX_NORMALIZE_DOT_HPP)
NUMBER_VECTOR_DEFN(normalizeDot, glm::normalizeDot, LAYOUT_BINARY)
#endif

#if defined(GTX_OPTIMUM_POW_HPP)
INTEGER_NUMBER_VECTOR_DEFN(pow2, glm::pow2, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(pow3, glm::pow3, LAYOUT_UNARY)
INTEGER_NUMBER_VECTOR_DEFN(pow4, glm::pow4, LAYOUT_UNARY)
#endif

#if defined(GTX_ORTHONORMALIZE_HPP)
GLM_BINDING_QUALIFIER(orthonormalize) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  if (ttisvector3(_tv))
    BIND_FUNC(LB, glm::orthonormalize, gLuaVec3<>::fast, gLuaVec3<>);
  else if (ttismatrix(_tv) && mvalue_dims(_tv) == LUAGLM_MATRIX_3x3)
    BIND_FUNC(LB, glm::orthonormalize, gLuaMat3x3<>::fast);
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR3 " or " GLM_STRING_MATRIX "3x3");
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(orthonormalize3) {  // LUA_VECTOR_EXTENSIONS
  GLM_BINDING_BEGIN
  gLuaVec3<>::type x = gLuaVec3<>::Next(LB.L, LB.idx);
  gLuaVec3<>::type y = gLuaVec3<>::Next(LB.L, LB.idx);
  if (gLuaVec3<>::fast::Is(LB.L, LB.idx)) {
    gLuaVec3<>::fast::type z = gLuaVec3<>::fast::Next(LB.L, LB.idx);
    glm::orthonormalize3(x, y, z);
    TRAITS_PUSH(LB, x, y, z);
  }
  else {
    glm::orthonormalize2(x, y);
    TRAITS_PUSH(LB, x, y);
  }
  GLM_BINDING_END
}
#endif

#if defined(GTX_PERPENDICULAR_HPP)
#define LAYOUT_PERPBASIS(LB, F, Tr, ...) \
  LUA_MLM_BEGIN                          \
  Tr::type u, v;                         \
  F(Tr::Next((LB).L, (LB).idx), u, v);   \
  TRAITS_PUSH(LB, u, v);                 \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(perp, glm::perp, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(isPerpendicular, glm::isPerpendicular, LAYOUT_BINARY)  // LUA_VECTOR_EXTENSIONS
LAYOUT_DEFN(perpendicular, glm::perpendicular, LAYOUT_UNARY_OR_TERNARY, gLuaDir3<>)
LAYOUT_DEFN(perpendicular2, glm::perpendicular2, LAYOUT_UNARY_OR_TERNARY, gLuaDir3<>)
LAYOUT_DEFN(perpendicularBasis, glm::perpendicularBasis, LAYOUT_PERPBASIS, gLuaDir3<>)
BINARY_LAYOUT_DEFN(perpendicularFast, glm::perpendicularFast, LAYOUT_UNARY, gLuaVec3<>::fast, gLuaVec2<>::fast)
#endif

#if defined(GTX_POLAR_COORDINATES_HPP)
TRAITS_DEFN(euclidean, glm::euclidean, gLuaVec2<>)
TRAITS_DEFN(polar, glm::polar, gLuaVec3<>)
#endif

#if defined(GTX_PROJECTION_HPP)
#define LAYOUT_DECOMPOSE(LB, F, Tr, ...)               \
  LUA_MLM_BEGIN                                        \
  Tr::type q, r;                                       \
  const Tr::type p = Tr::Next((LB).L, (LB).idx);       \
  const Tr::type d = Tr::safe::Next((LB).L, (LB).idx); \
  F(p, d, q, r);                                       \
  TRAITS_PUSH(LB, q, r);                               \
  LUA_MLM_END

NUMBER_VECTOR_DEFN(proj, glm::proj, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(projNorm, glm::projNorm, LAYOUT_BINARY)  // LUA_VECTOR_EXTENSIONS
NUMBER_VECTOR_DEFN(projPlane, glm::projPlane, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(projDecompose, glm::projDecompose, LAYOUT_DECOMPOSE)
#endif

#if defined(GLM_HAS_RANGE_FOR) || defined(GTX_RANGE_HPP)
// GLM_BINDING_DECL(begin);
// GLM_BINDING_DECL(end);
GLM_BINDING_QUALIFIER(components) {  // An optimized variant of glm::components
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttype(_tv)) {
    case LUA_TVECTOR: return gLuaBase::Push(LB, glm_dimensions(ttypetag(_tv)));
    case LUA_TMATRIX: {
      gLuaBase::Push(LB, LUAGLM_MATRIX_COLS(mvalue_dims(_tv)));
      gLuaBase::Push(LB, LUAGLM_MATRIX_ROWS(mvalue_dims(_tv)));
      return 2;
    }
    default: {
      return gLuaBase::Push(LB, 1);
    }
  }
  GLM_BINDING_END
}
#endif

#if defined(GTX_ROTATE_VECTOR_HPP)
TRAITS_DEFN(orientation, glm::orientation, gLuaDir3<>, gLuaDir3<>)
BINARY_LAYOUT_DEFN(rotateX, glm::rotateX, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
BINARY_LAYOUT_DEFN(rotateY, glm::rotateY, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
BINARY_LAYOUT_DEFN(rotateZ, glm::rotateZ, LAYOUT_BINARY_SCALAR, gLuaVec3<>::fast, gLuaVec4<>::fast)
#endif

#if defined(GTX_ROTATE_VECTOR_HPP) || defined(EXT_QUATERNION_COMMON_HPP)
#if GLM_VERSION >= 998  // @COMPAT: slerp + 'additional spin count' introduced in 0.9.9.8
  #define LAYOUT_QUAT_SLERP(LB, F, Tr, ...)                                                    \
    LUA_MLM_BEGIN                                                                              \
    if (gLuaTrait<int>::Is((LB).L, (LB).idx + 3))                                              \
      VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, gLuaTrait<int>, ##__VA_ARGS__); \
    else                                                                                       \
      VA_CALL(BIND_FUNC, LB, F, Tr, Tr::safe, Tr::value_trait, ##__VA_ARGS__);                 \
    LUA_MLM_END
#else
  #define LAYOUT_QUAT_SLERP(LB, F, Tr, ...) LAYOUT_TERNARY_SCALAR(LB, F, Tr, ##__VA_ARGS__)
#endif

NUMBER_VECTOR_QUAT_DEFNS(slerp, glm::__slerp, LAYOUT_TERNARY_SCALAR, LAYOUT_TERNARY_SCALAR, LAYOUT_QUAT_SLERP)
NUMBER_VECTOR_QUAT_DEFN(barycentric, glm::barycentric, LAYOUT_BARYCENTRIC)  // LUA_VECTOR_EXTENSIONS
#endif

#if defined(GTX_ROTATE_VECTOR_HPP) || defined(EXT_MATRIX_TRANSFORM_HPP) || defined(GTX_MATRIX_TRANSFORM_2D_HPP) || defined(GTX_QUATERNION_TRANSFORM_HPP)
GLM_BINDING_QUALIFIER(rotate) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VFALSE: case LUA_VTRUE:  // @BoolCoercion
    case LUA_VSHRSTR: case LUA_VLNGSTR:  // @StringCoercion
    case LUA_VNUMINT:  // @IntCoercion
    case LUA_VNUMFLT: BIND_FUNC(LB, glm::rotate, gLuaFloat, gLuaVec3<>); break; /* glm/gtx/transform.hpp */
    case LUA_VVECTOR2: BIND_FUNC(LB, glm::rotate, gLuaVec2<>::fast, gLuaVec2<>::value_trait); break;
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::rotate, gLuaVec3<>::fast, gLuaVec3<>::value_trait, gLuaDir3<>); break;
    case LUA_VVECTOR4: BIND_FUNC(LB, glm::rotate, gLuaVec4<>::fast, gLuaVec4<>::value_trait, gLuaDir3<>); break;
    case LUA_VQUAT: { /* glm/ext/quaternion_transform.hpp */
      const TValue *_tv2 = glm_i2v(LB.L, LB.idx + 1);
      if (ttisnumber(_tv2))
        BIND_FUNC(LB, glm::rotate, gLuaQuat<>::fast, gLuaFloat::fast, gLuaDir3<>); /* <quat, angle, axis> */
      else if (ttisvector3(_tv2)) /* glm/gtx/quaternion.hpp */
        BIND_FUNC(LB, glm::rotate, gLuaQuat<>::fast, gLuaVec3<>::fast);
#if defined(LUAGLM_ALIGNED)
      else if (ttisvector4(_tv2))
        BIND_FUNC(LB, glm::__rotate, gLuaQuat<>::fast, gLuaVec4<>::fast);
#else
      else if (ttisvector4(_tv2)) /* glm/gtx/quaternion.hpp */
        BIND_FUNC(LB, glm::rotate, gLuaQuat<>::fast, gLuaVec4<>::fast);
#endif
      return luaL_error(LB.L, "quat-rotate expects: {quat, angle:radians, axis:vec3}, {quat, dir:vec3}, {quat, point:vec4}");
    }
    case LUA_VMATRIX: {
      switch (mvalue_dims(_tv)) {
        case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, glm::rotate, gLuaMat3x3<>::fast, gLuaMat3x3<>::value_trait); break;
        case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, glm::rotate, gLuaMat4x4<>::fast, gLuaMat4x4<>::value_trait, gLuaDir3<>); break;
        default: {
          break;
        }
      }
      return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_MATRIX "3x3 or " GLM_STRING_MATRIX "4x4");
    }
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR " or " GLM_STRING_QUATERN " or " GLM_STRING_MATRIX);
  GLM_BINDING_END
}

// TRAITS_DEFN(rotate_slow, glm::rotate_slow, gLuaMat4x4<>, gLuaMat4x4<>::value_trait, gLuaDir3<>)
TRAITS_DEFN(rotateFromTo, glm::rotateFromTo, gLuaVec3<>, gLuaVec3<>)  // LUA_QUATERNION_EXTENSIONS
TRAITS_DEFN(shortest_equivalent, glm::shortest_equivalent, gLuaQuat<>)
ROTATION_MATRIX_DEFN(transformDir, glm::transformDir, LAYOUT_UNARY, gLuaVec3<>)  // LUA_MATRIX_EXTENSIONS
ROTATION_MATRIX_DEFN(transformPos, glm::transformPos, LAYOUT_UNARY, gLuaVec3<>)
TRAITS_DEFN(transformPosPerspective, glm::transformPosPerspective, gLuaMat4x4<>, gLuaVec3<>)
#if defined(LUAGLM_INLINED_TEMPLATES)
TRAITS_DEFN(rotate_mat3, glm::rotate, gLuaMat3x3<>, gLuaMat3x3<>::value_trait)
TRAITS_DEFN(rotate_mat4, glm::rotate, gLuaMat4x4<>, gLuaMat4x4<>::value_trait, gLuaDir3<>)
#endif
#endif

#if defined(GTX_SPLINE_HPP)
NUMBER_VECTOR_DEFN(catmullRom, glm::catmullRom, LAYOUT_QUINARY_SCALAR)
NUMBER_VECTOR_DEFN(cubic, glm::cubic, LAYOUT_QUINARY_SCALAR)
NUMBER_VECTOR_DEFN(hermite, glm::hermite, LAYOUT_QUINARY_SCALAR)
#endif

#if defined(GTX_TEXTURE_HPP)
INTEGER_NUMBER_VECTOR_DEFN(levels, glm::levels, LAYOUT_UNARY)
#endif

#if defined(GTX_TRANSFORM_HPP) || defined(EXT_MATRIX_TRANSFORM_HPP)
GLM_BINDING_QUALIFIER(scale) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::scale, gLuaVec3<>::fast); break;
    case LUA_VMATRIX: {
      switch (mvalue_dims(_tv)) {
        case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, glm::scale, gLuaMat3x3<>::fast, gLuaVec2<>); break;
        case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, glm::scale, gLuaMat4x4<>::fast, gLuaVec3<>); break;
        default: {
          break;
        }
      }
      return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_MATRIX "3x3 or " GLM_STRING_MATRIX "4x4");
    }
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR3);
  GLM_BINDING_END
}

// TRAITS_DEFN(scale_slow, glm::scale_slow, gLuaMat4x4<>, gLuaVec3<>)
GLM_BINDING_QUALIFIER(translate) {
  GLM_BINDING_BEGIN
  const TValue *_tv = glm_i2v(LB.L, LB.idx);
  switch (ttypetag(_tv)) {
    case LUA_VVECTOR3: BIND_FUNC(LB, glm::translate, gLuaVec3<>::fast); break;
    case LUA_VMATRIX: {
      switch (mvalue_dims(_tv)) {
        case LUAGLM_MATRIX_3x3: BIND_FUNC(LB, glm::translate, gLuaMat3x3<>::fast, gLuaVec2<>); break;
        case LUAGLM_MATRIX_4x4: BIND_FUNC(LB, glm::translate, gLuaMat4x4<>::fast, gLuaVec3<>); break;
        default: {
          break;
        }
      }
      return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_MATRIX "3x3 or " GLM_STRING_MATRIX "4x4");
    }
    default: {
      break;
    }
  }
  return GLM_TYPE_ERROR(LB.L, LB.idx, GLM_STRING_VECTOR3 " or " GLM_STRING_SYMMATRIX);
  GLM_BINDING_END
}

TRAITS_DEFN(trs, glm::trs, gLuaVec3<>, gLuaQuat<>, gLuaVec3<>)  // LUA_MATRIX_EXTENSIONS
#if defined(LUAGLM_INLINED_TEMPLATES)
TRAITS_DEFN(translate_vec3, glm::translate, gLuaVec3<>)
TRAITS_DEFN(translate_mat3, glm::translate, gLuaMat3x3<>, gLuaVec2<>)
TRAITS_DEFN(translate_mat4, glm::translate, gLuaMat4x4<>, gLuaVec3<>)
TRAITS_DEFN(scale_vec3, glm::scale, gLuaVec3<>)
TRAITS_DEFN(scale_mat3, glm::scale, gLuaMat3x3<>, gLuaVec2<>)
TRAITS_DEFN(scale_mat4, glm::scale, gLuaMat4x4<>, gLuaVec3<>)
#endif
#endif

#if defined(GTX_VECTOR_ANGLE_HPP) || defined(EXT_QUATERNION_TRIGONOMETRIC_HPP)
#define ORIENTED_ANGLE_DEFN(Name, F, ...)                                                                           \
  GLM_BINDING_QUALIFIER(Name) {                                                                                     \
    GLM_BINDING_BEGIN                                                                                               \
    const TValue *_tv = glm_i2v((LB).L, (LB).idx);                                                                  \
    switch (ttypetag(_tv)) {                                                                                        \
      case LUA_VVECTOR2: VA_CALL(BIND_FUNC, LB, F, gLuaDir2<>::fast, gLuaDir2<>, ##__VA_ARGS__); break;             \
      case LUA_VVECTOR3: VA_CALL(BIND_FUNC, LB, F, gLuaDir3<>::fast, gLuaDir3<>, gLuaDir3<>, ##__VA_ARGS__); break; \
      case LUA_VQUAT: VA_CALL(BIND_FUNC, LB, F, gLuaQuat<>::fast, gLuaQuat<>, gLuaDir3<>, ##__VA_ARGS__); break;    \
      default:                                                                                                      \
        break;                                                                                                      \
    }                                                                                                               \
    return GLM_TYPE_ERROR((LB).L, (LB).idx, GLM_STRING_VECTOR);                                                     \
    GLM_BINDING_END                                                                                                 \
  }

NUMBER_VECTOR_QUAT_DEFNS(angle, glm::angle, LAYOUT_BINARY, LAYOUT_BINARY, LAYOUT_UNARY_OR_BINARY)
ORIENTED_ANGLE_DEFN(orientedAngle, glm::orientedAngle)
NUMBER_VECTOR_QUAT_DEFNS(angle_atan, glm::__angle, LAYOUT_BINARY, LAYOUT_BINARY, LAYOUT_UNARY_OR_BINARY)  // LUA_VECTOR_EXTENSIONS
ORIENTED_ANGLE_DEFN(orientedAngle_atan, glm::__orientedAngle)
TRAITS_DEFN(fromAngle, glm::fromAngle, gLuaCNum)  // LUA_VECTOR_EXTENSIONS
#endif

#if defined(GTX_VECTOR_QUERY_HPP)
NUMBER_VECTOR_DEFN(areCollinear, glm::areCollinear, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(areOrthogonal, glm::areOrthogonal, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(areOrthonormal, glm::areOrthonormal, LAYOUT_TERNARY_EPS)
NUMBER_VECTOR_DEFN(isCompNull, glm::isCompNull, LAYOUT_BINARY_EPS)
#endif

#if defined(GTX_VECTOR_QUERY_HPP) || defined(GTX_MATRIX_QUERY_HPP)
GLM_BINDING_QUALIFIER(isNormalized) {
  GLM_BINDING_BEGIN  // Error message technically incorrect here.
  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttismatrix(o))
    PARSE_MATRIX(LB, mvalue_dims(o), glm::_isNormalized, LAYOUT_BINARY_EPS);
  else
    PARSE_NUMBER_VECTOR_QUAT(LB, glm::isNormalized, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS);
  GLM_BINDING_END
}

GLM_BINDING_QUALIFIER(isNull) {
  GLM_BINDING_BEGIN  // Error message technically incorrect here.
  const TValue *o = glm_i2v(LB.L, LB.idx);
  if (ttismatrix(o))
    PARSE_MATRIX(LB, mvalue_dims(o), glm::_isNull, LAYOUT_BINARY_EPS);
  else
    PARSE_NUMBER_VECTOR_QUAT(LB, glm::isNull, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS, LAYOUT_BINARY_EPS);
  GLM_BINDING_END
}

NUMBER_VECTOR_DEFN(isUniform, glm::isUniform, LAYOUT_UNARY)  // LUA_VECTOR_EXTENSIONS
#endif

#if defined(GTX_WRAP_HPP) || defined(EXT_SCALAR_COMMON_HPP)
NUMBER_VECTOR_DEFN(mirrorClamp, glm::mirrorClamp, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(mirrorRepeat, glm::mirrorRepeat, LAYOUT_UNARY)
NUMBER_VECTOR_DEFN(repeat, glm::repeat, LAYOUT_UNARY)
TRAITS_DEFN(deltaAngle, glm::deltaAngle, gLuaNumber, gLuaNumber)  // LUA_VECTOR_EXTENSIONS
NUMBER_VECTOR_DEFN(loopRepeat, glm::loopRepeat, LAYOUT_BINARY_OPTIONAL)
NUMBER_VECTOR_DEFN(pingPong, glm::pingPong, LAYOUT_BINARY)
NUMBER_VECTOR_DEFN(lerpAngle, glm::lerpAngle, LAYOUT_TERNARY_OPTIONAL)
#endif

/* }================================================================== */

/*
** {==================================================================
** <random>
** ===================================================================
*/
// @TODO: sanitize arguments, e.g., uniform_real_distribution requires:
//  min <= max && (0 <= min || max <= min + (numeric_limits<T>::max)()
#if defined(STD_RANDOM) && GLM_HAS_CXX11_STL
#include <random>

#define RAND_TRAIT(LB, F, A, ...) RANDOM_DEVICE(LB, F, A) /* Avoid /Zc:preprocessor- headache */
#define RANDOM_DEVICE(...) VA_CALL(RANDOM_DEVICE, __VA_ARGS__) /* Mapping to <random> structures */
#define RANDOM_DEVICE1(LB) return gLuaBase::Push(LB, 0) /* zero */
#define RANDOM_DEVICE2(LB, F) return gLuaBase::Push(LB, F()(LB)) /* std::rand_func<>()(LB) */

/* std::rand_func<>(a)(LB) */
#define RANDOM_DEVICE3(LB, F, A)                                 \
  LUA_MLM_BEGIN                                                  \
  if (A::Is((LB).L, (LB).idx))                                   \
    return gLuaBase::Push(LB, F(A::Next((LB).L, (LB).idx))(LB)); \
  return gLuaBase::Push(LB, F()(LB));                            \
  LUA_MLM_END

/* std::rand_func<>(a, b)(LB) */
#define RANDOM_DEVICE4(LB, F, A, B)                                  \
  LUA_MLM_BEGIN                                                      \
  if (A::Is((LB).L, (LB).idx) && B::Is((LB).L, (LB).idx + 1)) {      \
    const A::type _a = A::Next((LB).L, (LB).idx);                    \
    return gLuaBase::Push(LB, F(_a, B::Next((LB).L, (LB).idx))(LB)); \
  }                                                                  \
  return gLuaBase::Push(LB, F()(LB));                                \
  LUA_MLM_END

LAYOUT_DEFN(uniform_int, std::uniform_int_distribution<lua_Integer>, RANDOM_DEVICE, gLuaInteger, gLuaInteger)
LAYOUT_DEFN(uniform_real, std::uniform_real_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(bernoulli, std::bernoulli_distribution, RAND_TRAIT, gLuaTrait<double>)
LAYOUT_DEFN(binomial, std::binomial_distribution<lua_Integer>, RANDOM_DEVICE, gLuaInteger, gLuaTrait<double>)
LAYOUT_DEFN(negative_binomial, std::negative_binomial_distribution<lua_Integer>, RANDOM_DEVICE, gLuaInteger, gLuaTrait<double>)
LAYOUT_DEFN(geometric, std::geometric_distribution<lua_Integer>, RAND_TRAIT, gLuaTrait<double>)
LAYOUT_DEFN(poisson, std::poisson_distribution<lua_Integer>, RAND_TRAIT, gLuaTrait<double>)
LAYOUT_DEFN(exponential, std::exponential_distribution<glm_Number>, RAND_TRAIT, gLuaNumber)
LAYOUT_DEFN(gamma, std::gamma_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(weibull, std::weibull_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(extreme_value, std::extreme_value_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(normal, std::normal_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(lognormal, std::lognormal_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(chi_squared, std::chi_squared_distribution<glm_Number>, RAND_TRAIT, gLuaNumber)
LAYOUT_DEFN(cauchy, std::cauchy_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(fisher_f, std::fisher_f_distribution<glm_Number>, RANDOM_DEVICE, gLuaNumber, gLuaNumber)
LAYOUT_DEFN(student_t, std::student_t_distribution<glm_Number>, RAND_TRAIT, gLuaNumber)
// discrete
// piecewise_constant_distribution
// piecewise_linear_distribution
#endif
/* }================================================================== */

#endif
