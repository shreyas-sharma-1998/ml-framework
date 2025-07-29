import argparse
import time
import numpy as np
import pandas as pd

def callback_function(i, error, model):
    print(f"Iteration {i}, error {error:.20f}")
    return True

def ss_linear_regression_fit(X, y, iterations, learning_rate, callback=None):
    m, n = X.shape
    X_bias = np.c_[np.ones((m, 1)), X]
    model = np.random.randn(n + 1)

    for i in range(1, iterations + 1):
        y_hat = X_bias @ model
        error_vector = y_hat - y
        cost = (error_vector @ error_vector) / (2 * m)

        if callback and not callback(i, cost, model):
            break

        gradient = (X_bias.T @ error_vector) * (learning_rate / m)
        model -= gradient

    return model

# --- Main execution starts here ---
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="Path to the CSV dataset")
parser.add_argument("iterations", type=int, help="Number of training iterations")
parser.add_argument("learning_rate", type=float, help="Learning rate")
args = parser.parse_args()

start_time = time.time()

# Load dataset
data = pd.read_csv(args.filename, header=None)
X = data.iloc[:, :-1].values
y = data.iloc[:, -1].values

# Fit model
model = ss_linear_regression_fit(X, y, args.iterations, args.learning_rate, callback_function)

# Save model
np.savetxt("model.csv", model, delimiter=",")

time.sleep(5)
end_time = time.time()
elapsed = end_time - start_time

print("Task completed.")
print(f"Elapsed time: {elapsed:.2f} seconds")