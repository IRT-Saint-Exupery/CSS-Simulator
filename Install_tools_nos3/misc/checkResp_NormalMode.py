import matplotlib.pyplot as plt

# Initialize lists to store data from each column
column1 = []
column2 = []
column3 = []
column4 = []

# Read the file and extract data
with open('/tmp/ctrlErr.txt', 'r') as file:
    for line in file:
        values = line.split()
        column1.append(float(values[0]))
        column2.append(float(values[1]))
        column3.append(float(values[2]))
        column4.append(float(values[3]))

# Plot the data
plt.figure(figsize=(10, 6))
plt.plot(column1, label='Column1')
plt.plot(column2, label='Column2')
plt.plot(column3, label='Column3')
plt.plot(column4, label='Column4')

# Add labels and title
plt.xlabel('Time (seconds)')
plt.ylabel('Values')
plt.title('Plot of 4 Columns from ctrlErr.txt')
plt.legend()

# Show the plot
plt.show()
