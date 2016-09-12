////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cassert>
#include <cmath>

// dataset
#include "RowDataset.h"

namespace emll
{
namespace trainers
{
    template <typename LossFunctionType>
    SGDIncrementalTrainer<LossFunctionType>::SGDIncrementalTrainer(uint64_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _total_iterations(1), _lastPredictor(dim), _averagedPredictor(std::make_shared<PredictorType>(dim)) // iterations start from 1 to prevent divide-by-zero
    {
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        UpdateSparse(std::move(exampleIterator));
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::IIncrementalTrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(uint64_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<SGDIncrementalTrainer<LossFunctionType>>(dim, lossFunction, parameters);
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::UpdateSparse(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // get references to the vector and biases
        auto& vLast = _lastPredictor.GetWeights();
        auto& vAvg = _averagedPredictor->GetWeights();

        double& bLast = _lastPredictor.GetBias();
        double& bAvg = _averagedPredictor->GetBias();

        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + exampleIterator.NumIteratesLeft();
        const double eta = 1.0 / _parameters.regularization / T_prev;
        const double sigma = std::log(T_next) + 0.5 / T_next;

        // calulate the contribution of the old lastPredictor to the new avergedPredictor
        const double historyWeight = sigma - std::log(T_prev) - 0.5 / T_prev;
        vLast.AddTo(vAvg, historyWeight);
        bAvg += bLast * historyWeight;
        while (exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;
            const auto& dataVector = example.GetDataVector();

            // calculate the prediction
            double alpha = T_prev / (t - 1) * _lastPredictor.Predict(dataVector);

            // calculate the loss derivative
            double beta = weight * _lossFunction.GetDerivative(alpha, label);

            // Update vLast and vAvg
            double lastCoeff = -eta * beta;
            dataVector.AddTo(vLast, lastCoeff);
            bLast += lastCoeff;

            double avgCoeff = lastCoeff * (sigma - std::log(t) - 0.5 / t);
            dataVector.AddTo(vAvg, avgCoeff);
            bAvg += avgCoeff;

            exampleIterator.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        double scale = T_prev / T_next;
        vLast.Scale(scale);
        bLast *= scale;
        vAvg.Scale(scale);
        bAvg *= scale;
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::UpdateDense(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // get references to the vector and biases
        auto& vLast = _lastPredictor.GetWeights();
        auto& vAvg = _averagedPredictor->GetWeights();

        double& bLast = _lastPredictor.GetBias();
        double& bAvg = _averagedPredictor->GetBias();

        while (exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;
            const auto& dataVector = example.GetDataVector();

            // predict
            double alpha = _lastPredictor.Predict(dataVector);

            // calculate the loss derivative
            double beta = weight * _lossFunction.GetDerivative(alpha, label);

            // update last
            double scaleCoefficient = 1.0 - 1.0 / t;
            vLast.Scale(scaleCoefficient); // dense operation
            bLast *= scaleCoefficient;

            double updateCoefficient = -beta / t / _parameters.regularization;
            dataVector.AddTo(vLast, updateCoefficient);
            bLast += updateCoefficient;

            // update average
            double averageingCoefficient = (t - 1) / t;
            vAvg.Scale(averageingCoefficient); // dense operation
            bAvg *= averageingCoefficient;
            vLast.AddTo(vAvg, 1 / t); // dense operation
            bAvg += bLast / t;

            exampleIterator.Next();
        }
    }
}
}