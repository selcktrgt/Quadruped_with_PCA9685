# RS422 Kaynak Secici Birimi — STM32F103 (2 giris / 1 cikis)

Ek-A Teknik Gereksinimler ve SDDS/SMASH 200/30 Arayuz Dokumanina gore
hazirlanmis, **2 girisli / 1 cikisli** RS422 kaynak secici biriminin
STM32CubeIDE (STM32F103, HAL) uygulamasi. Cikis sayisi arttirilacagi
zaman ayni `rs422_selector` modulunden birden fazla ornek (instance)
turetilerek genisletilebilir (bkz. "Daha sonra ek cikis" bolumu).

## Neyi karsiliyor?

| Gereksinim (Ek-A) | Karsilanma sekli |
|---|---|
| Md.2 — 2 farkli kaynaktan NMEA0183 ve/veya binary veri | Byte-seviyesinde saydam aktarim; icerik yorumlanmaz, hem ASCII hem binary akislarda calisir |
| Md.3 — 4800..115200 bps, 1..100 Hz | `RS422_UART_BAUDRATE` (main.c) ile ayarlanir; veri hizi/frekansi cihazi ilgilendirmez, sadece byte akisi tasinir |
| Md.4 — Kaynaklar arasi gecis <=100 ms | `RS422_SOURCE_TIMEOUT_MS = 100` |
| Md.5 — Kaynak Onceliklendirme Mekanizmasi | AUTO modda SRC1 birincil oncelikli; SRC1 canliysa hep o secilir |
| Md.6 — Secilen kaynak cikis hattina | Aktif kaynagin byte'lari kesme + halka tampon ile USART3 TX'e aktarilir |
| Md.7 — Manuel kaynak gecisi | 3 konumlu kol/anahtar -> ADC1_IN0 -> AUTO / FORCE SRC1 / FORCE SRC2 |

Md.8 (IP64), Md.9 (STANAG 1008 / 115VAC-24VDC besleme), Md.10 (guc
gostergesi) ve Md.11 (guc donusumu) donanim/mekanik/kutu tasarimi ile
ilgilidir, bu firmware'in kapsami disindadir; Md.10 icin oneri asagida.

## Dosyalar

```
RS422_Selector_STM32F103/
  Core/Inc/main.h
  Core/Inc/rs422_selector.h     <- yapilandirma sabitleri burada
  Core/Src/main.c               <- CubeMX USER CODE yapisina uygun
  Core/Src/rs422_selector.c     <- secim/failover/relay mantigi
  Core/Src/stm32f1xx_hal_msp.c  <- periferik GPIO/saat/NVIC ilklendirme
  Core/Src/stm32f1xx_it.c       <- kesme isleyicileri
```

`Drivers/` (CMSIS + STM32F1xx HAL kutuphanesi), `startup_stm32f103xb.s`,
linker script'i ve `stm32f1xx_hal_conf.h` bu depoya dahil edilmedi;
bunlar STM32CubeMX/CubeIDE tarafindan otomatik uretilir (asagidaki
adimlarla). Sadece uygulamaya ozgu dosyalar elle yazilmistir.

## STM32CubeIDE'de proje olusturma

1. **File > New > STM32 Project** ile MCU olarak kullandigin F103'u sec
   (ornekte **STM32F103C8T6**, "Blue Pill" karti varsayilmistir; farkli
   bir F103 kartinsa sadece pin atamalarini asagidaki tabloya gore
   degistir, uygulama mantigi degismez).
2. Pinout & Configuration sekmesinde:
   - **USART1**: Mode = *Asynchronous*, sonra Parameter Settings'te
     Hardware Flow Control = None. **NVIC**'te USART1 kesmesini ac.
     RX pini PA10 olarak kalsin; TX pinini kullanmayacaksan Pinout
     görünümünde PA9'u GPIO olarak birakabilirsin (opsiyonel).
   - **USART2**: Ayni sekilde Asynchronous, NVIC kesmesi acik, RX = PA3.
   - **USART3**: Asynchronous, NVIC kesmesi acik, TX = PB10.
   - Her uc USART icin **Baud Rate = 115200**, Word Length = 8 Bits,
     Parity = None, Stop Bits = 1 (SDDS arayuz dokumaniyla birebir).
   - **ADC1**: IN0 kanalini ac (PA0), Independent mode, tek donusum.
   - **GPIO Output**: PC13 ve PB1'i cikis olarak ayarla (opsiyonel
     SRC1/SRC2 aktif LED gostergeleri; istemiyorsan main.h'deki
     `SRC1_ACTIVE_LED_*` / `SRC2_ACTIVE_LED_*` define'larini ve
     main.c'deki ilgili satirlari kaldir).
   - Clock Configuration: HSE 8 MHz -> SYSCLK 72 MHz (PLL x9).
