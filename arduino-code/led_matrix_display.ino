#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <BluetoothSerial.h>

#define Cihaz_Tipi MD_MAX72XX::GENERIC_HW
#define Maksimum_cihaz 4
#define CLK_PIN   14  // ya da SCK
#define DATA_PIN  13  // ya da MOSI
#define CS_PIN    12  // ya da SS

MD_MAX72XX mx = MD_MAX72XX(Cihaz_Tipi, DATA_PIN, CLK_PIN, CS_PIN, Maksimum_cihaz); // Kütüphaneye pinler ve cihaz tanıtılıyor.

BluetoothSerial SerialBT;

#define Calistir_DEMO  1
#include <SPI.h>

#if Calistir_DEMO
#define Demo_gecikmesi  15  // her demo elemanını gösterme süresi saniye cinsinden
#else
#include <MD_UISwitch.h>
#endif

#define DEBUG 1   // Hata ayıklama çıktısını etkinleştir veya devre dışı bırak (varsayılan)
#if DEBUG
#define PRINT(s, v)   { Serial.print(F(s)); Serial.print(v); }        // Bir dizeyi ve bir değeri (onluk) takip et
#define PRINTX(s, v)  { Serial.print(F(s)); Serial.print(v, HEX); }   // Bir dizeyi ve bir değeri (onaltılık) takip et
#define PRINTB(s, v)  { Serial.print(F(s)); Serial.print(v, BIN); }   // Bir dizeyi ve bir değeri (ikili) takip et
#define PRINTC(s, v)  { Serial.print(F(s)); Serial.print((char)v); }  // Bir dizeyi ve bir değeri (karakter) takip et
#define PRINTS(s)     { Serial.print(F(s)); }                         // Bir dizeyi takip et
#else
#define PRINT(s, v)   // Bir dizeyi ve bir değeri (onluk) takip et
#define PRINTX(s, v)  // Bir dizeyi ve bir değeri (onaltılık) takip et
#define PRINTB(s, v)  // Bir dizeyi ve bir değeri (ikili) takip et
#define PRINTC(s, v)  // Bir dizeyi ve bir değeri (karakter) takip et
#define PRINTS(s)     // Bir dizeyi takip et
#endif

#if !Calistir_DEMO
#define MODE_SWITCH 9 // Dijital Pin

MD_UISwitch_Digital ks = MD_UISwitch_Digital(MODE_SWITCH, LOW);
#endif

#define BIRIM_GECIKME      25  // milisaniye cinsinden çeşitli gecikmeler
#define KAYDIRMA_GECIKMESI    (4 * BIRIM_GECIKME)
#define TARAMA_GECIKMESI   (2 * BIRIM_GECIKME)
#define RASTGELE_GECIKMESI    (6 * BIRIM_GECIKME)
#define SOLMA_GECIKMESI      (8 * BIRIM_GECIKME)
#define SpekTRUM_DELAY  (4 * BIRIM_GECIKME)
#define KALPATISI_GECIKMESI (1 * BIRIM_GECIKME)
#define KALPLER_GECIKMESI    (28 * BIRIM_GECIKME)
#define GOZLER_GECIKMESI      (20 * BIRIM_GECIKME)
#define SILECEK_GECIKMESI     (1 * BIRIM_GECIKME)
#define OKLAR_GECIKMESI    (3 * BIRIM_GECIKME)
#define OKLARTERS_GECIKMESI    (8 * BIRIM_GECIKME)
#define BOCEK_GECIKMESI   (6 * BIRIM_GECIKME)
#define PACMAN_GECIKMESI    (4 * BIRIM_GECIKME)
#define DALGA_GECIKMESI      (2 * BIRIM_GECIKME)

#define CHAR_SPACING     1  // karakterler arası piksel
#define BUF_SIZE        75  // karakterler arasındaki boşluk boyutu

uint32_t oncekianimasyonzamani = 0;    // Animasyonlardaki millis() değerini hatırlamak için kullanılır
#if Calistir_DEMO
uint32_t oncekidemozamani = 0;      // Demo döngüsündeki millis() değerini hatırlamak için kullanılır
uint8_t  Demozamani = Demo_gecikmesi; // bu demo döngüsündeki kalan saniye sayısı
#endif


