import numpy as np
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures

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
print(f"coefficient of determination: {r_sq}")
print(f"intercept: {model.intercept_}")
print(f"slope: {model.coef_}")

y_pred = model.predict(x)
print(f"predicted response:\n{y_pred}")

x_ = PolynomialFeatures(degree=2, include_bias=False).fit_transform(x)
model = LinearRegression().fit(x_, y)

x_1 = PolynomialFeatures(degree=2, include_bias=False).fit_transform(x[0].reshape(1,-1))
r_sq = model.score(x_, y)
print(f"coefficient of determination: {r_sq}")

print(f"intercept: {model.intercept_}")

print(f"coefficients: {model.coef_}")
y_pred = model.predict(x_1)
print(f"predicted response:\n{y_pred}")