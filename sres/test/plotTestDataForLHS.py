import pandas as pd
import matplotlib.pyplot as plt
import seaborn

scaled = pd.read_csv("scaledParams.txt")
unscaled = pd.read_csv("unscaledParams.txt")

scaled.columns = ['x', 'y', ""]
unscaled.columns = ["x", "y", ""]

scaled = scaled.iloc[:, [0, 1]]
unscaled = unscaled.iloc[:, [0, 1]]

data = {
    "scaled": scaled,
    "unscaled": unscaled,
}
df = pd.concat(data, axis=1)
df = df.unstack()
df = df.reset_index()
df = df.drop("level_2", axis=1)
df.columns = ["group", "param", "val"]

df = df[df["group"] == "unscaled"]
fig = plt.figure()
seaborn.displot(df[df["param"] == "x"], y="val", hue="group")
plt.show()
print(df)

# plt.figure()
