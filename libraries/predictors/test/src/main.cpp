////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (predictors_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictor.h"

// testing
#include "testing.h"

// linear
#include "DoubleVector.h"

using namespace emll;

void ForestPredictorTest()
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;
    using NodeId = predictors::SimpleForestPredictor::SplittableNodeId;

    // add a tree
    predictors::SimpleForestPredictor forest;
    auto tree0Root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.3 }, EdgePredictorVector{ -1.0, 1.0 } });
    forest.Split(SplitAction{ forest.GetChildId(0, 0), SplitRule{ 1, 0.6 }, EdgePredictorVector{ -2.0, 2.0 } });
    forest.Split(SplitAction{ forest.GetChildId(0, 1), SplitRule{ 2, 0.9 }, EdgePredictorVector{ -4.0, 4.0 } });

    // add another tree
    auto tree1Root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.2 }, EdgePredictorVector{ -3.0, 3.0 } });

    // test NumTrees
    testing::ProcessTest("Testing NumTrees()", forest.NumTrees() == 2);

    // test NumInteriorNodes
    testing::ProcessTest("Testing NumInteriorNodes()", forest.NumInteriorNodes() == 4);
    testing::ProcessTest("Testing NumInteriorNodes(tree0)", forest.NumInteriorNodes(tree0Root) == 3);
    testing::ProcessTest("Testing NumInteriorNodes(tree1)", forest.NumInteriorNodes(tree1Root) == 1);

    // test NumEdges
    testing::ProcessTest("Testing NumEdges()", forest.NumEdges() == 8);
    testing::ProcessTest("Testing NumEdges(tree0)", forest.NumEdges(tree0Root) == 6);
    testing::ProcessTest("Testing NumEdges(tree1)", forest.NumEdges(tree1Root) == 2);

    // test Compute
    double output = forest.Predict(linear::DoubleVector({ 0.2, 0.5, 0.0 }), tree0Root);
    testing::ProcessTest("Testing Compute(tree0)", testing::IsEqual(output, -3.0, 1.0e-8));

    output = forest.Predict(linear::DoubleVector({ 0.2, 0.7, 0.0 }), tree0Root);
    testing::ProcessTest("Testing Compute(tree0)", testing::IsEqual(output, 1.0, 1.0e-8));

    output = forest.Predict(linear::DoubleVector({ 0.5, 0.7, 0.7 }), tree0Root);
    testing::ProcessTest("Testing Compute(tree0)", testing::IsEqual(output, -3.0, 1.0e-8));

    output = forest.Predict(linear::DoubleVector({ 0.5, 0.7, 1.0 }), tree0Root);
    testing::ProcessTest("Testing Compute(tree0)", testing::IsEqual(output, 5.0, 1.0e-8));

    output = forest.Predict(linear::DoubleVector({ 0.2, 0.5, 0.0 }));
    testing::ProcessTest("Testing Compute()", testing::IsEqual(output, -6.0, 1.0e-8));

    output = forest.Predict(linear::DoubleVector({ 0.25, 0.7, 0.0 }));
    testing::ProcessTest("Testing Compute()", testing::IsEqual(output, 4.0, 1.0e-8));

    // test path generation
    auto edgeIndicator = forest.GetEdgeIndicatorVector(linear::DoubleVector({ 0.25, 0.7, 0.0 }));
    testing::ProcessTest("Testing SetEdgeIndicatorVector()", testing::IsEqual(edgeIndicator, std::vector<bool>{ 1, 0, 0, 1, 0, 0, 0, 1 }));
}

/// Runs all tests
///
int main()
{
    ForestPredictorTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}