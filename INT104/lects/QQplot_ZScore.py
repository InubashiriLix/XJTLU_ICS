import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as st

data = np.random.exponential(scale=1.0, size=500)
xbar = np.mean(data)

deviations = data - xbar

# get the z score
s = np.std(data, ddof=1)
# the z score is how many standard is the error with data and data mean value
z_scores = (data - xbar) / s

# calcualte the QQ plt
plt.figure(figsize=(6, 6))
st.probplot(z_scores, dist="norm", plot=plt)
plt.title("QQ Plot of Z-scores")
plt.show()
