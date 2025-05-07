/*****************************************************************************
  Small program using intel SSE operations to do matrix math on a vector type
  and a matrix type. Some parts of the snippets are redacted to prevent future students
  from cheating. Lots of syntax is incorrect on purpose to convey the structure,
  rather than specifics.

  Author(s): Evan O'Bryant
  Copyright © 2023 DigiPen (USA) Corporation.    
*****************************************************************************/

#include <climits>
#include <intrin.h>

struct alignas(16) Vector4 {
    operator[](...);

    union {
        int a[4];
        struct { int x, y, z, w; };
    };
};

struct alignas(16) Matrix4 {
    operator[](...);
    
    union {
        int a[16];
        Vector4 v[4];
        int m[4][4];
    };
};

/**
 * Returns dot product of two vectors.
*/
int DotProduct(const Vector4& v1, const Vector4& v2)
{
        //convert vectors
    __m128i vecRes = _mm_load_si128((__m128i*)v1.a);
    __m128i b = _mm_load_si128((__m128i*)v2.a);

        //mult packed 32-bit signed ints (xyzw)
    vecRes = SSEMult128s(vecRes, b);
        //add x's/y's and z's/w's (yxwz)
    vecRes = SSEAdd128s(vecRes,
                           swizzle vecRes to yxwz: SSEShuffle(2, 3, 0, 1));
        //add combos together (wzyx)
    vecRes = SSEAdd128s(vecRes,
                            swizzle vecRes to wzyx: SSEShuffle(1, 0, 3, 2));

        //convert to int and return
    return SSEConvert128ToInt(vecRes);
}

/**
 * Returns result of a matrix multiplied by a vector.
*/
Vector4 Multiply(const Matrix4& a, const Vector4& x)
{
        //column order version of matrix
    Matrix4 colOrdr = { a.m[0][0], a.m[1][0], a.m[2][0], a.m[3][0],
                        a.m[0][1], a.m[1][1], a.m[2][1], a.m[3][1],
                        a.m[0][2], a.m[1][2], a.m[2][2], a.m[3][2],
                        a.m[0][3], a.m[1][3], a.m[2][3], a.m[3][3] };

        //convert vector
    __m128i xyzw = _mm_load_si128((__m128i*)x.a);
        //convert transposed matrix
    __m128i mat[4] = { _mm_load_si128((__m128i*)colOrdr.m[0]),
                       ... };

        //multiply transpose by respective values (c1 * x, c2 * y, ...)
        //can't be in loop due to shuffle 2nd param requiring constant
    mat[0] = SSEMult128s(mat[0], swizzle xyzw to xxxx);
    ... // for all 4

        //sum all into first vector column
    for (int i = 1; i < 4; ++i) {
        mat[0] = SSEAdd128s(mat[0], mat[i]);
    }

        //reinterpret first column as result and return
    Vector4 res{ 0 };
    _mm_store_si128((__m128i*)&res, mat[0]);

    return res;
}

/**
 * Returns result of a matrix multiplied by a matrix.
*/
Matrix4 Multiply(const Matrix4& a, const Matrix4& b)
{
    Matrix4 res{ 0 }; //initialize new matrix

    for (int i = 0; i < 4; ++i) {
            //create column to multiply by for each row
        Vector4 col = { ... };
        
            //call sse dotproduct function and store result
        for (int j = 0; j < 4; ++j) {
            res.m[j][i] = DotProduct(a.v[j], col);
        }
    }

    return res;
}
