# RSX Cluster

## Çalıştırma

1. Bilgisayarına **Python 3.11 veya üzerini** kur. Windows'ta kurarken **Add Python to PATH** seçeneğini işaretle.
2. GitHub'dan **Code → Download ZIP** ile indirip ZIP'i çıkart.
3. Çıkarttığın klasörde terminal aç ve aşağıdaki komutları çalıştır.

### Windows (PowerShell)

```powershell
cd Cluster_UI
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe main.py
```

Sonraki açılışlarda `Cluster_UI` klasöründe sadece:

```powershell
.\.venv\Scripts\python.exe main.py
```

### macOS / Linux

```sh
cd Cluster_UI
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python main.py
```

Sonraki açılışlarda `Cluster_UI` klasöründe sadece:

```sh
sh start.sh
```

İlk kurulumda internet gerekir. Uygulama demo modunda açılır; Arduino veya araç bağlantısı gerekmez.
