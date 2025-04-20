import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

# Load the CSV file
df = pd.read_csv("3days_data.csv")

# Convert Timestamp to datetime
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

# Define function to categorize time of day
def categorize_time_of_day(ts):
    hour = ts.hour
    if 6 <= hour < 12:
        return 'Morning'
    elif 12 <= hour < 18:
        return 'Afternoon'
    else:
        return 'Night'

# Apply categorization
df['TimeOfDay'] = df['Timestamp'].apply(categorize_time_of_day)

# Count the number of entries per time of day
counts = df['TimeOfDay'].value_counts().reindex(['Morning', 'Afternoon', 'Night'], fill_value=0)

# Plotting
plt.figure(figsize=(8, 6))
counts.plot(kind='bar', color=['#FFD700', '#FF8C00', '#4B0082'])
plt.title('Total Changes by Time of Day')
plt.xlabel('Time of Day')
plt.ylabel('Total Count')
plt.xticks(rotation=0)
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Save the plot
plt.tight_layout()
plt.savefig("time_of_day_plot.png")
plt.show()