bool YaziKaydirma(bool bInit, const String &pmsg)
{
  static char   curMessage[BUF_SIZE];
  static char   *p = curMessage;
  static uint8_t  state = 0;
  static uint8_t  curLen, showLen;
  static uint8_t  cBuf[8];
  uint8_t         colData;

  if (bInit)
  {
    PRINTS("\n--- YaziKaydirma Başlatılıyor");
    MatrixiSifirla();
    pmsg.toCharArray(curMessage, BUF_SIZE - 1);
    curMessage[BUF_SIZE - 1] = '\0';
    state = 0;
    p = curMessage;
    bInit = false;
    Serial.println(p);
  }

  if (millis()-oncekianimasyonzamani < KAYDIRMA_GECIKMESI)
    return(bInit);

  mx.transform(MD_MAX72XX::TSL);
  oncekianimasyonzamani = millis();

  PRINT("\nYazi Kaydirma S:", state);
  switch (state)
  {
    case 0:
      PRINTC("\nYükleniyor ", *p);
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state = 1;

    case 1:
      colData = cBuf[curLen++];
      mx.setColumn(0, colData);
      if (curLen == showLen)
      {
        showLen = ((*p != '\0') ? CHAR_SPACING : mx.getColumnCount()-1);
        curLen = 0;
        state = 2;
      }
      break;

    case 2:
      mx.setColumn(0, 0);
      if (++curLen == showLen)
      {
        state = 0;
        bInit = (*p == '\0');
      }
      break;

    default:
      state = 0;
  }

  return(bInit);
}


