import pandas as pd

from evaluate import evaluate_classification
from load_data import load_data, generate_feature_sets
from models import get_model_configs
from utils import save_results_csv, get_confusion_matrix_plot, evaluate_model_cv
from visualizer import plot_confusion_matrix

data = load_data("training_set_MCQ.xlsx")
X_full = data.drop("Programme", axis=1)
y = data["Programme"]

X_test = load_data("test_set_Friday.xlsx")

feature_sets = generate_feature_sets(X_full)
# model_configs = get_model_configs()
#
# all_results = []
# best_models_info = {}
#
# for feat_name, X in feature_sets.items():
#     print("Evaluating feature set:", feat_name)
#     for model_name, configs in model_configs.items():
#         print("  Model:", model_name)
#         for model, config_name in configs:
#             res = evaluate_model_cv(model, X, y, model_name, feat_name, config_name, cv=5)
#             all_results.extend(res)
#             mean_acc = pd.DataFrame(res)["Accuracy"].mean()
#             if (model_name not in best_models_info) or (mean_acc > best_models_info[model_name]["accuracy"]):
#                 best_models_info[model_name] = {
#                     "feat_name": feat_name,
#                     "feat_cols": list(X.columns),
#                     "model": model,
#                     "config_name": config_name,
#                     "accuracy": mean_acc
#                 }
#
# all_results_df = pd.DataFrame(all_results)
# save_results_csv(all_results_df, "results/demo_all_results.csv")
#
# best_fold_results = all_results_df.loc[
#     all_results_df.groupby("Model")["Accuracy"].idxmax()
# ].reset_index(drop = True)
# save_results_csv(best_fold_results, "results/demo_best_fold_results.csv")
#
# summary_rows = []
# for model_name, info in best_models_info.items():
#     summary_rows.append({
#         "Model": model_name,
#         "Feature_Set": info["feat_name"],
#         "Configuration": info["config_name"],
#         "Mean_Accuracy": info["accuracy"]
#     })
# summary_df = pd.DataFrame(summary_rows)
# save_results_csv(summary_df, "results/demo_average_result_summary.csv")
#
# for model_name, info in best_models_info.items():
#     X_best = X_full[info["feat_cols"]]
#     cm = get_confusion_matrix_plot(info["model"], X_best, y, cv=5)
#     fig_path = f"figures/confusion_{model_name}.png"
#     plot_confusion_matrix(cm, title=f"{model_name} Confusion Matrix", filename=fig_path)
#     print(f"Saved confusion matrix for {model_name} at {fig_path}")


# Bagging,No Gender,n_estimators=10,5,0.6129032258064516
# DecisionTree,No Gender,max_depth=5,1,0.6382978723404256
# KNN,All features,k=7,5,0.6021505376344086
# NaiveBayes,All features,var_smoothing=1e-09,1,0.648936170212766
# Stacking,No Gender,default,1,0.6702127659574468
# Voting,All features,hard_voting,5,0.6344086021505376

import sklearn

from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.ensemble import VotingClassifier, BaggingClassifier, StackingClassifier
from sklearn.linear_model import LogisticRegression

train_df = load_data("training_set_MCQ.xlsx")
X_train_full = train_df.drop("Programme", axis=1)
y_train = train_df["Programme"]
feature_sets_train = generate_feature_sets(X_train_full)

test_df = load_data("test_set_Friday.xlsx")
X_test_full = test_df.drop("Index", axis=1)
feature_sets_test = generate_feature_sets(X_test_full)

best_configs = {
    "Bagging":   {"feat": "No Gender", "params": {"n_estimators": 10}},
    "DecisionTree": {"feat": "No Gender", "params": {"max_depth": 5, "random_state": 42}},
    "KNN":       {"feat": "All features", "params": {"n_neighbors": 7}},
    "NaiveBayes": {"feat": "All features", "params": {"var_smoothing": 1e-9}},
    "Stacking":  {"feat": "No Gender", "params": {
         "estimators": [
             ('dt', DecisionTreeClassifier(max_depth=5, random_state=42)),
             ('nb', GaussianNB()),
             ('knn', KNeighborsClassifier(n_neighbors=5))
         ],
         "final_estimator": LogisticRegression(max_iter=1000)
    }},
    "Voting":    {"feat": "All features", "params": {
         "estimators": [
             ('dt', DecisionTreeClassifier(random_state=42)),
             ('nb', GaussianNB()),
             ('knn', KNeighborsClassifier())
         ],
         "voting": "hard"
    }},
}

# —— 4. 循环训练 & 在 Friday 上预测
for name, cfg in best_configs.items():
    # 训练集子集
    X_tr = feature_sets_train[cfg["feat"]]
    # 测试集子集
    X_te = feature_sets_test[cfg["feat"]]

    # 重建模型
    if name == "Bagging":
        model = BaggingClassifier(
            estimator=DecisionTreeClassifier(random_state=42),
            **cfg["params"], random_state=42
        )
    elif name == "Stacking":
        model = StackingClassifier(**cfg["params"])
    elif name == "Voting":
        model = VotingClassifier(**cfg["params"])
    else:
        ModelClass = {
            "DecisionTree": DecisionTreeClassifier,
            "KNN":          KNeighborsClassifier,
            "NaiveBayes":   GaussianNB
        }[name]
        model = ModelClass(**cfg["params"])

    # 在训练集上 fit
    model.fit(X_tr, y_train)
    # 在 Friday 测试集上 predict
    y_pred = model.predict(X_te)

    # 对比 checks_Friday.bin（50 条真值）
    acc = evaluate_classification(y_pred, "checks_Friday.bin")
    print(f"{name} on Friday accuracy: {acc:.2f}%")