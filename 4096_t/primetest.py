import random, subprocess, sys

ELF = "./primetest.out"
TRIALS = int(sys.argv[1]) if len(sys.argv) > 1 else 5


def is_probable_prime(n, rounds=40):
    """Miller-Rabin, independent of the C code and of GMP."""
    if n < 2:
        return False
    for sp in (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37):
        if n % sp == 0:
            return n == sp
    d, s = n - 1, 0
    while d % 2 == 0:
        d //= 2
        s += 1
    for _ in range(rounds):
        a = random.randrange(2, n - 1)
        x = pow(a, d, n)
        if x in (1, n - 1):
            continue
        for _ in range(s - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True


fails = 0
seen = set()
for words in (4, 8, 16, 32):  # 256, 512, 1024, 2048 bits
    for _ in range(TRIALS):
        out = subprocess.check_output([ELF, str(words)]).decode().strip()
        p = int(out, 16)
        ok = (p.bit_length() == words * 64) and (p % 2 == 1) and is_probable_prime(p)
        if p in seen:
            ok = False
            print("DUPLICATE prime: the random source is not random!")
        seen.add(p)
        print("%4d bits  %s" % (words * 64, "prime, full size" if ok else "FAIL"))
        if not ok:
            fails += 1
            print("   value:", hex(p))

print("%d primes checked, %d failures" % (len(seen), fails))
sys.exit(1 if fails else 0)
