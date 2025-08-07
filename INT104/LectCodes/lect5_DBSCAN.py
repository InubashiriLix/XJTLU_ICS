import numpy as np
import pandas as pd

from sklearn import datasets
from sklearn.cluster import DBSCAN
import matplotlib.pyplot as plt
import seaborn as sns

iris = datasets.load_iris()

data_y = pd.DataFrame(iris.target)
data_y.columns = ["original_label"]

data_X = pd.DataFrame(iris.data)
data_X.columns = ["Sepal length", "Sepal width", "Petal length", "Petal width"]

train = pd.concat([data_X, data_y], axis=1)

train.head()

# sns.color_palette("pastel")
# sns.pairplot(train, hue="original_label")
# plt.show()

TRAIN_LABEL_COL = "original_label"
TRAIN_FEATURES = [col for col in train.columns if col != TRAIN_LABEL_COL]
X = train[TRAIN_FEATURES]
y = train[TRAIN_LABEL_COL]

dbscan = DBSCAN(eps=0.5, min_samples=5)
y_pred = dbscan.fit_predict(X)

y_pred_df = pd.DataFrame(y_pred)
y_pred_df.columns = ["pred_label"]

val = pd.concat([X, y_pred_df], axis=1)
print(val)

sns.color_palette("pastel")
sns.pairplot(val, hue="pred_label")
plt.show()
