# FreeRTOS PC Görev Zamanlayıcı Simülasyonu

Bu proje, FreeRTOS gerçek zamanlı işletim sistemi çekirdeğinin bir PC (Linux) ortamında simülasyonunu sunmaktadır. Geliştiricilerin FreeRTOS tabanlı uygulamaları gömülü donanıma yüklemeden önce PC üzerinde geliştirip test etmelerini sağlamak amacıyla oluşturulmuştur.

## Proje İçeriği

Proje, temel FreeRTOS kaynak dosyalarını ve PC üzerinde çalışabilmesi için gerekli taşınabilir (portable) katmanı içermektedir. `src` dizini altında ise, örnek görevlerin ve zamanlayıcı konfigürasyonunun bulunduğu uygulama kodları yer almaktadır.

- `FreeRTOS/`: FreeRTOS kaynak kodları ve başlık dosyaları.
- `FreeRTOS/portable/ThirdParty/GCC/Posix/`: FreeRTOS'un Linux (POSIX) ortamında çalışmasını sağlayan taşınabilir katman.
- `src/`: Uygulama özelindeki kaynak kodları (`main.c`, `scheduler.c`, `tasks1.c`, `FreeRTOSConfig.h`, `heap_3.c`).

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
