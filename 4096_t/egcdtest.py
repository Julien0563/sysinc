import random, subprocess, sys

ELF = "./egcdtest.out"
fails = 0
count = 0


def run(a, m):
    out = subprocess.check_output([ELF, hex(a), hex(m)]).decode().strip()
    return None if out == "none" else int(out, 16)


def expect(a, m):
    if m <= 1:
        return None
    try:
        return pow(a, -1, m)
    except ValueError:
        return None


def check(a, m, label=""):
    global fails, count
    count += 1
    got, want = run(a, m), expect(a, m)
    if got != want:
        fails += 1
        print("FAIL", label, "a =", hex(a), "m =", hex(m))
        print("  expected", None if want is None else hex(want))
        print("  received", None if got is None else hex(got))


check(3, 7, "3^-1 mod 7 = 5")
check(5, 7, "5^-1 mod 7 = 3")
check(65537, 3120, "65537 mod 3120")
check(17, 3120, "17^-1 mod 3120 = 2753")
check(6, 9, "no inverse (gcd 3)")
check(0, 7, "a = 0")
check(7, 7, "a = m")
check(14, 7, "a multiple of m")
check(1, 7, "a = 1")
check(6, 7, "a = m-1")
check(10, 7, "a > m")
check(5, 1, "m = 1")
check(5, 0, "m = 0")
check(2, 4, "even/even")
for m in range(2, 60):
    for a in range(0, 130):
        check(a, m, "exhaustive small")

for bits in (64, 256, 512, 1024, 2048):
    for _ in range(25):
        m = random.getrandbits(bits) | (1 << (bits - 1))
        a = random.getrandbits(random.randint(2, bits))
        check(a, m, "%d-bit random" % bits)
    for _ in range(10):
        m = (random.getrandbits(bits) | (1 << (bits - 1))) & ~1
        check(65537, m, "%d-bit e=65537" % bits)

for _ in range(int(sys.argv[1]) if len(sys.argv) > 1 else 3):
    m = random.getrandbits(4096) | (1 << 4095)
    check(65537, m & ~1, "4096-bit e=65537")
    check(random.getrandbits(4090), m, "4096-bit random")

print("%d cases, %d failures" % (count, fails))
sys.exit(1 if fails else 0)
