import pandas as pd
import matplotlib.pyplot as plt

import sys

# Read in the CSV file

df = pd.read_csv(sys.argv[1])

node_color = {
    (1, 1): 'blue',  # Fast and high CPU
    (1, 0): 'black', # Fast and low CPU
    (0, 1): 'red',   # Slow and high CPU
    (0, 0): 'purple' # Slow and low CPU
}

colors = [node_color[(row['SpeedNode'], row['CpuUsageNode'])] for _, row in df.iterrows()]

sizes = [row['Ratio'] * 100 * 4 for _, row in df.iterrows()]

fig, ax = plt.subplots()
scatter = ax.scatter(df['BlocksMinedInLongestChain'], df['TotalBlocksMined'], c=colors, s=sizes)

ax.set_xlabel('Blocks Mined in Longest Chain')
ax.set_ylabel('Total Blocks Mined')
ax.set_title('Ratio of Blocks Mined in Longest Chain to Total Blocks Mined')

ax.set_facecolor('lightgrey')  # set plot background color to light grey


# for _, row in df.iterrows():
#     ax.annotate(int(row['NodeId']), (row['BlocksMinedInLongestChain'], row['TotalBlocksMined']), ha='center', va='center', color='white', fontsize=10, xytext=(-2, 2), textcoords='offset points')

legend_elements = [
    plt.scatter([], [], c='blue', s=100, label='Fast and High CPU'),
    plt.scatter([], [], c='black', s=100, label='Fast and Low CPU'),
    plt.scatter([], [], c='red', s=100, label='Slow and High CPU'),
    plt.scatter([], [], c='purple', s=100, label='Slow and Low CPU')
]
ax.legend(handles=legend_elements)

plt.show()
