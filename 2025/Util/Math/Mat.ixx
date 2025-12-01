export module Util:Math.Mat;

import :Math.Vec;
import :Types;


export template<typename T, ssz R, ssz C>
class Mat
{
public:
  static constexpr ssz RowCount = R;
  static constexpr ssz ColumnCount = C;

  T data[C][R] = {};

  Mat<T, R - 1, R - 1> Submatrix(ssz c, ssz r) const requires (R == C)
  {
    Mat<T, R-1, R-1> subMat;

    ssz dc = 0;
    for (ssz sc = 0; sc < C; sc++)
    {
      if (sc == c)
        { continue; }

      ssz dr = 0;
      for (ssz sr = 0; sr < R; sr++)
      {
        if (sr == r)
          { continue; }

        subMat.data[dc][dr] = data[sc][sr];
        dr++;
      }

      dc++;
    }

    return subMat;
  }


  [[nodiscard]] Mat<T, C, R> Transpose() const
  {
    Mat<T, C, R> out;
    for (ssz c = 0; c < C; c++)
    {
      for (ssz r = 0; r < R; r++)
        { out.data[r][c] = data[c][r]; }
    }

    return out;
  }

  template <ssz N>
  Mat<T, R, N> operator * (const Mat<T, C, N> &b) const
  {
    Mat<T, R, N> out {};
    for (ssz r = 0; r < R; r++)
    {
      for (ssz n = 0; n < N; n++)
      {
        for (ssz c = 0; c < C; c++)
          { out.data[n][r] += data[c][r] * b.data[n][c]; }
      }
    }

    return out;
  }


  T Determinant() const requires (R == C)
  {
    if constexpr (R == 1)
      { return data[0][0]; }
    else if constexpr (R == 2)
      { return data[0][0] * data[1][1] - data[0][1] * data[1][0]; }
    else if constexpr (R == 3)
    {
      return (data[0][0] * (data[1][1] * data[2][2] - data[2][1] * data[1][2]) -
              data[1][0] * (data[0][1] * data[2][2] - data[2][1] * data[0][2]) +
              data[2][0] * (data[0][1] * data[1][2] - data[1][1] * data[0][2]));
    }
    else if constexpr (R == 4)
    {
      T temp1 = (data[2][2] * data[3][3]) - (data[3][2] * data[2][3]);
      T temp2 = (data[1][2] * data[3][3]) - (data[3][2] * data[1][3]);
      T temp3 = (data[1][2] * data[2][3]) - (data[2][2] * data[1][3]);
      T temp4 = (data[0][2] * data[3][3]) - (data[3][2] * data[0][3]);
      T temp5 = (data[0][2] * data[2][3]) - (data[2][2] * data[0][3]);
      T temp6 = (data[0][2] * data[1][3]) - (data[1][2] * data[0][3]);

      return ((((data[0][0] * (((data[1][1] * temp1) - (data[2][1] * temp2)) + (data[3][1] * temp3))) -
                (data[1][0] * (((data[0][1] * temp1) - (data[2][1] * temp4)) + (data[3][1] * temp5)))) +
                (data[2][0] * (((data[0][1] * temp2) - (data[1][1] * temp4)) + (data[3][1] * temp6)))) -
                (data[3][0] * (((data[0][1] * temp3) - (data[1][1] * temp5)) + (data[2][1] * temp6))));
    }
    else
    {
      double det = 0;
      for (ssz c = 0; c < C; c++)
        { det += data[c][0] * ((c & 1) ? -1 : 1) * Submatrix(c, 0).Determinant(); }

      return det;
    }
  }

  [[nodiscard]] Mat Inverse() const requires (R == C)
  {
    T det = Determinant();
    Mat out;

    // TODO: add determinant div 0 assert

    T invDet = T(1) / det;
    if constexpr(R == 2)
    {
      out.data[0][0] = data[1][1] *  invDet;
      out.data[0][1] = data[1][0] * -invDet;
      out.data[1][0] = data[0][1] * -invDet;
      out.data[1][1] = data[0][0] *  invDet;
    }
    else if constexpr(R == 3)
    {
      out.data[0][0] = (data[1][1] * data[2][2] - data[2][1] * data[1][2]) * invDet;
      out.data[0][1] = (data[2][1] * data[0][2] - data[0][1] * data[2][2]) * invDet;
      out.data[0][2] = (data[0][1] * data[1][2] - data[1][1] * data[0][2]) * invDet;

      out.data[1][0] = (data[2][0] * data[1][2] - data[1][0] * data[2][2]) * invDet;
      out.data[1][1] = (data[0][0] * data[2][2] - data[2][0] * data[0][2]) * invDet;
      out.data[1][2] = (data[1][0] * data[0][2] - data[0][0] * data[1][2]) * invDet;

      out.data[2][0] = (data[1][0] * data[2][1] - data[2][0] * data[1][1]) * invDet;
      out.data[2][1] = (data[2][0] * data[0][1] - data[0][0] * data[2][1]) * invDet;
      out.data[2][2] = (data[0][0] * data[1][1] - data[1][0] * data[0][1]) * invDet;
    }
    else
    {
      for (ssz c = 0; c < C; c++)
      {
        for (ssz r = 0; r < C; r++)
        { out.data[r][c] = invDet * Submatrix(c, r).Determinant() * (((c + r) & 1) ? -1 : 1); }
      }
    }

    return out;
  }

  VecN<T, C> operator * (VecN<T, R> v) const
  {
    VecN<T, C> out;
    for (ssz c = 0; c < C; c++)
    {
      out[c] = T(0);

      for (ssz r = 0; r < R; r++)
        { out[c] += data[c][r] * v[r]; }
    }

    return out;
  }
};