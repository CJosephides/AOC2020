import math
import numpy as np

def lcm(a, b):
    return int(abs(a*b) / math.gcd(a, b))  # sometimes we get a floating integer...


def find_pairwise_coincidence(n1, p1, n2, p2):
    """
    n: numbers (trip period)
    p: phase (minutes after 0)
    """
    max_idx = np.argmax((n1, n2))
    x_max = [n1, n2][max_idx]
    p_max = [p1, p2][max_idx]

    x = x_max
    
    while True:
        if ((x - (p_max - p1)) % n1) == ((x - (p_max - p2)) % n2) == 0:
            if ((x - (p_max - p1)) > 0) and ((x - (p_max - p2))) > 0:
                return (x - p_max, n1 * n2)
        x += x_max


def group_pairs(b1, p1, b2, p2):
    """
    b: bases (first incidence)
    p: period (of the pair)
    """

    seen = set()
    b = [b1, b2]
    p = [p1, p2]
    c = 1
    while True:
        for i in range(2):
            x = b[i] + p[i] * c
            if x in seen:
                return (x, lcm(p1, p2))
            seen.add(x)
        c += 1


#if __name__ == "__main__":
numbers = [23, 41, 829, 13, 17, 29, 677, 37, 19]
orders = [0, 13, 23, 36, 37, 52, 54, 60, 73]  # aka phases
n = len(numbers)

# Create n-1 pairs.
pairwise_incidences = []
for i in range(n-1):
    pairwise_incidences.append(find_pairwise_coincidence(numbers[i], orders[i], numbers[i+1], orders[i+1]))

# Now do a pairwise grouping of pairs until we reduce to a single element.
while len(pairwise_incidences) > 1:
    pairwise_incidences_new = []
    for i in range(len(pairwise_incidences) - 1):
        pairwise_incidences_new.append(group_pairs(*pairwise_incidences[i], *pairwise_incidences[i+1]))
    pairwise_incidences = pairwise_incidences_new.copy()

print(pairwise_incidences)
