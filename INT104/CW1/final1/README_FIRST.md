# Readme First

## PYTHON ENV
a python venv has been given in the root directory of the project.
some packages seems have version issues, so please USE THE GIVEN ENVIRONMENT.

### structure
| Path / File                | Purpose                                                                                         |
|----------------------------|--------------------------------------------------------------------------------------------------|
| **data_loader.py**         | • Reads **training_set.xlsx**  <br>• Builds **4 feature sets** *(Scores, Demo, PCA2D, PCA3D)*   |
| **clustering_models.py**   | Thin wrappers around *KMeans, Agglomerative, DBSCAN, GaussianMixture* + a parameter grid         |
| **ensemble.py**            | • `voting_ensemble` (majority vote)  <br>• `stacking_ensemble` (one-hot → KMeans)                |
| **utils.py**               | `cluster_accuracy_score` → Hungarian-based best mapping; handles `None/-1` robustly              |
| **main.py**                | *Entry script* – loops through **Feature × Algorithm × Grid**, logs metrics, writes CSVs         |
| **training_set.xlsx**      | Source data (ID, Gender, Grade, Q1–Q5, etc.). **Programme column is *only* used as ground truth**|
| `clustering_results_full_<timestamp>.csv` | Raw log – every experiment row *(FeatureSet, Algorithm, Params, ARI, Accuracy)*  |
| `clustering_results_best_<timestamp>.csv` | Condensed table – **best-ARI entry per (Feature × Algorithm)** + Two ensemble rows |

### Result CSV
#### clustering_results_full_YYYYMMDD_HHMMSS.csv
Every experiment run = 1 row:

| Column        | Meaning                                                                                   |
|---------------|-------------------------------------------------------------------------------------------|
| `FeatureSet`  | One of **Scores / Demo / PCA2D / PCA3D**                                                  |
| `Algorithm`   | Clustering method name                                                                    |
| `Params`      | Hyper-parameter combo (dict-str) used for that run                                        |
| `ARI`         | *Adjusted Rand Index* against true `Programme` labels (range −1 → 1)                      |
| `Accuracy`    | Best-mapping cluster accuracy (Hungarian alignment)                                       |

#### clustering_results_best_YYYYMMDD_HHMMSS.csv
For each Feature × Algorithm the row with highest ARI is kept.
Two extra rows append ensemble scores:

| Algorithm          | Notes                                                          |
|--------------------|----------------------------------------------------------------|
| `VotingEnsemble`   | Majority vote over 4 base models on **PCA3D** features         |
| `StackingEnsemble` | One-hot of the same 4 label sets → KMeans `k=#classes`         |

