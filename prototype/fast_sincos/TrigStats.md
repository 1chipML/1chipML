## Arduino UNO fast sine and cosine statistics

### Fast sine error

| Degree                 | 1             | 2             | 3             | 5             | 7             |
| ---------------------- | ------------- | ------------- | ------------- | ------------- | ------------- |
| Average absolute error | 2.0257819e-03 | 1.6181282e-05 | 5.3691248e-05 | 2.3202971e-07 | 9.5162541e-08 |
| Average relative error | 1.0108900e-02 | 4.8639300e-05 | 2.2533398e-04 | 1.0679576e-06 | 5.3257844e-07 |
| Max absolute error     | 5.8567077e-03 | 3.3497810e-05 | 1.5160441e-04 | 8.1956387e-07 | 2.9802322e-07 |
| Max relative error     | 2.7036989e-01 | 8.4236218e-04 | 9.7717997e-04 | 1.5287478e-05 | 1.0600788e-05 |

### Fast cosine error
| Degree                 | 1             | 2             | 3             | 5             | 7             |
| ---------------------- | ------------- | ------------- | ------------- | ------------- | ------------- |
| Average absolute error | 1.9016085e-03 | 1.6104057e-05 | 4.7826281e-05 | 2.1358021e-07 | 8.7972730e-08 |
| Average relative error | 8.2163326e-03 | 4.4945144e-05 | 1.9975228e-04 | 8.8590423e-07 | 4.2095914e-07 |
| Max absolute error     | 5.7379752e-03 | 3.1769276e-05 | 1.5184283e-04 | 7.5995922e-07 | 2.3841858e-07 |
| Max relative error     | 1.5970843e-01 | 1.0896173e-03 | 9.6482690e-04 | 9.6366984e-06 | 1.0675152e-05 |

### Fast Sine speed (us)
| Degree       | 1      | 2      | 3      | 5      | 7      |
| ------------ | ------ | ------ | ------ | ------ | ------ |
| sin          | 125,78 | 125,78 | 125,78 | 125,78 | 125,78 |
| fastSin      | 39,60  | 63,72  | 73,30  | 91,46  | 109,42 |
| Acceleration | 218%   | 97%    | 72%    | 38%    | 15%    |

### Fast Cosine speed (us)
| Degree       | 1      | 2      | 3      | 5      | 7      |
| ------------ | ------ | ------ | ------ | ------ | ------ |
| cos          | 126,24 | 126,24 | 126,24 | 126,24 | 126,24 |
| fastCos      | 35,54  | 63,94  | 73,74  | 91,82  | 109,28 |
| Acceleration | 255%   | 97%    | 71%    | 37%    | 16%    |

### Fast Fixed Sine error
|                        |                  |
| ---------------------- | ---------------- |
| Average absolute error | 1.5833357052e-05 |
| Max absolute error     | 4.2209289868e-05 |

### Fast Fixed Sine speed (us)
|              |        |
| ------------ | ------ |
| sin          | 125,78 |
| fastFixedSin | 13,82  |
| Acceleration | 810%   |

### Fast Fixed Cosine error
|                        |                  |
| ---------------------- | ---------------- |
| Average absolute error | 1.5833357052e-05 |
| Max absolute error     | 4.2209289868e-05 |

### Fast Fixed Cosine speed (us)
|              |        |
| ------------ | ------ |
| cos          | 125,78 |
| fastFixedSin | 14,18  |
| Acceleration | 787%   |
