
# NX-FanControl

**NX-FanControl** is a Nintendo Switch homebrew utility that lets you fully customize the console’s internal fan curve. It supports up to **10 configurable points** based on SoC temperature, giving you complete control over cooling behavior and noise levels.

---

## ✨ Features

* 🧠 **Custom fan curve** — Define up to **10 temperature points** with corresponding fan speeds.
* 🌡️ **Real-time monitoring** — View current **SoC temperature** and **fan RPM** directly.
* ⚙️ **Fine-tuned control** — Balance noise, cooling, and performance to your preference.

---

## 📦 Requirements

Before building, make sure you have the [**devkitPro toolchain**](https://devkitpro.org/wiki/Getting_Started) installed and set up.

---

## 🛠️ Building from Source

Clone the repository (including submodules), fetch dependencies, and build:

```
git clone --recurse-submodules https://github.com/dominatorul/NX-FanControl.git
cd NX-FanControl
cd lib
git clone https://github.com/DaveGamble/cJSON.git
cd ..
make
```

## ⚠️ Disclaimer

This project is **homebrew software** and is **not affiliated with or endorsed by Nintendo**.
Use at your own risk — modifying fan behavior may impact system stability, performance, or hardware lifespan.

---

## 📜 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

---

Would you like me to add a **Usage** section (e.g., how to install it with Atmosphère or how to configure the fan curve)? That would make the README even more helpful for users.