3. **Project > Generate Code** ile iskeleti uret. Bu adim
   `Drivers/`, `startup_*.s`, linker script ve `stm32f1xx_hal_conf.h`
   dosyalarini otomatik olusturur.
4. Uretilen `Core/Inc/main.h`, `Core/Src/main.c`,
   `Core/Src/stm32f1xx_hal_msp.c`, `Core/Src/stm32f1xx_it.c`
   dosyalarinin **USER CODE** bolumlerini bu depodaki ayni isimli
   dosyalarla degistir/birlestir (CubeMX'in kendi ilklendirme kodu ile
   buradaki UART Mode = RX only / TX only, ADC kanal ayari ve
   callback yonlendirmelerinin ayni oldugunu dogrula — .ioc'ta yukaridaki
   adimlar dogru uygulandiysa pratikte birebir eslesir).
5. `rs422_selector.h` ve `rs422_selector.c` dosyalarini `Core/Inc` ve
   `Core/Src` altina ekle (proje aciklamasina/derleme listesine dahil
   olduklarindan emin ol).
6. Derle ve karta yukle.

> Not: `HAL_UART_Init()` cagrisinda `Init.Mode` alanini `UART_MODE_RX`
> / `UART_MODE_TX` olarak CubeMX GUI'sinde secemiyorsan (bazi eski
> CubeMX surumlerinde bu secenek gizli olabilir), `MX_USARTx_UART_Init()`
> fonksiyonunun icindeki `huartX.Init.Mode = UART_MODE_TX_RX;` satirini
> bu depodaki gibi `UART_MODE_RX` / `UART_MODE_TX` olarak elle
> degistirmen yeterlidir; RS422 tam dupleks oldugu icin kullanilmayan
> yon donanimsal olarak zaten bagli degildir.

## Donanim / kablolama

RS422 diferansiyel hatlari ile MCU arasina, her hat icin ayri bir
RS422 alici (ornegin SN75176/MAX3095, sadece A/B -> RXD) veya surucu
(ornegin MAX3488/SN65HVD, TXD -> A/B) IC'si konulmalidir:

```
Kaynak 1 (RS422, A1/B1) --> RS422 Alici IC --> PA10 (USART1_RX)
Kaynak 2 (RS422, A2/B2) --> RS422 Alici IC --> PA3  (USART2_RX)
PB10 (USART3_TX)        --> RS422 Surucu IC --> Cikis (RS422, A_out/B_out)
```

RS422 noktadan-noktaya tam dupleks oldugundan RS485'in aksine DE/RE
yon kontrol pini gerekmez (alici IC'nin RE'si surekli aktif, surucu
IC'nin DE'si surekli aktif baglanir).

### Manuel secim kolu/anahtari (ADC, PA0)

3 konumlu bir anahtar/kol (SRC1 / AUTO / SRC2) ile PA0 pinine 3 ayri
gerilim seviyesi uygulanir:

```
      VCC (3.3V)
       |
      [R]
       |
       +---- AUTO konumu (~1.65V, orta uc)
       |
      [R]
       |
      GND
```

- Anahtar **SRC1** konumundayken PA0 dogrudan **GND**'ye
- Anahtar **AUTO** konumundayken PA0, iki esit direncin ortasina
  (~1.65 V)
- Anahtar **SRC2** konumundayken PA0 dogrudan **VCC**'ye (3.3V) baglanir

`rs422_selector.h` icindeki `RS422_ADC_TH_*` esikleri bu 3 seviyeye
gore ayarlanmistir (12-bit ADC, Vref=3.3V):

