# RSX Digital Cluster

Python + PySide6 / Qt Quick ile yerel masaüstü gösterge. Tarayıcı, web sunucusu veya firmware içermez. İlk açılış DEMO modundadır. Siyah zemindeki ikonlar bu proje için çizilmiş yaklaşık vektör sembollerdir; resmi otomotiv sembolleri değildir.

## macOS kurulumu

Intel veya Apple Silicon üzerinde mimarinize uygun Python 3.11 veya daha yeni bir Python kullanın. PySide6 Qt kitaplıklarını pip ile getirir; ayrı Qt kurulumu gerekmez. Python ve bağımlılıkların aynı mimaride olması gerekir. Bu kaynak proje macOS paketleme/imzalama yapmaz.

Terminalde proje klasöründe:

```sh
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
python main.py
```

Sonraki açılışlar için tek komut (sanal ortamı etkinleştirmek gerekmez):

```sh
sh start.sh
```

Windows geliştirme ortamında: `.venv\Scripts\python main.py`.

## Kullanım

Ana ekran donanım olmadan deterministik 30 saniyelik hızlanma, vites geçişi RPM düşüşleri, sabit sürüş ve yavaşlama senaryosu gösterir. Mesafeler örnektir. Sağ alttaki Ayarlar / Demo panelinde senaryo duraklatılır; elle kontrol açılınca senaryo RPM/hız yazmaz. Elle hız girişi her zaman MPH cinsindedir. Her lamba ayrı test edilir; demo sinyallerini açıp kapatabilirsiniz. Panel kapalıyken sürüş alanını örtmez.

Tam ekran düğmesi veya F11 ile geçiş yapın; Escape tam ekrandan çıkar ve ayar panelini kapatır. Ölçek, kırmızı bölge, MPH/km/h ve RPM filtresi panelden uygulanır. 9000/7000 RPM başlangıçları yalnızca görsel örnektir, herhangi bir araç için doğrulanmış değerler değildir. Ayarlar uygulama oturumu boyunca geçerlidir.

USB için panelden portları yenileyin, portu, baud değerini (varsayılan 115200) ve JSON ya da Legacy CSV modunu seçip Bağlan düğmesine basın. Birçok UNO kartı port açılınca yeniden başlayabilir. Gerekiyorsa kartın USB sürücüsünü ayrıca kurun. Kes düğmesi gerçek modu korur; otomatik demo geçişi yapılmaz. Demo düğmesiyle açıkça geri dönülür. GPIO veya araç kablolaması bu projenin kapsamında değildir.

## Gösterim ve geçerlilik

QtSerialPort readyRead olayları bloklamadan işlenir. Ekran modeli seri mesajlardan bağımsız 16 ms zamanlayıcıyla güncellenir; Qt Quick ekranı sunar. Hedef yaklaşık 60 FPS'tir, donanım üzerinde ölçülmüş bir garanti değildir. RPM için zaman tabanlı üstel filtre kullanılır (varsayılan zaman sabiti 85 ms; yaklaşık %95 yakınsama 255 ms). Yeni mesajlar animasyon başlatmaz, yalnızca hedefi değiştirir. Filtreyi 0 ms ile kapatabilirsiniz. Hız tam sayı yuvarlama ve 0.65 birimlik histerezisle kararlı gösterilir; zaman filtresi uygulanmaz. Lambalar anında değişir; gerçek sinyallere ek yanıp sönme uygulanmaz.

Her alanın ayrı 1.5 saniye geçerlilik süresi vardır (rsx/model.py içindeki timeout). Eksik alan mevcut değerini yalnızca bu süre dolana kadar korur. Sonrasında sayı çizgi, RPM barı boş ve lamba soru işaretli bilinmeyen durumundadır. RPM veya hız geçersizse VERİ YOK yazısı görünür; kalan geçerli alanlar gösterilebilir. Hiç alınmamış alan baştan bilinmeyendir. Kapalı lamba sönük, açık lamba renkli, bilinmeyen lamba soru işaretlidir. Bağlantı kesilince tüm gerçek alanlar hemen temizlenir. Port açılışından önce tampon ve önceki ölçümler temizlenir.

Gerçek toplam/trip mesafeleri hesaplanmaz, diske yazılmaz ve reset kontrolü yoktur. Firmware bu alanları sağlamazsa çizgi görünür.

## Protokol

[PROTOCOL.md](PROTOCOL.md) JSON ve CSV ayrıntılarını içerir.

## Testler

```sh
python -m unittest discover -s tests -v
```

Testler Qt gerektirmeden parçalı/birleşik seri veriyi, taşan satırı, bozuk UTF-8/JSON, NaN/sonsuz/tür/aralık hatalarını, alan bazlı zaman aşımını, yeniden bağlantı temizliğini ve demo kaynak ayrımını kontrol eder. Qt duman testi: `python smoke_test.py`; offscreen yükleme, demo ilerlemesi, boyutlandırma ve tam ekran durum geçişlerini kontrol edip `preview.png` üretir. Bu, fiziksel ekran performansı veya USB donanım testi yerine geçmez.

Dosya düzeni: protocol.py çözümleme, serial_source.py USB yaşam döngüsü, model.py geçerlilik, demo.py örnek kaynak, app.py Qt bağlantıları/gösterim filtresi, Main.qml arayüz, assets/ yerel SVG ikonları.


## Eliptik RPM yay?

RPM bar? bas?k bir ?st elips yay?d?r. `rsx/RpmGeometry.js` d?? elips boyunca e?it yay uzunlu?u aral?klar? hesaplar; segment kenarlar? d?? elips normalinin i? elips ile kesi?imine uzan?r. QML vekt?r yollar? yaln?zca ?st s?n?r/redline de?i?ince olu?turulur. RPM g?ncellemeleri mevcut renkli yollar?n doluluk k?rpmas?n? de?i?tirir. Redline tam RPM oran?nda b?l?n?r, ?l?ek i?aretlerine yuvarlanmaz. ?l?ek 1000 RPM'den ba?lar ve 500 RPM ad?mlarla ?st s?n?ra kadar devam eder; ?rne?in 8000 ?st s?n?r?nda son etiket 8'dir. 500'?n kat? olmayan bir ?st s?n?rda son etiket s?n?r?n alt?ndaki son 500 RPM de?eridir; yay yine tam ?st s?n?ra uzan?r.

Geometri kontrolleri (yaln?zca geli?tirme i?in Node.js gerekir; uygulaman?n Node.js ba??ml?l??? yoktur):

```sh
node tests/test_rpm_geometry.cjs
```

Qt kuruluysa `python smoke_test.py`, ?st s?n?r 8000/redline 6800 ayarlar?nda 1000, 3500, 6800 ve 8000 RPM i?in 1280?720, 800?480 ve 1600?900 boyutlar?nda 12 `preview-*.png` g?r?nt?s? ?retir. Bu testin g?r?nt?leri ayr?ca g?zle incelenmelidir; otomatik geometri testi fiziksel pencere g?r?nt?s?n? do?rulamaz.
