## Model Evaluation, classify into Good,ok,bad (FP = False Positives)

Good:
-Up
-Down
-Right
-Left 
-x 
-circle
-0

ok:
-6 (FP = 8,circle)
-7 (FP = x,z)
-9 (FP 6,8)
-v (FP = 7,W)
-y (FP = 7)


bad (needs retraining):
-8 (FP = 9,6)
-w (FP = v)
-z (FP = x)
