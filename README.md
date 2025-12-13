# FreeRTOS PC Görev Zamanlayıcı Simülasyonu

Bu proje, FreeRTOS gerçek zamanlı işletim sistemi çekirdeğinin bir PC (Linux) ortamında simülasyonunu sunmaktadır. Geliştiricilerin FreeRTOS tabanlı uygulamaları gömülü donanıma yüklemeden önce PC üzerinde geliştirip test etmelerini sağlamak amacıyla oluşturulmuştur.

## Proje İçeriği

Proje, temel FreeRTOS kaynak dosyalarını ve PC üzerinde çalışabilmesi için gerekli taşınabilir (portable) katmanı içermektedir. `src` dizini altında ise, örnek görevlerin ve zamanlayıcı konfigürasyonunun bulunduğu uygulama kodları yer almaktadır.

- `FreeRTOS/`: FreeRTOS kaynak kodları ve başlık dosyaları.
- `FreeRTOS/portable/ThirdParty/GCC/Posix/`: FreeRTOS'un Linux (POSIX) ortamında çalışmasını sağlayan taşınabilir katman.
- `src/`: Uygulama özelindeki kaynak kodları (`main.c`, `scheduler.c`, `tasks1.c`, `FreeRTOSConfig.h`, `heap_3.c`).
## Zamanlayıcı (Scheduler) Tasarımı

Bu projede, FreeRTOS'un görev zamanlama mantığını göstermek amacıyla
dört seviyeli öncelik tabanlı bir görev zamanlayıcı simüle edilmiştir.

### Öncelik Seviyeleri

Zamanlayıcı aşağıdaki dört öncelik seviyesini kullanır:

- **Öncelik 0 – Gerçek Zamanlı Görevler (FCFS)**  
  Bu seviyedeki görevler, First-Come First-Served (FCFS) algoritması ile
  çalıştırılır. Görev başlatıldıktan sonra, tamamlanana kadar kesintiye
  uğramadan yürütülür. Daha düşük öncelikli görevler bu sırada askıya alınır.

- **Öncelik 1–3 – Kullanıcı Görevleri (Geri Beslemeli Kuyruklar)**  
  Kullanıcı görevleri, üç seviyeli çok kademeli geri beslemeli
  zamanlayıcı (Multi-Level Feedback Queue – MLFQ) kullanılarak yürütülür.
  Her görev, **1 saniyelik zaman dilimi (quantum)** boyunca çalıştırılır.

### Öncelik Düşürme Mekanizması

Bir kullanıcı görevi, kendisine ayrılan zaman dilimini kullandıktan sonra
askıya alınır ve bir alt öncelik seviyesine taşınarak uygun kuyruğa tekrar
eklenir. Bu mekanizma, uzun süre çalışan görevlerin sistem kaynaklarını
tek başına tüketmesini engeller.

### Round Robin Davranışı

Tüm kullanıcı görevleri en düşük öncelik seviyesine (öncelik 3) ulaştığında,
zamanlayıcı basit bir Round Robin algoritması gibi davranır. Bu aşamada
görevler sırayla ve eşit zaman dilimleriyle yürütülür.

---

## Görev Yaşam Döngüsü

Bir görevin sistem içindeki yaşam döngüsü aşağıdaki adımlardan oluşur:

1. Görevler, `giris.txt` dosyasından varış zamanı, öncelik ve gerekli
   işlem süresi bilgileri ile sisteme alınır.
2. Varış zamanı gelen görev, uygun öncelik kuyruğuna eklenir.
3. Gerçek zamanlı görevler doğrudan çalıştırılır.
4. Kullanıcı görevleri, zaman dilimi sonunda askıya alınır ve öncelikleri
   düşürülerek tekrar kuyruğa eklenir.
5. Görev, gerekli işlem süresini tamamladığında sonlandırılır.
6. Bir görev, **20 saniyeden uzun süre sistemde beklerse** zaman aşımı
   nedeniyle sonlandırılır.

Görevlerin terminal çıktıları, her görev için rastgele atanmış renkler ile
gösterilerek zamanlama davranışının görsel olarak izlenmesi sağlanır.

## Kurulum ve Çalıştırma

Projenin derlenebilmesi için aşağıdakilerin yüklü olması gerekir.

- gcc
- make

### Derleme

Projenin derlenmesi için ana dizinde aşağıdaki komutu çalıştırın:

```bash
make
```

Bu komut, tüm kaynak kodları derleyecek ve `freertos_sim` adında çalıştırılabilir bir dosya oluşturacaktır.

### Çalıştırma

Derleme başarılı olduktan sonra, uygulamayı aşağıdaki komutla çalıştırabilirsiniz ("giris.txt" dosyasi yerine kendi dosyanizla da calistirabilirsiniz.):

```bash
./freertos_sim giris.txt
```

### Temizleme

Derleme sırasında oluşturulan tüm nesne dosyalarını ve çalıştırılabilir dosyayı silmek için aşağıdaki komutu kullanabilirsiniz:

```bash
make clean
```
