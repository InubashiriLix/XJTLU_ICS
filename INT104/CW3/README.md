# README.md

## Project Overview
This project implements and compares several machine learning classification models using scikit-learn. It evaluates their performance using stratified k-fold cross-validation and compares individual classifiers and ensemble methods. Detailed results and summaries are saved as CSV files, and confusion matrices are plotted.

### Script Descriptions

 #### models.py

- Defines multiple machine learning models and their configurations:
    1. Decision Tree: Varies max_depth.
    2. Naïve Bayes: Varies var_smoothing.
    3. k-Nearest Neighbors (kNN): Varies n_neighbors.
    4. VotingClassifier: Hard voting of DT, NB, and KNN.
    5. BaggingClassifier: Decision Tree base estimator with varying number of estimators.
    6. StackingClassifier: DT, NB, KNN as base learners; Logistic Regression as meta-classifier.

#### utils.py
- Contains helper functions for model evaluation and result handling:
    1. evaluate_model_cv: Performs stratified k-fold cross-validation.
    2. save_results_csv: Saves result DataFrames to CSV.
    3. get_confusion_matrix_plot: Computes confusion matrix using cross-validated predictions.

#### visualizer.py
- Plots and saves heatmaps of confusion matrices using Seaborn.

#### main.py
- Main execution script:
    1. Loads data and prepares multiple feature sets.
    2. Iterates over all model configurations and feature sets.
    3. Performs cross-validation, records results, and identifies best configurations.
    4. Saves all and summary results to CSV.
    5. Generates and saves confusion matrix plots.

#### load_data.py
- Loads the dataset from Excel and performs preprocessing:
    1. Drops rows with missing values.
    2. Generates multiple feature subsets (e.g., with/without Gender and Grade, Q1–Q3, etc.).

### Output Files

#### results/all_results.csv
- Contains detailed results from all cross-validation runs.
- Columns include:
    1. Model: Classifier type (e.g., DecisionTree, NaiveBayes).
    2. Feature_Set: Which feature set was used.
    3. Configuration: Specific hyperparameter setting.
    4. Fold: Fold number in cross-validation.
    5. Accuracy: Accuracy score for that fold.

#### results/average_result_summary.csv
- Summary table showing the best performance per model type.
- Columns include:
    1. Model: Classifier type. 
    2. Feature_Set: Feature set with highest accuracy.
    3. Configuration: Best hyperparameter setting.
    4. Mean_Accuracy: Mean accuracy across folds.
