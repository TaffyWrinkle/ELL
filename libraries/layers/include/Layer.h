////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Layer.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinate.h"

// types
#include "types.h"
#include "RealArray.h"

// utilities
#include "JsonSerializer.h"
#include "StlIteratorAdapter.h"

// stl
#include <vector>
#include <memory>
#include <string>

namespace layers
{
    /// <summary> Implements a layer. </summary>
    class Layer
    {
    public:
        enum class Type { zero, scale, shift, sum, decisionTreePath };

        /// <summary> Default ctor. </summary>
        ///
        /// <param name="type"> The type. </param>
        Layer(Type type);

        /// <summary> Default copy ctor. </summary>
        ///
        /// <param name="other"> The other. </param>
        Layer(const Layer& other) = default;

        /// <summary> Default move ctor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        Layer(Layer&&) = default;

        /// <summary> Default virtual destructor. </summary>
        virtual ~Layer() = default;

        /// <summary> \returns The size of the layer's output. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const = 0;

        /// <summary> \returns A std::string tha trepresents the layer type. </summary>
        ///
        /// <returns> The type name. </returns>
        std::string GetTypeName() const;

        /// <summary> Computes the output of the layer. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="outputs"> [in,out] The outputs. </param>
        virtual void Compute(uint64 layerIndex, std::vector<types::DoubleArray>& outputs) const = 0;

        /// <summary> \Returns An Iterator to the inputs that the specified output depends on. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The input coordinates. </returns>
        using Iterator = utilities::StlIteratorAdapter<std::vector<Coordinate>::const_iterator, Coordinate>;
        virtual Iterator GetInputCoordinates(uint64 index) const = 0;

        /// <summary> Serializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Serialize(utilities::JsonSerializer& serializer) const = 0;

        /// <summary> Deserializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="version"> The version. </param>
        virtual void Deserialize(utilities::JsonSerializer& serializer, int version) = 0;

    protected:
        void SerializeHeader(utilities::JsonSerializer& serializer, int version) const;

        Type _type;
        static const std::string typeName[];
    };
}


