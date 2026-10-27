# USB veri protokolü

## JSON v1

UTF-8, her mesaj sonunda LF (CRLF kabul edilir); varsayılan 115200 baud, 8 veri biti, parite yok, 1 stop biti, akış kontrolü yok. En fazla 4096 bayt/satır (LF hariç). Aşan satır bir sonraki LF dahil atılır. Bölünmüş ve tek okumada birleşmiş satırlar desteklenir. Geçersiz UTF-8 veya bozuk mesaj atılır, sonraki satır işlenir.

```json
{"version":1,"seq":123,"rpm":3250,"speed_mph":46.2,"left":true,"right":false,"low_beam":true,"high_beam":false,"cel":false,"srs":false,"charge":false,"low_fuel":false,"brake":false,"seatbelt_warning":true}
```

| Alan | Tür / anlam | Kabul aralığı |
|---|---|---|
| version | Zorunlu tam sayı sürüm | 1 |
| seq | İsteğe bağlı tam sayı sıra numarası, yalnızca doğrulanır | 0…2^53−1 |
| rpm | Motor devri, devir/dakika | 0…30000 |
| speed_mph | Hız, mil/saat | 0…400 |
| odometer_mi | Firmware kaynaklı toplam mesafe, mil | 0…1e9 |
| trip_a_mi, trip_b_mi | Firmware kaynaklı trip, mil | 0…1e7 |
| left, right | Sol/sağ sinyalin o andaki açık durumu | boolean |
| low_beam, high_beam | Kısa/uzun far açık | boolean |
| cel, srs, charge | Motor, SRS, şarj uyarısı aktif | boolean |
| low_fuel, brake, seatbelt_warning | Düşük yakıt, fren/el freni, kemer uyarısı aktif | boolean |

Tüm lamba alanlarında true aktif, false pasif demektir; 0/1 veya metin kabul edilmez. Sayılar sonlu olmalı; boolean sayıya dönüşmez. Aralıklar giriş korumasıdır, araç teknik özellikleri değildir. Bilinen bir alan yanlışsa mesajın tamamı atılır; hiçbir alanın zaman aşımı yenilenmez. null geçerli veri değildir. Bilinmeyen anahtarlar yok sayılır. version dışında tüm alanlar isteğe bağlıdır; en az bir bilinen ölçüm/lamba alanı bulunmalıdır. seq için tekrar/sıralama filtresi yoktur: USB geliş sırası kullanılır. Firmware en güncel durumunu göndermelidir.

Eksik alanlar ayrı 1.5 saniye sonra geçersizleşir; mesajın diğer alanları bunu yenilemez. Zaman ölçümü laptop monotonic saatidir. Uyarı verisi hiç gelmezse bilinmeyendir. Kesilme/yeniden bağlanma tamponu ve tüm geçerlilikleri temizler. Birimler ekranda dönüştürülür; protokol daima MPH ve mil kullanır. Trip reset komutu tanımlı değildir; uygulama seri hatta komut göndermez.

## Legacy CSV

Bağlantı panelinde ayrıca seçilmelidir; otomatik format tahmini yapılmaz.

```text
elapsed_ms,rpm_pulses,rpm,vss_pulses,vss_hz,speed_mph
500,50,3000,25,50.00,UNCALIBRATED
500,50,3000,25,50.00,46.2
```

Altı sütun gerekir. elapsed_ms örnekleme süresidir, mutlak zaman damgası değildir; laptop saati geçerlilik için kullanılır. İlk beş sütun sonlu ve negatif olmayan sayılardır; rpm en çok 30000, diğer yardımcı değerler en çok 1e12 olabilir. Başlık, başlangıç yazıları, # yanıtları ve biçime uymayan satırlar atılır. speed_mph 0…400 veya büyük harfle UNCALIBRATED olabilir. UNCALIBRATED hız anında geçersizleşir; sıfır yapılmaz. Yalnızca rpm ve speed_mph güncellenir; uyarılar ve mesafeler bilinmeyen kalır.

Mevcut firmware yaklaşık 500 ms aralıkla örnek gönderdiğinden ölçüm gecikmesi vardır. Animasyon bu gecikmeyi ortadan kaldıramaz; iki örnek arasında gerçek motor davranışı bilinmez. Gerekli örnekleme ve kalibrasyon firmware tarafında ayrıca ele alınmalıdır.
