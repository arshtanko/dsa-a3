import csv
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def read_csv(path):
    with open(path) as f:
        reader = csv.DictReader(f)
        rows = list(reader)
    return rows


rows1 = read_csv("graph1.csv")
t1 = [int(r["t"]) for r in rows1]
ft0_1 = [int(r["Ft0"]) for r in rows1]
nt_1 = [float(r["Nt"]) for r in rows1]

plt.figure(figsize=(10, 5))
plt.plot(t1, ft0_1, lw=2, label="$F_t^0$ ")
plt.plot(t1, nt_1, lw=2, label="$N_t$ ")
plt.xlabel("Размер потока")
plt.ylabel("Уникальных элементов")
plt.title("Сравнение $N_t$ и $F_t^0$")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("graph1.png", dpi=150)
plt.close()
print("graph1.png saved")

rows2 = read_csv("graph2.csv")
t2 = [int(r["t"]) for r in rows2]
ft0_2 = [int(r["Ft0"]) for r in rows2]
mean_2 = [float(r["E_Nt"]) for r in rows2]
lower_2 = [float(r["lower"]) for r in rows2]
upper_2 = [float(r["upper"]) for r in rows2]

plt.figure(figsize=(10, 5))
plt.fill_between(
    t2,
    lower_2,
    upper_2,
    alpha=0.3,
    color="blue",
    label=r"$E(N_t) \pm \sigma$",
)
plt.plot(t2, mean_2, lw=2, color="blue", label="$E(N_t)$")
plt.plot(t2, ft0_2, lw=2, color="red", linestyle="--", label="$F_t^0$ (exact)")
plt.xlabel("Размер потока")
plt.ylabel("Уникальных элементов")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("graph2.png", dpi=150)
plt.close()
print("graph2.png saved")
