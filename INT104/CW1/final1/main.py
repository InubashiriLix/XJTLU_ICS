#!/usr/bin/env python
# -*- coding: utf-8 -*-
# main.py – run all clustering experiments and save results

from itertools import product
from datetime import datetime

import pandas as pd
from sklearn.metrics import adjusted_rand_score

from clustering_models import clustering_algorithms
from data_loader import load_data
from utils import cluster_accuracy_score
from ensemble import voting_ensemble, stacking_ensemble

X_sets, y = load_data("training_set.xlsx")          # adjust path if needed
true_class_count = len(set(y))

results = []
print("Starting clustering experiments...\n")

for feat_name, X in X_sets.items():
    print(f"Feature set: {feat_name}")
    for alg in clustering_algorithms:
        alg_name = alg["name"]
        func = alg["function"]
        param_grid = alg["params"]

        for combo in product(*param_grid.values()):
            params = dict(zip(param_grid.keys(), combo))
            try:
                labels = func(X, **params)
            except Exception as e:
                print(f"  - {alg_name} failed, params {params}: {e}")
                continue

            ari = adjusted_rand_score(y, labels)
            acc = cluster_accuracy_score(y, labels)
            results.append(
                dict(
                    FeatureSet=feat_name,
                    Algorithm=alg_name,
                    Params=str(params),
                    ARI=ari,
                    Accuracy=acc,
                )
            )
            print(f"  - {alg_name}  {params}  ->  ARI={ari:.3f}  Acc={acc:.3f}")
    print("")

ts = datetime.now().strftime("%Y%m%d_%H%M%S")
full_csv = f"clustering_results_full_{ts}.csv"
results_df = pd.DataFrame(results)
results_df.to_csv(full_csv, index=False)
print(f"\n>>> Full results saved: {full_csv}")


best_rows = []
for feat in results_df["FeatureSet"].unique():
    for alg in results_df["Algorithm"].unique():
        sub = results_df[(results_df["FeatureSet"] == feat) &
                         (results_df["Algorithm"] == alg)]
        if sub.empty:
            continue
        best_rows.append(sub.sort_values("ARI", ascending=False).iloc[0])

best_df = pd.DataFrame(best_rows).reset_index(drop=True)

feature_for_summary = "PCA3D"
X_summary = X_sets[feature_for_summary]

base_labels = [
    # KMeans
    clustering_algorithms[0]["function"](X_summary, n_clusters=true_class_count),
    # Agglomerative
    clustering_algorithms[1]["function"](X_summary, n_clusters=true_class_count, linkage="ward"),
    # DBSCAN
    clustering_algorithms[2]["function"](X_summary, eps=1.0, min_samples=5),
    # GaussianMixture
    clustering_algorithms[3]["function"](X_summary, n_components=true_class_count),
]

vote_pred = voting_ensemble(base_labels, y)
stack_pred = stacking_ensemble(base_labels, n_clusters=true_class_count)

best_df = pd.concat(
    [
        best_df,
        pd.DataFrame(
            [
                dict(
                    FeatureSet=feature_for_summary,
                    Algorithm="VotingEnsemble",
                    Params="-",
                    ARI=adjusted_rand_score(y, vote_pred),
                    Accuracy=cluster_accuracy_score(y, vote_pred),
                ),
                dict(
                    FeatureSet=feature_for_summary,
                    Algorithm="StackingEnsemble",
                    Params="-",
                    ARI=adjusted_rand_score(y, stack_pred),
                    Accuracy=cluster_accuracy_score(y, stack_pred),
                ),
            ]
        ),
    ],
    ignore_index=True,
)

best_csv = f"clustering_results_best_{ts}.csv"
best_df.to_csv(best_csv, index=False)
print(f"Summary of best results saved: {best_csv}")
print(best_df.to_string(index=False))
