import numpy as np
from scipy.optimize import linear_sum_assignment
from sklearn.metrics import adjusted_rand_score


def cluster_accuracy_score(y_true, y_pred):
    """
    Compute clustering accuracy given true labels and predicted cluster labels.
    This finds the optimal one-to-one mapping between cluster labels and true labels (using the Hungarian algorithm)
    to maximize the accuracy, then returns the fraction of correctly classified samples.

      - If y_true or y_pred is None, returns np.nan.
      - Ignores samples with missing labels (None or NaN in y_true or y_pred).
      - Handles cluster label -1 (e.g., noise in DBSCAN) as an ordinary cluster label (it will be treated as its own cluster).
    Parameters:
        y_true (array-like): True class labels.
        y_pred (array-like): Predicted cluster labels.
    Returns:
        float: Optimized accuracy (between 0 and 1). Returns np.nan if no valid samples to compare.
    """
    if y_true is None or y_pred is None:
        return np.nan
    y_true = np.array(y_true)
    y_pred = np.array(y_pred)
    # Filter out invalid entries (None or NaN in either array)
    mask = np.ones(len(y_true), dtype=bool)
    for i, (yt, yp) in enumerate(zip(y_true, y_pred)):
        # Mark false if true or pred label is None or NaN
        if yt is None or yp is None:
            mask[i] = False
        else:
            # If either is nan (and not the other)
            try:
                # np.isnan works for float or array; if yt/yp are not float, it will throw
                if np.isnan(yt) or np.isnan(yp):
                    mask[i] = False
            except:
                # If values are not numeric (e.g., string labels), ignore this check
                pass
    y_true_f = y_true[mask]
    y_pred_f = y_pred[mask]
    if y_true_f.size == 0 or y_pred_f.size == 0:
        return np.nan

    # Get unique labels
    true_labels = np.unique(y_true_f)
    pred_labels = np.unique(y_pred_f)
    # Construct contingency matrix (rows=true labels, cols=predicted clusters)
    # Map labels to indices
    true_index = {label: idx for idx, label in enumerate(true_labels)}
    pred_index = {label: idx for idx, label in enumerate(pred_labels)}
    contingency = np.zeros((true_labels.size, pred_labels.size), dtype=np.int64)
    for true_val, pred_val in zip(y_true_f, y_pred_f):
        i = true_index[true_val]
        j = pred_index[pred_val]
        contingency[i, j] += 1
    # Pad the contingency matrix to square matrix (for Hungarian algorithm if needed)
    n_true, n_pred = contingency.shape
    if n_true != n_pred:
        # Create a square matrix of size max(n_true, n_pred)
        n = max(n_true, n_pred)
        padded = np.zeros((n, n), dtype=np.int64)
        padded[:n_true, :n_pred] = contingency
        contingency = padded
        n_true = n_pred = n
    # Solve assignment problem (maximize accuracy -> maximize sum of matched counts)
    # We convert it to a minimization problem by negating counts
    cost_matrix = -contingency
    row_ind, col_ind = linear_sum_assignment(cost_matrix)
    total_matched = contingency[row_ind, col_ind].sum()
    accuracy = total_matched / len(y_true_f)
    return accuracy
