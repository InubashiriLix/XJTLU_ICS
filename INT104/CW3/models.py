import sklearn
from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.ensemble import VotingClassifier, BaggingClassifier, StackingClassifier
from sklearn.linear_model import LogisticRegression

def get_model_configs():
    """Return a dictionary of model names to lists of (model, config_name) tuples."""
    configs = {}

    # Decision Tree with various max_depth
    dt_configs = []
    for depth in [None, 3, 5, 10]:
        dt = DecisionTreeClassifier(max_depth=depth, random_state=42)
        name = f"max_depth={depth}" if depth is not None else "max_depth=None"
        dt_configs.append((dt, name))
    configs["DecisionTree"] = dt_configs

    # Naive Bayes with different var_smoothing
    nb_configs = [
        (GaussianNB(var_smoothing=1e-6), "var_smoothing=1e-6")
    ]
    configs["NaiveBayes"] = nb_configs

    # kNN with different neighbors
    knn_configs = []
    for k in [3, 5, 7, 9, 11, 13]:
        knn = KNeighborsClassifier(n_neighbors=k)
        name = f"k={k}"
        knn_configs.append((knn, name))
    configs["KNN"] = knn_configs

    # Voting Classifier (hard voting of DT, NB, KNN)
    voting_clf = VotingClassifier(estimators=[
        ('dt', DecisionTreeClassifier(random_state=42)),
        ('nb', GaussianNB()),
        ('knn', KNeighborsClassifier())
    ], voting='hard')
    configs["Voting"] = [(voting_clf, "hard_voting")]

    # Bagging (DecisionTree as base, varying number of estimators)
    bag_configs = []
    for n in [10, 50, 100]:
        # scikit‑learn ≥ 1.4 uses 'estimator', older versions use 'base_estimator'
        bag_kwargs = dict(
            n_estimators=n,
            random_state=42
        )
        # pick correct kwarg depending on version
        if sklearn.__version__ >= "1.4":
            bag_kwargs["estimator"] = DecisionTreeClassifier(random_state=42)
        else:
            bag_kwargs["base_estimator"] = DecisionTreeClassifier(random_state=42)

        bag = BaggingClassifier(**bag_kwargs)
        name = f"n_estimators={n}"
        bag_configs.append((bag, name))
    configs["Bagging"] = bag_configs

    # Stacking (DT, NB, KNN with logistic regression meta-classifier)
    stacking_clf = StackingClassifier(estimators=[
        ('dt', DecisionTreeClassifier(max_depth=5, random_state=42)),
        ('nb', GaussianNB()),
        ('knn', KNeighborsClassifier(n_neighbors=5))
    ], final_estimator=LogisticRegression(max_iter=1000))
    configs["Stacking"] = [(stacking_clf, "default")]

    return configs
