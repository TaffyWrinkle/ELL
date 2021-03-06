////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorStatistics.h (debugCompiler)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Tensor.h>

#include <cmath>
#include <cstring> // for size_t
#include <vector>

namespace ell
{
/// <summary> This class provides some handy statistics over vectors. </summary>
class VectorStatistics
{
public:
    VectorStatistics() = default;

    /// <summary> Create VectorStatistics from the given vector. </summary>
    template <typename ValueType>
    VectorStatistics(const std::vector<ValueType>& vec);

    /// <summary> Create VectorStatistics from the given tensor. </summary>
    template <typename TensorType>
    VectorStatistics(const TensorType& tensor);

    /// <summary> Return the number of elements in the vector. </summary>
    size_t NumElements() const { return _size; }

    /// <summary> Return the minimum numeric value found in the vector. </summary>
    double Min() const { return _min; }

    /// <summary> Return the maximum numeric value found in the vector. </summary>
    double Max() const { return _max; }

    /// <summary> Return the mean of the values in the vector. </summary>
    double Mean() const { return _mean; }

    /// <summary> Return the variance of the values in the vector. </summary>
    double Variance() const { return _variance; }

    /// <summary> Return the standard deviation of the values in the vector. </summary>
    double StdDev() const { return _stdDev; }

    /// <summary> Returns true if the vector is valid (has values). </summary>
    bool IsValid() const { return _valid; }

    /// <summary> Returns the sum of the difference between all the values in the given vectors. </summary>
    template <typename ValueType>
    static double Diff(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

    /// <summary> Returns the sum of the difference between all the values in the given Tensors. </summary>
    template <typename TensorType>
    static double Diff(const TensorType& tensor1, const TensorType& tensor2);

private:
    template <typename ValueType>
    void Initialize(const std::vector<ValueType>& vec);

    bool _valid = false;
    size_t _size = 0;
    double _min = 0;
    double _max = 0;
    double _mean = 0;
    double _variance = 0;
    double _stdDev = 0;
};

/// <summary> Returns vec1 with element-wise subtraction of values in vec2.</summary>
template <typename ValueType>
std::vector<ValueType> Subtract(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

/// <summary> Returns tensor1 with element-wise subtraction of values in tensor2.</summary>
template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Subtract(const TensorType& tensor1, const TensorType& tensor2);

/// <summary> Returns vec with each element converted to the absolute value of the original.</summary>
template <typename ValueType>
std::vector<ValueType> Abs(const std::vector<ValueType>& vec);

/// <summary> Returns tensor with each element converted to the absolute value of the original.</summary>
template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Abs(const TensorType& tensor);
} // namespace ell

#pragma region implementation

#include <algorithm>
#include <cassert>

namespace ell
{
//
// VectorStatistics class
//
template <typename ValueType>
VectorStatistics::VectorStatistics(const std::vector<ValueType>& vec)
{
    Initialize(vec);
}

template <typename TensorType>
VectorStatistics::VectorStatistics(const TensorType& tensor)
{
    Initialize(tensor.ToArray());
}

template <typename ValueType>
void VectorStatistics::Initialize(const std::vector<ValueType>& vec)
{
    if (vec.size() == 0)
    {
        _valid = false;
        return;
    }

    // compute basic stats
    _min = static_cast<double>(vec[0]);
    _max = static_cast<double>(vec[0]);
    double sum = 0;
    for (auto x : vec)
    {
        double d = static_cast<double>(x);
        _min = std::min(_min, d);
        _max = std::max(_max, d);
        sum += d;
    }
    _mean = sum / vec.size();

    double sumDiffMeanSq = 0;
    for (auto x : vec)
    {
        auto diff = x - _mean;
        sumDiffMeanSq += diff * diff;
    }
    _variance = sumDiffMeanSq / vec.size();
    _stdDev = std::sqrt(_variance);
    _size = vec.size();
    _valid = true;
}

template <typename ValueType>
double VectorStatistics::Diff(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2)
{
    ValueType error = 0;
    for (size_t i = 0, len1 = vec1.size(), len2 = vec2.size(); i < len1 || i < len2; i++)
    {
        if (i < len1 && i < len2)
        {
            ValueType e = vec1[i];
            ValueType f = vec2[i];
            if (e != f)
            {
                error += fabs(e - f);
            }
        }
        else if (i < len1)
        {
            ValueType e = vec1[i];
            error += fabs(e);
        }
        else if (i < len2)
        {
            ValueType f = vec2[i];
            error += fabs(f);
        }
    }
    return error;
}

template <typename TensorType>
double VectorStatistics::Diff(const TensorType& tensor1, const TensorType& tensor2)
{
    return Diff(tensor1.ToArray(), tensor2.ToArray());
}

template <typename ValueType>
std::vector<ValueType> Subtract(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2)
{
    auto size1 = vec1.size();
    auto size2 = vec2.size();
    assert(size1 == size2); // require this for now
    auto maxSize = std::max(size1, size2);
    std::vector<ValueType> result(maxSize);
    for (size_t index = 0; index < maxSize; ++index)
    {
        auto val1 = index < size1 ? vec1[index] : 0;
        auto val2 = index < size2 ? vec2[index] : 0;
        result[index] = val1 - val2;
    }

    return result;
}

template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Subtract(const TensorType& tensor1, const TensorType& tensor2)
{
    ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> result(tensor1);

    for (size_t i = 0; i < result.NumRows(); ++i)
    {
        for (size_t j = 0; j < result.NumColumns(); ++j)
        {
            for (size_t k = 0; k < result.NumChannels(); ++k)
            {
                result(i, j, k) = tensor1(i, j, k) - tensor2(i, j, k);
            }
        }
    }

    return result;
}

template <typename ValueType>
std::vector<ValueType> Abs(const std::vector<ValueType>& vec)
{
    std::vector<ValueType> result(vec.size());
    for (size_t index = 0; index < vec.size(); ++index)
    {
        result[index] = std::fabs(vec[index]);
    }

    return result;
}

template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Abs(const TensorType& tensor)
{
    ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> result(tensor);
    result.Transform([](auto x) { return std::abs(x); });
    return result;
}
} // namespace ell

#pragma endregion implementation
