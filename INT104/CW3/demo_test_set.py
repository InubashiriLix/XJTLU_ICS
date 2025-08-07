import os
import pandas as pd
import numpy as np
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
import matplotlib.pyplot as plt

from load_data import load_data, generate_feature_sets
from models import get_model_configs
from evaluate import evaluate_classification

# ---------- 路径 ----------
TRAIN_PATH = "training_set_MCQ.xlsx"
TEST_PATH  = "test_set_Friday.xlsx"
TRUE_BIN   = "checks_Friday.bin"
OUT_CSV    = "results/friday_test_results.csv"
BEST_IMG   = "figures/best_confusion.png"

# ========== 1. 加载数据 ==========
train_df = load_data(TRAIN_PATH)
X_train_full = train_df.drop("Programme", axis=1)
y_train = train_df["Programme"]

test_df = load_data(TEST_PATH)
cols_to_drop = [c for c in ["Programme", "Index"] if c in test_df.columns]
X_test_full = test_df.drop(cols_to_drop, axis=1)

# ========== 2. 生成特征子集 ==========
feature_sets_train = generate_feature_sets(X_train_full)
feature_sets_test  = generate_feature_sets(X_test_full)

# ========== 3. 遍历所有模型 / 配置 ==========
model_configs = get_model_configs()
all_results   = []

for feat_name, X_tr in feature_sets_train.items():
    if feat_name not in feature_sets_test:
        continue
    X_te = feature_sets_test[feat_name]

    for model_name, cfg_list in model_configs.items():
        for model, cfg_name in cfg_list:
            model.fit(X_tr, y_train)
            y_pred = model.predict(X_te)
            acc    = evaluate_classification(y_pred, TRUE_BIN)  # 百分比

            all_results.append({
                "Feature_Set": feat_name,
                "Model":       model_name,
                "Config":      cfg_name,
                "Accuracy":    acc
            })
# ========== 4. 保存所有成绩 ==========
os.makedirs(os.path.dirname(OUT_CSV), exist_ok=True)
results_df = pd.DataFrame(all_results)
results_df.to_csv(OUT_CSV, index=False)

# ========== 5. 逐模型挑最高 ==========
best_each_df = results_df.loc[
    results_df.groupby("Model")["Accuracy"].idxmax()
].reset_index(drop=True)

print("\n=== BEST COMBINATION FOR EACH MODEL ===")
print(best_each_df.to_string(index=False))
print("=======================================\n")

# ========== 6. 用每个模型自己的最佳组合重新评估 ==========
for _, row in best_each_df.iterrows():
    best_feat  = row["Feature_Set"]
    best_model = row["Model"]
    best_cfg   = row["Config"]

    # 6‑1 取模型对象
    target_model = None
    for mdl, cfg_list in model_configs.items():
        if mdl == best_model:
            for m, cfg_name in cfg_list:
                if cfg_name == best_cfg:
                    target_model = m
                    break
        if target_model:
            break

    if target_model is None:
        raise RuntimeError(f"未找到 {best_model} ({best_cfg}) 对象")

    # 6‑2 fit / predict
    X_tr_best = feature_sets_train[best_feat]
    X_te_best = feature_sets_test[best_feat]

    target_model.fit(X_tr_best, y_train)
    y_pred_best = target_model.predict(X_te_best)

    # 6‑3 评估
    final_acc = evaluate_classification(y_pred_best, TRUE_BIN)
    print(f"{best_model:12s} ({best_cfg}) | {best_feat:10s} -> {final_acc:.2f}%")
