# RSX cluster bağlantı ve kullanım

## Ortak ground

İzolasyonsuz sürücü kullanılıyorsa Arduino GND, sürücü GND ve cluster'ın
belgelenmiş güç ground'u ortak referansa bağlanır. Kodda bunun için ayar yoktur.
Optik izolasyonlu arayüzde ise izolasyon tasarımını izleyin; iki tarafın ground'unu
birleştirmek izolasyonu ortadan kaldırabilir.

Ortak ground, gerilim uyumluluğu sağlamaz. Cluster'ın 12 V hatları Arduino'ya
doğrudan bağlanmaz. Cluster güç beslemesi, kontak beslemesi ve ground soket
pinleri ayrıca doğrulanmalıdır. Arduino cluster'ı beslemez.

## Arduino pinleri ve sinyal yönü

**RSX servis dokümanı bu proje klasöründe bulunamadı.** Aşağıdaki Arduino pinleri
kodda tanımlıdır; araç soket numaraları ve aktif seviyeleri henüz doğrulanmamıştır.
Model yılına ve doğru soket görünüşüne ait şema olmadan kablo bağlamayın.
Bu tablo bir RSX pinout'u değildir.

| İşlev | RSX soket/pin | Bağlantı yolu | Arduino |
|---|---|---|---|
| RPM okuma | Doğrulanacak | Araç sinyali → korumalı 0–5 V koşullandırıcı → Arduino | D2 giriş |
| VSS okuma | Doğrulanacak | Araç sinyali → korumalı 0–5 V koşullandırıcı → Arduino | D3 giriş |
| CEL göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D4 çıkış |
| Cruise göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D5 çıkış |
| SRS göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D6 çıkış |
| Kısa far / aydınlatma isteği | Ayrı gösterge olup olmadığı doğrulanacak | Arduino → uygun harici sürücü → doğrulanmış cluster girişi | D7 çıkış |
| Uzun far göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D8 çıkış |
| Sol sinyal göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D9 çıkış |
| Sağ sinyal göstergesi | Doğrulanacak | Arduino → harici sürücü → cluster girişi | D10 çıkış |
| Güç ground'u | Doğrulanacak | Cluster ground ↔ sürücü ground ↔ Arduino GND (izolasyonsuz devre) | GND |

Ground ile aktif olan doğrulanmış bir giriş için uygun düşük taraf sürücüsü;
pozitif gerilim ile aktif olan giriş için uygun yüksek taraf sürücüsü gerekir.
Sürücü türünü şema ve ölçüm belirler. `Config.h` içindeki `*_ON_LEVEL`,
**sürücünün Arduino tarafındaki lojik seviyesidir**, araç hattının gerilimi değildir.
Varsayılan hepsi HIGH: ON = HIGH, OFF = LOW.
Reset/yükleme sırasında pinler giriş olabilir; sürücüde donanımsal OFF bias'ı
bulunmalıdır. Yazılım başlangıçta bütün çıkışları OFF yapar.

Bu komutlar yalnızca gösterge kontrolü içindir. Cruise sistemini, airbag sistemini
veya dış farları çalıştırmaz. SRS devrelerine/airbag ateşleyicilerine bağlanmayın;
mevcut araçtaki uyarı sistemlerini bu çıkışlarla bastırmayın. Önce bağımsız
cluster/tezgâh düzeneğinde doğrulayın.

## Hemen denenebilen komutlar

Arduino IDE Serial Monitor: **115200 baud**, satır sonu **Newline** veya **Both NL & CR**.
Harici sürücü yokken çıkışları uygun seri dirençli test LED'leriyle doğrulayabilirsiniz.

```text
CEL ON
CEL OFF
CRUISE ON
SRS ON
LOWBEAM ON
HIGHBEAM ON
LEFT ON
LEFT OFF
RIGHT ON
LEFT BLINK
RIGHT BLINK
HAZARD ON
HAZARD OFF
ALL OFF
LIGHTS
HELP
```

Her kanal ON/OFF veya 1/0 kabul eder; küçük harf de kullanılabilir.
ON sürekli yakar; LEFT/RIGHT BLINK 500 ms açık / 500 ms kapalı çalışır.
HAZARD ON iki sinyali eşzamanlı yanıp söndürür; HAZARD OFF ikisini kapatır.
Bir taraftan diğerine geçmek için önce `HAZARD OFF`, ardından `LEFT BLINK`
veya `RIGHT BLINK` gönderin. `ALL OFF` tüm yanıp sönmeleri de durdurur.
`LIGHTS` anlık çıkış durumlarını ve BLINK modlarını gösterir.
Yanıp sönme `delay()` kullanmaz; RPM/VSS örnekleme devam eder.

Işık cevapları `#` ile başlar. CSV tüketen uygulama başlangıç mesajlarını ve bu
cevapları ayıklamalıdır. Mevcut RPM/VSS kodu **sinyal okur**; cluster ibrelerini
sürmek için RPM/VSS darbesi üretmez.
