# ESP32-S3 yeni gösterge hedefi

## Mevcut durum

Mevcut `.ino` UNO/Nano için RPM/VSS okuyucusu ve harici ışık sürücüsü
tezgâh denemesidir. ESP32-S3 firmware'i değildir. `Config.h` pinleri ekran
kartına taşınmamalıdır. ESP32 tarafı için GPIO gerilim sınırı ve kartın
ekran/PSRAM/flash/USB pin kullanımı kart şemasından doğrulanmalıdır.

Yeni gösterge OEM tesisatından giriş okur, ışık simgelerini ekranda gösterir.
Eski `Lights.cpp` ise çıkış üretir. Araçtan gelen bir hattı bu çıkışlara bağlamayın.

## Öncelik ve veri kaynakları

1. RPM: koşullandırılmış tach sinyali; MPH: koşullandırılmış VSS.
2. Fuel: OEM göndericiye uygun ölçüm devresi, kalibrasyon ve sönümleme.
3. Toplam mil + Trip A/B: ilk OEM değer ve biriken VSS pulse'ları.
4. Sol/sağ sinyal ve uzun far: koşullandırılmış OEM gösterge girişleri.
5. Yağ, şarj, fren, ABS, SRS ve diğer uyarılar: doğrulanmış OEM devreleri.
6. CEL: seçilecek OBD arayüzü üzerinden veya doğrulanmış OEM gösterge hattı.

RPM/VSS dışındaki sensör ve gösterge girişleri, ekran ve OBD sürücüsü
henüz uygulanmadı. Fuel için depo dolu/boş dirençlerini veya GPIO'yu varsaymıyoruz.
Kısa farın ayrı simge mi yoksa panel aydınlatma isteği mi olduğu doğrulanmalı.

## Hazırlanan mesafe çekirdeği

`Mileage.h` Arduino'dan bağımsız bir C++ hesaplama sınıfıdır. Mevcut sketch'e
henüz bağlanmadı. `MileagePreferences.h` ESP32 için kalıcı kayıt katmanını sağlar;
ESP32 kartına entegrasyon ve donanım testleri henüz yapılmadı.

- İlk kurulumda OEM mil değeri binde bir mil birimiyle ve kalibre edilmiş
  pulse/mil değeriyle verilir. Örneğin 172450 mil → 172450000.
- 172450 örnektir; otomatik başlangıç değeri değildir.
- `addPulses()` her örneklemede alınan VSS pulse grubunu tam bir kez işler.
- Toplam mesafe = başlangıç + toplam pulse / pulse-mil.
- Trip A ve B ayrı başlangıç pulse konumları kullanır. Reset toplamı değiştirmez.
- Tam pulse sayıları korunur; her ekran güncellemesinde yuvarlama yapılmaz.
- Kalibrasyon yoksa veya kayıt yüklenmediyse değer geçersizdir; sıfır gösterilmez.
- Kurulum/restore yalnızca bir kez yapılabilir; normal toplam reset API'si yoktur.
  Bu, fiziksel erişim veya firmware değişikliğine karşı güvenlik kilidi değildir.
- Durum kaydı kalibrasyonu da taşır. Kalibrasyon değişikliği ayrı bir veri geçişi
  gerektirir; mevcut pulse geçmişi yeni katsayıyla sessizce yorumlanmamalıdır.

## Kalıcı kayıt: Preferences / NVS

Ekstra FRAM kullanılmayacak. `MileagePreferences.h` ESP32 dahili flash'ına
Preferences üzerinden yazar. Bütün sayaçlar 44 byte sürümlü, CRC kontrollü tek
blob'da tutulur; ayrı anahtarların farklı anlara ait olması önlenir. Alanlar
little-endian kodlanır, C++ struct padding'i kaydedilmez.

