import numpy as np
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures
from sklearn.metrics import mean_squared_error

# x = np.array([
#     [206.2868217, 162.4392765],
#     [112.2826087, 110.8478261],
#     [104.0526316, 101.7368421],
#     [84.86111111, 82.94444444],
#     [83.32624113, 76.60283688],
#     [56.99367089, 62.24050633],
#     [66.32704403, 65.12578616],
#     [55.83707865, 58.02808989],
#     [47.47619048, 42.28571429],
# ])

x = np.array([
    [206.2868217],
    [112.2826087],
    [104.0526316],
    [84.86111111],
    [83.32624113],
    [56.99367089],
    [66.32704403],
    [55.83707865],
    [47.47619048],
])

y = np.array([50, 80, 100, 130, 150, 180, 200, 250, 300])

model = LinearRegression()
model.fit(x,y)

r_sq = model.score(x, y)
print("----------------Linear Regression----------------")
print(f"coefficient of determination: {r_sq}")
print(f"intercept: {model.intercept_}")
print(f"slope: {model.coef_}")

y_pred = model.predict(x)
print(f"predicted response:\n{y_pred}")

x_ = PolynomialFeatures(degree=2, include_bias=False).fit_transform(x)
model = LinearRegression().fit(x_, y)

x_1 = PolynomialFeatures(degree=2, include_bias=False).fit_transform(x[0].reshape(1,-1))
r_sq = model.score(x_, y)

print("----------------Polynomial Regression----------------")
print(f"coefficient of determination: {r_sq}")

print(f"intercept: {model.intercept_}")

print(f"coefficients: {model.coef_}")
y_pred = model.predict(x_)
print(f"predicted response:\n{y_pred}")

## Calculate mean square error
Y_true = np.array([
    [40],
    [50],
    [70],
    [100],
    [150],
    [200],
    [220],
    [250],
    [270],
])

Y_pred_linear = np.array([
    [4],
    [77.78],
    [57.294],
    [143.37],
    [165.23],
    [199.39],
    [206.22],
    [214.41],
    [222.61],
])

Y_pred_quadratic = np.array([
    [49.631],
    [28.606],
    [24.35],
    [94.414],
    [134.02],
    [213.6],
    [232.11],
    [255.45],
    [280.04],
])
mse_linear = mean_squared_error(Y_true,Y_pred_linear)
mse_quadratic = mean_squared_error(Y_true,Y_pred_quadratic)

print(f"MSE Linear : {mse_linear}")
print(f"MSE Quadratic : {mse_quadratic}")