| ADC degeri (0..4095) | Yaklasik gerilim | Mod |
|---|---|---|
| 0 .. 1000 | 0 .. 0.8 V | FORCE SRC1 |
| 1600 .. 2500 | 1.3 .. 2.0 V | AUTO |
| 3100 .. 4095 | 2.5 .. 3.3 V | FORCE SRC2 |

Aradaki bolgeler "olu bolge"dir; anahtar gecis anindaki ara degerlerde
yazilim bir onceki modu korur (titresim/gurultuye karsi). Kendi
anahtarinin/direnc degerlerinin urettigi gercek gerilimlere gore bu
esikleri kalibre et. **Not:** PA0'a VCC/GND kisa devre baglantisi
yaparken araya seri bir koruma direnci (ornegin 1 kOhm) koymani
oneririm; boylece anahtar arizasinda/yanlis kablolamada pime asiri akim
gitmez.

## Calisma mantigi (ozet)

- USART1/USART2 uzerinden gelen her byte kesmede yakalanir ve o
  kaynagin "son veri zamani" (`HAL_GetTick()`) guncellenir.
- `RS422_Selector_Process()` ana dongude surekli cagrilir:
  - Her 20 ms'de bir ADC'den anahtar konumu okunur (AUTO/FORCE SRC1/
    FORCE SRC2).
  - AUTO modda: SRC1'den son 100 ms icinde veri geldiyse SRC1 aktif
    kalir/secilir (oncelik SRC1'de). SRC1, 100 ms'den uzun sure
    sessiz kalirsa ve SRC2 canliysa otomatik olarak SRC2'ye gecilir.
    SRC1 tekrar veri gondermeye basladigi an, oncelikli kaynak oldugu
    icin otomatik olarak geri SRC1'e donulur.
  - FORCE SRC1 / FORCE SRC2 modlarinda kaynagin canli olup olmadigina
    bakilmaksizin secim anahtarin dedigi kaynakta sabit kalir.
- Aktif kaynaktan gelen her byte, kucuk bir halka tampon (ring buffer)
  araciligiyla kesme guduml olarak USART3 TX'ten cikisa aktarilir;
  aktif olmayan kaynagin byte'lari (canliligini izlemek disinda)
  cikisa yazilmaz.
- Boylece hem NMEA0183 (ASCII, degisken uzunlukta, CR/LF sonlu) hem de
  SMASH binary 100 Hz/1 Hz mesajlari, icerikleri hic ayristirilmadan
  saydam sekilde aktarilmis olur.

## Daha sonra ek cikis eklemek icin

`rs422_selector` modulu, giris/cikis UART handle'larini parametre
olarak alacak sekilde tasarlandi (global degil). Birden fazla cikisa
ayni secili kaynagi ayni anda yaymak istersen en basit yol,
`RS422_Selector_OnRxByte()` icindeki `RS422_TxEnqueue()` cagrisinin
ikinci/ucuncu bir cikis UART'i icin de (kendi halka tamponu ve TX
callback'iyle) tekrarlanmasidir; farkli cikislarin farkli kaynak
secebilmesi gerekiyorsa (ör. Tablo 1'deki cok cikisli kutular) modul
`static` degiskenler yerine bir `RS422_SelectorCtx_t` struct'ina
tasinip her cikis icin ayri bir baglam (instance) olusturulacak sekilde
genisletilebilir — mevcut kod tek bir global baglam varsayimiyla
yazildi, bu bilerek yapilan bir sadelestirme (once 2x1'i saglamlastir,
sonra coka cikart).

## Guc gostergesi (Md.10) hakkinda not

Gereksinim, cihazin enerjili oldugunu gosteren bir gostergeyi (LED)
istiyor. Bu, MCU yazilimindan bagimsiz, dogrudan regüle besleme
hattina (ornegin 3.3V veya 5V raya) seri bir direncle baglanan basit
bir LED ile en guvenilir sekilde saglanir (MCU/yazilim coksede LED
yanmaya devam eder, gercek "enerjili" bilgisini verir). `main.c`
icindeki `SRC1_ACTIVE_LED` / `SRC2_ACTIVE_LED` ise bunun yerine gecmez,
sadece hangi kaynagin secili oldugunu gosteren ayri bir tanilama
ozelligidir.