- VSS pulse'ları RAM'de sürekli birikir; her 0.1 mil sınırında kayıt yapılır.
- İlk OEM kurulumu ve bağımsız Trip A/B resetleri ayrıca hemen kaydedilir.
- Kayıt pulse hassasiyetini korur; görüntü yalnız bir ondalık gösterir.
- Yazma dönüş değeri ve geri okuma kontrol edilir. Hatalar `storageFault()` ile
  bildirilir; ana döngü en çok saniyede bir yeniden dener.
- Bozuk kayıt Fault üretir; otomatik sıfırlama yapılmaz. Eksik kayıt yalnız
  açık bir kurulum işlemiyle OEM değeri girilmesini sağlar.
- NVS silinmesini ilk kurulumdan yazılımla ayırt edemeyiz; bu bir fiziksel erişim kilidi değildir.
- Yaklaşık 10 uygulama kaydı/mil oluşur; trip resetleri ve ilk kurulum buna eklenir.
  NVS aşınma dağıtımını kendisi yapar; bir uygulama kaydı tek fiziksel flash
  yazma/silme işlemi değildir. Ömür flash ve NVS bölüm boyutuna bağlıdır.

Güç kesilince kayıp sağlıklı sistemde yaklaşık son 0.1 mil ve örnekleme/yazma
gecikmesidir. Kesin `<0.1 mil` garantisi yoktur: eşik geçilirken güç kesilebilir;
yazma hatalarında kayıp daha büyük olabilir.

### Entegrasyon

1. ESP32 kodunda `MileagePreferences.h` dahil edilip tek nesne oluşturulur.
2. `begin()` Loaded döndürürse devam edilir; Fault hata ekranı gerektirir.
   Uncommissioned durumunda kurulum aracı gerçek OEM değerini binde bir mil
   olarak ve doğrulanmış pulse/mil katsayısını `commission()` ile kaydeder.
3. ISR yalnız pulse sayar. Ana döngü ESP32'ye uygun kritik bölümde pulse grubunu
   alıp `addPulses(count)` ve ardından `service(millis())` çağırır. RPM/MPH hesabı
   da aynı pulse grubunu kullanabilir; sayaç ikinci kez alınmaz.
4. Ekran mesafeleri aşağı yuvarlayıp bir ondalıkla gösterir; görünen değer
   değişince alanı yeniden çizer. Açılış ve trip reseti de çizimi tetikler.
   RPM/MPH alanlarının güncelleme hızı bundan bağımsızdır.
5. Normal arayüz yalnız `resetTripA()` ve `resetTripB()` sunar. False dönüşünde
   reset RAM'de bekliyordur; kalıcı başarı bildirilmez. Servis döngüsü tekrar dener.

Katman tek task/döngüden kullanılmalıdır; ISR'den NVS çağrısı yapılmaz.
Gerçek ESP32 derlemesi ve kayıt sırasında güç kesintisi testleri henüz yapılmadı.

Kaynaklar: [Preferences API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/preferences.html),
[NVS storage](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/index.html).

## Bağlantı tablosunu tamamlamak için

ESP32-S3 ekran kartının tam modeli/şeması, aracın M/T veya A/T
olması ve doğru servis şemasındaki soket görünümleri gerekir. Son tablo:

`OEM soket/pin ve kablo rengi → koşullandırıcı kanal → ESP32 GPIO → yazılım işlevi`

Kaynak adayı: [2002–2003 RSX Factory Service Manual, Body Electrical](https://www.redliners.ru/manuals/Acura%20RSX%20%2802-03%29%20Service%20Manual.pdf).
Arama dizini Gauges / Circuit Diagram bölümünü 22-62 olarak listeliyor;
PDF içeriği bu oturumda indirilemediği için pin numaraları doğrulanmış sayılmaz.

OEM cluster sökülmeden önce şarj, SRS/ABS/fren ve diğer uyarı devrelerinin
etkilenmediği doğrulanmalı. Başlangıç OEM okuması ve değişim tarihi belgelenmeli;
yerel odometre değişimi yükümlülükleri ayrıca kontrol edilmeli.
