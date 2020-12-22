import math


class Point:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def cross(self, p2):
        x = (self.y * p2.z) - (self.z * p2.y)
        y = (self.z * p2.x) - (self.x * p2.z)
        z = (self.x * p2.y) - (self.y * p2.x)
        return Point(x, y, z)

    def dot(self, p2):
        return (self.x * p2.x) + (self.y * p2.y) + (self.z * p2.z)

    def norm(self):
        return ((self.x * self.x) +
                (self.y * self.y) +
                (self.z * self.z)
                ) ** (0.5)

    def __sub__(self, p2):
        return Point(self.x - p2.x, self.y - p2.y, self.z - p2.z)

    def __str__(self):
        return f"{self.x}, {self.y}, {self.z}"

    def __repr__(self):
        return self.__str__()


def barycentre(A, B, C):
    # angle A == CAB
    AB = B - A
    AC = C - A

    crossed = AB.cross(AC)
    signe = crossed.z / abs(crossed.z)
    tan_A = signe * (crossed.norm() / AB.dot(AC))

    # angle B == ABC
    BA = A - B
    BC = C - B

    crossed = BC.cross(BA)
    signe = crossed.z / abs(crossed.z)
    tan_B = signe * (crossed.norm() / BC.dot(BA))

    # angle C == BCA
    CB = B - C
    CA = A - C

    crossed = CA.cross(CB)
    signe = crossed.z / abs(crossed.z)
    tan_C = signe * (crossed.norm() / CA.dot(CB))

    print(f"tan A = {tan_A}, tan B = {tan_B}, tan C = {tan_C}")

    tmp_alpha = tan_C + tan_B
    tmp_beta = tan_A + tan_C
    tmp_gamma = tan_B + tan_A

    print(f"tmp_alpha {tmp_alpha}, tmp_beta {tmp_beta}, tmp_gamma {tmp_gamma}")

    alpha = tmp_alpha / (tmp_alpha + tmp_beta + tmp_gamma)
    beta = tmp_beta / (tmp_alpha + tmp_beta + tmp_gamma)
    gamma = tmp_gamma / (tmp_alpha + tmp_beta + tmp_gamma)

    print(f"alpha {alpha}, beta {beta}, gamma {gamma}")

    return Point(alpha, beta, gamma)


def init():

    A = Point(1, 0, 0)
    B = Point(0.65, 1, 0)
    C = Point(0.5, 0, 0)
    return A, B, C


if __name__ == "__main__":
    A = Point(1, 0, 0)
    B = Point(0.65, 1, 0)
    C = Point(0.5, 0, 0)

    b = barycentre(A, B, C)
    print(b)
