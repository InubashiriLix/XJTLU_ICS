import os
import pandas as pd
from load_data import load_data, generate_feature_sets
from models import get_model_configs
from utils import evaluate_model_cv, save_results_csv, get_confusion_matrix_plot
from visualizer import plot_confusion_matrix
import warnings

if __name__ == "__main__":
    warnings.filterwarnings("ignore")
    os.makedirs("results", exist_ok=True)
    os.makedirs("figures", exist_ok=True)

    # Load and preprocess data
    data = load_data("training_set.xlsx")
    X_full = data.drop("Programme", axis=1)
    y = data["Programme"]

    # Define feature sets (dict of name -> DataFrame)
    feature_sets = generate_feature_sets(X_full)
    # Get model configurations (model instances + hyperparameter descriptions)
    model_configs = get_model_configs()

    all_results = []  # collect all cross-validation results
    best_models_info = {}  # track best model config for each model type

    for feat_name, X in feature_sets.items():
        print(f"Evaluating feature set: {feat_name}")
        for model_name, configs in model_configs.items():
            print(f"  Model: {model_name}")
            for model, config_name in configs:
                res = evaluate_model_cv(model, X, y, model_name, feat_name, config_name, cv=5)
                all_results.extend(res)
                mean_acc = pd.DataFrame(res)["Accuracy"].mean()
                if (model_name not in best_models_info) or (mean_acc > best_models_info[model_name]["accuracy"]):
                    best_models_info[model_name] = {
                        "feat_name": feat_name,
                        "feat_cols": list(X.columns),
                        "model": model,
                        "config_name": config_name,
                        "accuracy": mean_acc
                    }

    all_results_df = pd.DataFrame(all_results)
    save_results_csv(all_results_df, "results/all_results.csv")

    summary_rows = []
    for model_name, info in best_models_info.items():
        summary_rows.append({
            "Model": model_name,
            "Feature_Set": info["feat_name"],
            "Configuration": info["config_name"],
            "Mean_Accuracy": info["accuracy"]
        })
    summary_df = pd.DataFrame(summary_rows)
    save_results_csv(summary_df, "results/average_result_summary.csv")

    for model_name, info in best_models_info.items():
        X_best = X_full[info["feat_cols"]]
        cm = get_confusion_matrix_plot(info["model"], X_best, y, cv=5)
        fig_path = f"figures/confusion_{model_name}.png"
        plot_confusion_matrix(cm, title=f"{model_name} Confusion Matrix", filename=fig_path)
        print(f"Saved confusion matrix for {model_name} at {fig_path}")
