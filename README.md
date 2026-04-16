# Inventory Management System

Bu proje COME 202 - Object Oriented Programming II dersi icin hazirladigim bir Inventory Management System calismasidir. Ben projeyi bilincli olarak sade tuttum. Amacim karmasik bir yazilim yazmak degil, derste istenen OOP konularini calisan bir C++ uygulamasi uzerinde gostermek oldu.

## Proje Hakkinda Kisa Bilgi

Bu sistemde urun ekleme, urun silme, satis yapma, stok artirma, dusuk stok kontrolu ve islem gecmisi goruntuleme islemleri var. Yapida elektronik, gida ve ofis malzemesi gibi farkli urun turleri kullandim.

## Neleri Gosteriyor

- Kalitim yapisi
- Polimorfizm
- Virtual fonksiyon kullanimi
- Constructor ve destructor mantigi
- Function overloading
- Const metotlar
- Static data member ve static fonksiyon
- Friend function kullanimi
- Pointer kullanimi
- Dynamic memory allocation (`new` / `delete`)
- Basit composition / aggregation mantigi

## Klasor Yapisi

```text
.
├── CMakeLists.txt
├── README.md
├── README_EN.md
├── .env.example
├── data/
├── include/
│   ├── Inventory.hpp
│   └── models.hpp
└── src/
    ├── Inventory.cpp
    └── main.cpp
```

## Program Nasıl Calisiyor?

Program acildiginda ilk olarak kayitli urun dosyasini okumaya calisiyor. Dosya yoksa otomatik olarak demo urunler ile basliyor. Sonra menu uzerinden su islemler yapiliyor:

1. Urun ekleme
2. Urunleri listeleme
3. Satis yapma
4. Stok artirma
5. Urun silme
6. Dusuk stok raporu
7. Islem gecmisi
8. Ozet ekran
9. Kaydedip cikma

## Derleme ve Calistirma

### C++ ile dogrudan

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/Inventory.cpp -o inventory_system
./inventory_system
```

### CMake ile

```bash
cmake -S . -B build
cmake --build build
./build/inventory_system
```

## Veri Saklama

Urunler `inventory_data.db` dosyasinda saklaniyor. Bu dosya git'e eklenmedi, cunku program calisirken olusan yerel veri dosyasi olarak dusunuldu.

## Supabase Degiskenleri

Ben bu projede Supabase'i aktif olarak kullanmadim, ama ileride web tarafina baglanabilir diye env dosyasini hazir tuttum.

- `SUPABASE_PUBLISHABLE_KEY`
- `SUPABASE_URL`

Ornek degerler `.env.example` dosyasinda var.

## Supabase SQL Kurulumu (Yapistirilacak Dosya)

Supabase tarafinda yapistiracagin SQL dosyasi:

- [supabase/schema.sql](supabase/schema.sql)

Adimlar:

1. Supabase projesini ac.
2. Soldan SQL Editor bolumune gir.
3. New Query sec.
4. [supabase/schema.sql](supabase/schema.sql) dosyasinin tamamini kopyalayıp query alanina yapistir.
5. Run tusuna bas.

Bu script su yapilari olusturur:

- `products` tablosu
- `stock_transactions` tablosu
- `v_low_stock` view'i
- Ornek seed veriler

## Rapor Icın Kullanilabilecek Basliklar

- Sistem tanitimi
- Amaç ve kapsam
- Sinif diyagrami
- Kalitim yapisi
- Constructor / destructor kullanimi
- Static ve friend yapi
- Pointer ve dynamic memory ornegi
- Ekran goruntuleri ile demonstrasyon

## Son Not

Bu repo bilerek cok profesyonel gosterilmedi. Ders projesi gibi dursun, ama calissin diye bu sekilde hazirladim.