bool grafikTarayici(bool bInit)
{
  const uint8_t width = 3;
  static uint8_t  idx = 0;
  static int8_t   idOffs = 1;

  if (bInit)
  {
    PRINTS("\n--- Tarayıcı başlatılıyor");
    MatrixiSifirla();
    idx = 0;
    idOffs = 1;
    bInit = false;
  }

  if (millis()-oncekianimasyonzamani < TARAMA_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();

  PRINT("\nS R:", idx);
  PRINT(" D:", idOffs);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  for (uint8_t i=0; i<width; i++)
    mx.setColumn(idx+i, 0);

  idx += idOffs;
  if ((idx == 0) || (idx + width == mx.getColumnCount()))
    idOffs = -idOffs;

  for (uint8_t i=0; i<width; i++)
    mx.setColumn(idx+i, 0xff);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}
bool grafikRastgele(bool bInit)
{
  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Rastgele başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < RASTGELE_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i=0; i<mx.getColumnCount(); i++)
    mx.setColumn(i, (uint8_t)random(255));
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikKaydirici(bool bInit)
{
  const uint8_t   width = 3;     // kaydırma çubuğunun genişliği
  const uint8_t   offset = mx.getColumnCount()/3;
  static uint8_t  idx = 0;      // sayıcı

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Kaydırıcı başlat");
    MatrixiSifirla();
    idx = 0;
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < TARAMA_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  PRINT("\nS I:", idx);

  // şimdi animasyonu çalıştır
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  mx.transform(MD_MAX72XX::TSL);

  mx.setColumn(0, idx>=0 && idx<width ? 0xff : 0);
  if (++idx == offset) idx = 0;

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikSpektrum1(bool bInit)
{
  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Spektrum1 başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < SpekTRUM_DELAY)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i=0; i<Maksimum_cihaz; i++)
  {
    uint8_t r = random(ROW_SIZE);
    uint8_t cd = 0;

    for (uint8_t j=0; j<r; j++)
      cd |= 1<<j;
    for (uint8_t j=1; j<COL_SIZE-1; j++)
      mx.setColumn(i, j, ~cd);
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikSpektrum2(bool bInit)
{
  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Spektrum2 başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis() - oncekianimasyonzamani < SpekTRUM_DELAY)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i = 0; i<mx.getColumnCount(); i++)
  {
    uint8_t r = random(ROW_SIZE);
    uint8_t cd = 0;

    for (uint8_t j = 0; j<r; j++)
      cd |= 1 << j;

    mx.setColumn(i, ~cd);
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikKalpatisi(bool bInit)
{
  #define BASELINE_ROW  4

  static uint8_t  state;
  static uint8_t  r, c;
  static bool     bPoint;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Kalp atışı başlat");
    MatrixiSifirla();
    state = 0;
    r = BASELINE_ROW;
    c = mx.getColumnCount()-1;
    bPoint = true;
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < KALPATISI_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  PRINT("\nHB S:", state);
  PRINT(" R: ", r);
  PRINT(" C: ", c);
  PRINT(" P: ", bPoint);
  mx.setPoint(r, c, bPoint);

  switch (state)
  {
    case 0: // sağ taraftan düz çizgi
      if (c == mx.getColumnCount()/2 + COL_SIZE)
        state = 1;
      c--;
      break;

    case 1: // ilk vuruş
      if (r != 0) { r--; c--; }
      else state = 2;
      break;

    case 2: // aşağı vuruş
      if (r != ROW_SIZE-1) { r++; c--; }
      else state = 3;
      break;

    case 3: // ikinci yukarı vuruş
      if (r != BASELINE_ROW) { r--; c--; }
      else state = 4;
      break;

    case 4: // soldan düz çizgi
      if (c == 0)
      {
        c = mx.getColumnCount()-1;
        bPoint = !bPoint;
        state = 0;
      }
      else c--;
      break;

    default:
      state = 0;
  }

  return(bInit);
}

bool grafikSolma(bool bInit)
{
  static uint8_t  intensity = 0;
  static int8_t   iOffs = 1;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Solma başlat");
    MatrixiSifirla();
    mx.control(MD_MAX72XX::INTENSITY, intensity);

    // Tüm LED'leri açık konumuna getir
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (uint8_t i=0; i<mx.getColumnCount(); i++)
      mx.setColumn(i, 0xff);
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < SOLMA_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  intensity += iOffs;
  PRINT("\nF I:", intensity);
  PRINT(" D:", iOffs);
  if ((intensity == 0) || (intensity == MAX_INTENSITY))
    iOffs = -iOffs;
  mx.control(MD_MAX72XX::INTENSITY, intensity);

  return(bInit);
}

bool grafikKalpler(bool bInit)
{
#define Adet_Kalpler  ((Maksimum_cihaz/2) +1)
  const uint8_t kalpDolu[] = { 0x1c, 0x3e, 0x7e, 0xfc };
  const uint8_t kalpBos[] = { 0x1c, 0x22, 0x42, 0x84 };
  const uint8_t offset = mx.getColumnCount()/(Adet_Kalpler+1);
  const uint8_t dataSize = (sizeof(kalpDolu)/sizeof(kalpDolu[0]));

  static bool     bBos;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Kalpler başlat");
    MatrixiSifirla();
    bBos = true;
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < KALPLER_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  PRINT("\nH E:", bBos);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t h=1; h<=Adet_Kalpler; h++)
  {
    for (uint8_t i=0; i<dataSize; i++)
    {
      mx.setColumn((h*offset)-dataSize+i, bBos ? kalpBos[i] : kalpDolu[i]);
      mx.setColumn((h*offset)+dataSize-i-1, bBos ? kalpBos[i] : kalpDolu[i]);
    }
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  bBos = !bBos;

  return(bInit);
}

bool grafikGozler(bool bInit)
{
#define Adet_Gozler  2
  const uint8_t gozAcik[] = { 0x18, 0x3c, 0x66, 0x66 };
  const uint8_t gozKapali[] = { 0x18, 0x3c, 0x3c, 0x3c };
  const uint8_t offset = mx.getColumnCount()/(Adet_Gozler+1);
  const uint8_t dataSize = (sizeof(gozAcik)/sizeof(gozAcik[0]));

  bool bAcik;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Gözler başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < GOZLER_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  bAcik = (random(1000) > 100);
  PRINT("\nG E:", bAcik);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t e=1; e<=Adet_Gozler; e++)
  {
    for (uint8_t i=0; i<dataSize; i++)
    {
      mx.setColumn((e*offset)-dataSize+i, bAcik ? gozAcik[i] : gozKapali[i]);
      mx.setColumn((e*offset)+dataSize-i-1, bAcik ? gozAcik[i] : gozKapali[i]);
    }
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikSekentop(bool bInit)
{
  static uint8_t  idx = 0;      // pozisyon
  static int8_t   idOffs = 1;   // artış yönü

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Sekentop başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < TARAMA_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  PRINT("\nZT R:", idx);
  PRINT(" D:", idOffs);

  // şimdi animasyonu çalıştır
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // eski topu kapat
  mx.setColumn(idx, 0);
  mx.setColumn(idx+1, 0);

  idx += idOffs;
  if ((idx == 0) || (idx == mx.getColumnCount()-2))
    idOffs = -idOffs;

  // yeni çizgileri aç
  mx.setColumn(idx, 0x18);
  mx.setColumn(idx+1, 0x18);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikKayanOk(bool bInit)
{
  const uint8_t ok[] = { 0x3c, 0x66, 0xc3, 0x99 };
  const uint8_t dataSize = (sizeof(ok)/sizeof(ok[0]));

  static uint8_t  idx = 0;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- OkKaydır başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < OKLAR_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  PRINT("\nOK I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.transform(MD_MAX72XX::TSL);
  mx.setColumn(0, ok[idx++]);
  if (idx == dataSize) idx = 0;
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikSilecek(bool bInit)
{
  static uint8_t  idx = 0;      // pozisyon
  static int8_t   idOffs = 1;   // artış yönü

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Silecek başlat");
    MatrixiSifirla();
    bInit = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < SILECEK_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  PRINT("\nSWP R:", idx);
  PRINT(" D:", idOffs);

  // şimdi animasyonu çalıştır
  mx.setColumn(idx, idOffs == 1 ? 0xff : 0);
  idx += idOffs;
  if ((idx == 0) || (idx == mx.getColumnCount()))
    idOffs = -idOffs;

  return(bInit);
}

bool grafikBocek(bool bInit)
{
  const uint8_t Bocek1[] = { 0x0e, 0x98, 0x7d, 0x36, 0x3c };
  const uint8_t Bocek2[] = { 0x70, 0x18, 0x7d, 0xb6, 0x3c };
  const uint8_t dataSize = (sizeof(Bocek1)/sizeof(Bocek1[0]));

  static int8_t idx;
  static bool   iType;

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Bocek başlat");
    MatrixiSifirla();
    bInit = false;
    idx = -dataSize;
    iType = false;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis()-oncekianimasyonzamani < BOCEK_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  // şimdi animasyonu çalıştır
  PRINT("\nUVY I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();
  for (uint8_t i=0; i<dataSize; i++)
  {
    mx.setColumn(idx-dataSize+i, iType ? Bocek1[i] : Bocek2[i]);
    mx.setColumn(idx+dataSize-i-1, iType ? Bocek1[i] : Bocek2[i]);
  }
  idx++;
  if (idx == mx.getColumnCount()+(dataSize*2)) bInit = true;
  iType = !iType;
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikPacman(bool bInit)
{
  #define MAX_FRAMES  4   // animasyon kare sayısı
  #define PM_DATA_WIDTH  18
  const uint8_t pacman[MAX_FRAMES][PM_DATA_WIDTH] =  // hayalet tarafından takip edilen bir pacman
  {
    { 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
    { 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
    { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
    { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
  };

  static int16_t idx;        // gösterim indeksi (sütun)
  static uint8_t frame;      // mevcut animasyon karesi
  static uint8_t deltaFrame; // bir sonraki kare için animasyon kare ofseti

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Pacman başlat");
    MatrixiSifirla();
    bInit = false;
    idx = -1; //DATA_WIDTH;
    frame = 0;
    deltaFrame = 1;
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis() - oncekianimasyonzamani < PACMAN_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  PRINT("\nPAC I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();

  // eski grafiği temizle
  for (uint8_t i = 0; i<PM_DATA_WIDTH; i++)
    mx.setColumn(idx - PM_DATA_WIDTH + i, 0);
  // referans sütununu kaydır ve yeni grafiği çiz
  idx++;
  for (uint8_t i = 0; i<PM_DATA_WIDTH; i++)
    mx.setColumn(idx - PM_DATA_WIDTH + i, pacman[frame][i]);

  // animasyon karesini ilerlet
  frame += deltaFrame;
  if (frame == 0 || frame == MAX_FRAMES - 1)
    deltaFrame = -deltaFrame;

  // tamamlandı mı kontrol et ve bir sonraki döngü için başlangıcı ayarla
  if (idx == mx.getColumnCount() + PM_DATA_WIDTH) bInit = true;

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return(bInit);
}

bool grafikDonenOklar(bool bInit)
{
  static uint16_t idx;        // dönüş indeksi

  uint8_t ok[COL_SIZE] =
  {
    0b00000000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
  };

  MD_MAX72XX::transformType_t  t[] =
  {
    MD_MAX72XX::TRC, MD_MAX72XX::TRC,
    MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR, MD_MAX72XX::TSR,
    MD_MAX72XX::TRC, MD_MAX72XX::TRC,
    MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL, MD_MAX72XX::TSL,
    MD_MAX72XX::TRC,
  };

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- OkDöndür başlat");
    MatrixiSifirla();
    bInit = false;
    idx = 0;

    // ok bitmapsini kullan
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (uint8_t j = 0; j<mx.getDeviceCount(); j++)
      mx.setBuffer(((j + 1)*COL_SIZE) - 1, COL_SIZE, ok);
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  }

  // Animasyona başlama zamanı mı kontrol ediyoruz.
  if (millis() - oncekianimasyonzamani < OKLARTERS_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki zaman için başlangıç noktası

  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
  mx.transform(t[idx++]);
  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);

  // tamamlandı mı kontrol et ve bir sonraki döngü için başlangıcı ayarla
  if (idx == (sizeof(t) / sizeof(t[0]))) bInit = true;

  return(bInit);
}

bool grafikSinus(bool bInit)
{
  static uint8_t curDalga = 0;
  static uint8_t idx;

  #define SW_DATA_WIDTH  11    // geçerli veri sayısı, en fazla 10 veri noktasını takip eder
  const uint8_t Dalgalar[][SW_DATA_WIDTH] =
  {
    {  9,   8,  6,   1,   6,  24,  96, 128,  96,  16,   0 },
    {  6,  12,  2,  12,  48,  64,  48,   0,   0,   0,   0 },
    { 10,  12,   2,   1,   2,  12,  48,  64, 128,  64, 48 },
  };
  const uint8_t Dalga_COUNT = sizeof(Dalgalar) / (SW_DATA_WIDTH * sizeof(uint8_t));

  // başlatılıyor mu?
  if (bInit)
  {
    PRINTS("\n--- Sinus başlatma");
    MatrixiSifirla();
    bInit = false;
    idx = 1;
  }

  // animasyonu tekrardan başlatma zamanı mı?
  if (millis() - oncekianimasyonzamani < DALGA_GECIKMESI)
    return(bInit);
  oncekianimasyonzamani = millis();    // bir sonraki sefer için başlangıç noktası

  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
  mx.transform(MD_MAX72XX::TSL);
  mx.setColumn(0, Dalgalar[curDalga][idx++]);
  if (idx > Dalgalar[curDalga][0])
  {
    curDalga = random(Dalga_COUNT);
    idx = 1;
  }
  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);

  return(bInit);
}

// ========== Kontrol rutinleri ===========
//
void MatrixiSifirla(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
  oncekianimasyonzamani = 0;
}

String receivedString;

void CalistirPac(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikPacman(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirKALP(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikKalpatisi(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirSpek(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikSpektrum2(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirSil(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikSilecek(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirBocek(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikBocek(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirSinus(void)
{
  static uint8_t state = 0;
 

 static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
  
    switch (state)
  {
  case  0: bRestart = grafikSinus(bRestart); break;
  default:
    state = 0;
  }
}
  


void CalistirYazi(const String &message)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
   

  switch (state)
  {
  case  0: bInMessages = true; bRestart = YaziKaydirma(bRestart, message); break;
  default:
    state = 0;
  }
}

void CalistirTara(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;
      
    }
    else
     { 

    bRestart = true;
    }
  }
   

  switch (state)
  {
  case  0: bRestart = grafikTarayici(bRestart); break;
  default:
    state = 0;
  }
}
void CalistirRast(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }
  switch (state)
  {
  case  0:bRestart = grafikRastgele(bRestart); break;
  default:
    state = 0;
  }
}
void CalistirKay(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikKaydirici(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirSpek1(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikSpektrum1(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirSol(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikSolma(bRestart); break;
  default:
    state = 0;
  }
}
void CalistirKalp(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikKalpler(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirGoz(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikGozler(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirTop(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikSekentop(bRestart); break;
  default:
    state = 0;
  }
}


void CalistirOk(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikKayanOk(bRestart); break;
  default:
    state = 0;
  }
}

void CalistirDonen(void)
{
  static uint8_t state = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if Calistir_DEMO
  if (millis() - oncekidemozamani >= 1000)
  {
    oncekidemozamani = millis();
    if (--Demozamani == 0)
    {
      Demozamani = Demo_gecikmesi;
      changeState = true;
    }
  }
#else
  changeState = (ks.read() == MD_UISwitch::KEY_PRESS);
#endif

  if (changeState)
  {
    if (bInMessages)
    {
      bInMessages = false;    
    }
    else
     { 
    bRestart = true;
    }
  }  
  switch (state)
  {
  case  0:bRestart = grafikDonenOklar(bRestart); break;
  default:
    state = 0;
  }
}


void setup() {
  Serial.begin(115200);
  SerialBT.begin("TayfunLed");
  Serial.println("Bluetooth ile Bağlantı Bekleniyor...");
  mx.begin();
}

bool CalistirPacman = false;
bool CalistirAnimation = false;
bool CalistirKalpatisi = false;
bool CalistirSin = false;
bool CalistirBoc = false;
bool CalistirSilecek = false;
bool CalistirSpektrum = false;
bool CalistirTarayici = false;
bool CalistirRastgele = false;
bool CalistirKaydirici = false;
bool CalistirSpektrum1 = false;
bool CalistirSolma = false;
bool CalistirKalpler = false;
bool CalistirGozler = false;
bool CalistirSekentop = false;
bool CalistirKayanok = false;
bool CalistirDonenoklar = false;

void loop() {
  if (SerialBT.available()) {
    String receivedChar = SerialBT.readStringUntil('\n');
    receivedString = receivedChar;
    MatrixiSifirla();
     switch (receivedChar.charAt(0)) {
      case '%':
        CalistirPacman = true;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

      case '!':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = true;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

      case '@':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = true;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;  

      case '&':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = true;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break; 

      case '?':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = true;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;  

      case '_':
        CalistirSpektrum = true;
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '-':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = true;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '*':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = true;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '#':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = true;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '/':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = true;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '+':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = true;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '=':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = true;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case ',':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = true;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '.':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = true;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;

        case '(':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = true;
        CalistirDonenoklar = false;
        break;

        case ')':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = true;
        break;

        case '<':
        CalistirPacman = false;
        CalistirAnimation = false;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        mx.clear();
        break;


      

      default:
        CalistirPacman = false;
        CalistirAnimation = true;
        CalistirKalpatisi = false;
        CalistirSin = false;
        CalistirBoc = false;
        CalistirSilecek = false;
        CalistirSpektrum = false;
        CalistirTarayici = false;
        CalistirRastgele = false;
        CalistirKaydirici = false;
        CalistirSpektrum1 = false;
        CalistirSolma = false;
        CalistirKalpler = false;
        CalistirGozler = false;
        CalistirSekentop = false;
        CalistirKayanok = false;
        CalistirDonenoklar = false;
        break;
    }
  
  }
  if (CalistirAnimation) {
    CalistirYazi(receivedString);
  }
  if (CalistirPacman) {
    CalistirPac();
  }
  if (CalistirKalpatisi) {
    CalistirKALP();
  }
  if (CalistirSin) {
    CalistirSinus();
  }
  if (CalistirBoc) {
    CalistirBocek();
  }
  if (CalistirSilecek) {
    CalistirSil();
  }
  if (CalistirSpektrum) {
    CalistirSpek();
  }
  if (CalistirTarayici) {
    CalistirTara();
  }
  if (CalistirRastgele) {
    CalistirRast();
  }
    if (CalistirKaydirici) {
    CalistirKay();
  }
  if (CalistirSpektrum1) {
    CalistirSpek1();
  }
  if (CalistirSolma) {
    CalistirSol();
  }
  if (CalistirKalpler) {
    CalistirKalp();
  }
  if (CalistirGozler) {
    CalistirGoz();
  }
  if (CalistirSekentop) {
    CalistirTop();
  }
  if (CalistirKayanok) {
    CalistirOk();
  }
  if (CalistirDonenoklar) {
    CalistirDonen();
  }
}
